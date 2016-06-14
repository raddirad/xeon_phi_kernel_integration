#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for the macros */
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/random.h>
#include <linux/delay.h>

#include "include/mic/micscif.h"

struct scifmodule {
	size_t len;
	scif_epd_t endpoint;
        scif_epd_t remote_endpoint;
	size_t port;
	uint8_t *ptr;
	uint8_t *return_ptr;
	uint8_t *placeholder;
	off_t* send_offset0;
	off_t* send_offset1;
	off_t regreturn0;
	off_t regreturn1;
};

struct scifmodule dma;

static int __init scif_start(void)
{
	printk(KERN_INFO "Loading scif module...\n");
	printk(KERN_INFO "\n");
	dma.len = 262144*PAGE_SIZE;

	size_t ret;
	dma.port = 23969;
	dma.endpoint = scif_open();
	if (dma.endpoint == NULL)
	{ 
		printk(KERN_INFO "scif_open failed\n");
		goto exit;
	}

	printk(KERN_INFO "scif_open successfull\n");
	struct scif_portID portid;
	ret = scif_bind(dma.endpoint, dma.port);
	if (ret != dma.port)
	{
		printk(KERN_INFO "scif_bind failed due to: %zu\n", ret);
		goto exit;
	}
	printk(KERN_INFO "scif_bind successfull\n");

	ret = scif_listen(dma.endpoint, 4);
	if (ret != 0)
	{
		printk(KERN_INFO "scif_listen failed due to: %zu\n", ret);
		goto exit;
	}
	printk(KERN_INFO "scif_listen successfull\n");

	ret = scif_accept(dma.endpoint, &portid, &dma.remote_endpoint, SCIF_ACCEPT_SYNC);
	if (ret != 0)
	{
		printk(KERN_INFO "scif_accept failed due to: %zu\n", ret);
		goto exit;
	}
	printk(KERN_INFO "scif_accept successfull\n");

	dma.ptr = vmalloc(dma.len);
	if (!dma.ptr)
	{
		printk(KERN_INFO "ptr allocation failed\n");
		goto exit;
	} 
	printk(KERN_INFO "allocating ptr successfull\n");

	get_random_bytes(dma.ptr, dma.len);

	dma.return_ptr = vmalloc(dma.len);
	if (!dma.return_ptr)
	{
		printk(KERN_INFO "return_PTR allocation failed\n");
		goto exit;
	}
	printk(KERN_INFO "allocating return_ptr successfull\n");

	dma.regreturn0 = scif_register(dma.remote_endpoint, dma.ptr, dma.len, (off_t)dma.ptr, SCIF_PROT_READ, SCIF_MAP_KERNEL);
	dma.send_offset0 = dma.regreturn0;
	scif_send(dma.remote_endpoint, &dma.send_offset0, sizeof(off_t*),SCIF_SEND_BLOCK); 

        dma.regreturn1 = scif_register(dma.remote_endpoint, dma.return_ptr, dma.len, (off_t)dma.return_ptr, SCIF_PROT_WRITE, SCIF_MAP_KERNEL);
	dma.send_offset1 = dma.regreturn1;
	scif_send(dma.remote_endpoint, &dma.send_offset1, sizeof(off_t*),SCIF_SEND_BLOCK);

	ret = scif_recv(dma.remote_endpoint, &dma.placeholder, sizeof(void*), SCIF_RECV_BLOCK);
	if (ret != sizeof(void*))
	{
		printk(KERN_INFO "scif_recv of placeholder failed due to %zu\n", ret);
		goto exit;
	}	

	if (memcmp(dma.ptr, dma.return_ptr, dma.len) != 0)
	{ 
		printk("In & outbuffer arent equal\n"); 
		return 1;
	}
	printk("Buffers are equal\n");

	
	printk(KERN_INFO "MIC Code finished\n");
	return 0;

exit:
	scif_unregister(dma.remote_endpoint, dma.regreturn0, dma.len);
        scif_unregister(dma.remote_endpoint, dma.regreturn1, dma.len);
        vfree(dma.return_ptr);
        vfree(dma.ptr);
        scif_close(dma.endpoint);
	return -1;
}
static void __exit scif_end(void)
{	
     	scif_unregister(dma.remote_endpoint, dma.regreturn0, dma.len);
        scif_unregister(dma.remote_endpoint, dma.regreturn1, dma.len);
        vfree(dma.return_ptr);
        vfree(dma.ptr);
        scif_close(dma.endpoint);

	printk(KERN_INFO "Scif finished....\n");
}
module_init(scif_start);
module_exit(scif_end);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("M. Radtke");
