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

struct scifmodule latency;

static int __init scif_start(void)
{
	printk(KERN_INFO "Loading scif module...\n");
	printk(KERN_INFO "\n");
	latency.len = 1*PAGE_SIZE;

	size_t ret;
	latency.port = 23969;
	latency.endpoint = scif_open();
	if (latency.endpoint == NULL)
	{ 
		printk(KERN_INFO "scif_open failed\n");
		goto exit;
	}

	printk(KERN_INFO "scif_open successfull\n");
	struct scif_portID portid;
	ret = scif_bind(latency.endpoint, latency.port);
	if (ret != latency.port)
	{
		printk(KERN_INFO "scif_bind failed due to: %zu\n", ret);
		goto exit;
	}
	printk(KERN_INFO "scif_bind successfull\n");

	ret = scif_listen(latency.endpoint, 4);
	if (ret != 0)
	{
		printk(KERN_INFO "scif_listen failed due to: %zu\n", ret);
		goto exit;
	}
	printk(KERN_INFO "scif_listen successfull\n");

	ret = scif_accept(latency.endpoint, &portid, &latency.remote_endpoint, SCIF_ACCEPT_SYNC);
	if (ret != 0)
	{
		printk(KERN_INFO "scif_accept failed due to: %zu\n", ret);
		goto exit;
	}
	printk(KERN_INFO "scif_accept successfull\n");

	latency.ptr = vmalloc(latency.len);
	if (!latency.ptr)
	{
		printk(KERN_INFO "PTR allocation failed\n");
		goto exit;
	} 
	printk(KERN_INFO "allocating ptr successfull\n");

	get_random_bytes(latency.ptr, latency.len);

	latency.return_ptr = vmalloc(latency.len);
	if (!latency.return_ptr)
	{
		printk(KERN_INFO "return_PTR allocation failed\n");
		goto exit;
	}
	printk(KERN_INFO "allocating return_ptr successfull\n");

	latency.regreturn0 = scif_register(latency.remote_endpoint, latency.ptr, latency.len, 0, SCIF_PROT_READ, SCIF_MAP_KERNEL);

	latency.send_offset0 = latency.regreturn0;
	scif_send(latency.remote_endpoint, &latency.send_offset0, sizeof(off_t*),SCIF_SEND_BLOCK); 

        latency.regreturn1 = scif_register(latency.remote_endpoint, latency.return_ptr, latency.len, latency.return_ptr, SCIF_PROT_WRITE, SCIF_MAP_KERNEL);

	latency.send_offset1 = latency.regreturn1;
	scif_send(latency.remote_endpoint, &latency.send_offset1, sizeof(off_t*),SCIF_SEND_BLOCK);

	ret = scif_recv(latency.remote_endpoint, &latency.placeholder, sizeof(void*), SCIF_RECV_BLOCK);
	if (ret != sizeof(void*))
	{
		printk(KERN_INFO "scif_recv of placeholder failed due to %zu\n", ret);
		goto exit;
	}	

	printk(KERN_INFO "MIC Code finished\n");
	return 0;

exit:
	scif_unregister(latency.remote_endpoint, latency.regreturn0, latency.len);
        scif_unregister(latency.remote_endpoint, latency.regreturn1, latency.len);
        vfree(latency.return_ptr);
        vfree(latency.ptr);
        scif_close(latency.endpoint);
	return -1;
}
static void __exit scif_end(void)
{	
     	scif_unregister(latency.remote_endpoint, latency.regreturn0, latency.len);
        scif_unregister(latency.remote_endpoint, latency.regreturn1, latency.len);
        vfree(latency.return_ptr);
        vfree(latency.ptr);
        scif_close(latency.endpoint);

	printk(KERN_INFO "Scif finished....\n");
}
module_init(scif_start);
module_exit(scif_end);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("M. Radtke");
