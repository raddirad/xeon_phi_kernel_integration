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
};

struct scifmodule a;

static int __init scif_start(void)
{
	printk(KERN_INFO "Loading scif module...\n");
	printk(KERN_INFO "\n");
	a.len = 32*PAGE_SIZE;

	size_t ret;
	a.port = 23969;
	a.endpoint = scif_open();
	if (a.endpoint == NULL)
	{ 
		printk(KERN_INFO "scif_open failed\n");
		goto exit;
	}

	printk(KERN_INFO "scif_open successfull\n");
	struct scif_portID portid;
	ret = scif_bind(a.endpoint, a.port);
	if (ret != a.port)
	{
		printk(KERN_INFO "scif_bind failed due to: %zu\n", ret);
		goto exit;
	}
	printk(KERN_INFO "scif_bind successfull\n");

	ret = scif_listen(a.endpoint, 4);
	if (ret != 0)
	{
		printk(KERN_INFO "scif_listen failed due to: %zu\n", ret);
		goto exit;
	}
	printk(KERN_INFO "scif_listen successfull\n");

	ret = scif_accept(a.endpoint, &portid, &a.remote_endpoint, SCIF_ACCEPT_SYNC);
	if (ret != 0)
	{
		printk(KERN_INFO "scif_accept failed due to: %zu\n", ret);
		goto exit;
	}
	printk(KERN_INFO "scif_accept successfull\n");

	a.ptr = kmalloc(32*PAGE_SIZE, GFP_KERNEL);
	if (!a.ptr)
	{
		printk(KERN_INFO "PTR allocation failed\n");
		goto exit;
	} 
	printk(KERN_INFO "allocating ptr successfull\n");

	get_random_bytes(a.ptr, a.len);
	uint8_t i;
	for (i=0; i<3; i++)
	{
		printk("%d, ", a.ptr[i]);
	}
	printk("\n");
	a.return_ptr = kmalloc(32*PAGE_SIZE, GFP_KERNEL);
	if (!a.return_ptr)
	{
		printk(KERN_INFO "return_PTR allocation failed\n");
		goto exit;
	}
	printk(KERN_INFO "allocating return_ptr successfull\n");

	off_t regreturn;
	regreturn =0;
	printk("initial regreturn: %lx\n", (long)regreturn);
	regreturn = scif_register(a.remote_endpoint, a.ptr, a.len, 0, SCIF_PROT_READ, SCIF_MAP_KERNEL);
	printk("regreturn: %lx\n", (long)regreturn);
	if (regreturn != (off_t)a.ptr)
        {
        	printk(KERN_INFO "scif_register ptr failed due to: %llx\n", (long long)regreturn);
		goto exit;
        }
        printk(KERN_INFO "scif_register of ptr successfull\n");

        regreturn = scif_register(a.remote_endpoint, a.return_ptr, a.len, (off_t)a.return_ptr, SCIF_PROT_WRITE, SCIF_MAP_KERNEL);
	if (regreturn != (off_t)a.return_ptr)
        {
                printk(KERN_INFO "scif_register of return_ptr failed due to: %zu\n", regreturn);
		goto exit;
        }
        printk(KERN_INFO "scif_register of return_ptr successfull\n");

	printk("regreturn: %lx\n", (long)regreturn);
	off_t *loffset = regreturn;
	printk("loffset %lxd\n", (long)loffset);

	printk("sending over %p\n", a.ptr);	
	ret = scif_send(a.remote_endpoint, &a.ptr, sizeof(long), SCIF_SEND_BLOCK);
	if (ret != sizeof(long))
	{
		printk(KERN_INFO "scif_send failed due to %zu\n", ret);
		goto exit;
	}
	printk(KERN_INFO "scif_send of ptr successfull\n");
	
	printk("sending over %p\n", a.return_ptr);	
	ret = scif_send(a.remote_endpoint, &a.return_ptr, sizeof(long), SCIF_SEND_BLOCK);
	if (ret != sizeof(long))
	{
		printk(KERN_INFO "scif_send of return_ptr failed due to %zu\n", ret);
		goto exit;
	}	
	printk(KERN_INFO "scif_send of return_ptr successfull\n");
	ret =0;

	ret = scif_recv(a.remote_endpoint, &a.placeholder, sizeof(void*), SCIF_RECV_BLOCK);
	if (ret != sizeof(void*))
	{
		printk(KERN_INFO "scif_recv of placeholder failed due to %zu\n", ret);
		goto exit;
	}	
	
	printk(KERN_INFO "MIC Code finished\n");
	return 0;

exit:
	scif_unregister(a.remote_endpoint, (off_t)a.ptr, a.len);
        scif_unregister(a.remote_endpoint, (off_t)a.return_ptr, a.len);
        kfree(a.return_ptr);
        kfree(a.ptr);
        kfree(a.placeholder);
        scif_close(a.endpoint);
	return -1;
}
static void __exit scif_end(void)
{	
     	scif_unregister(a.remote_endpoint, (off_t)a.ptr, a.len);
        scif_unregister(a.remote_endpoint, (off_t)a.return_ptr, a.len);
        kfree(a.return_ptr);
        kfree(a.ptr);
	kfree(a.placeholder);
        scif_close(a.endpoint);

	printk(KERN_INFO "Scif finished....\n");
}
module_init(scif_start);
module_exit(scif_end);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("M. Radtke");
