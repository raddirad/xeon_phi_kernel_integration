#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for the macros */
#include <linux/slab.h>

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

struct scifmodule latency;

static int __init scif_start(void)
{
	printk(KERN_INFO "Loading scif module...\n");
	printk(KERN_INFO "\n");
	latency.len = 1;
	uint32_t loops = 50000;

	size_t ret;
	latency.port = 23968;
	latency.endpoint = scif_open();
	if (latency.endpoint == NULL)
	{ 
		printk(KERN_INFO "scif_open failed\n");
		goto exit;                
	}

	printk("scif_open successfull\n");
	struct scif_portID portid;
	ret = scif_bind(latency.endpoint, latency.port);
	if (ret != latency.port)
	{
		printk(KERN_INFO "scif_bind failed due to: %d\n", ret);
		goto exit;
	}
	printk(KERN_INFO "scif_bind successfull\n");

	ret = scif_listen(latency.endpoint, 4);
	if (ret != 0)
	{
		printk(KERN_INFO "scif_listen failed due to: %d\n", ret);
		goto exit;
	}
	printk(KERN_INFO "scif_listen successfull\n");

	ret = scif_accept(latency.endpoint, &portid, &latency.remote_endpoint, SCIF_ACCEPT_SYNC);
	if (ret != 0)
	{
		printk(KERN_INFO "scif_accept failed due to: %d\n", ret);
		goto exit;
	}
	printk(KERN_INFO "scif_accept successfull\n");
	
	latency.ptr = kmalloc(latency.len,__GFP_NORETRY);
	if (!latency.ptr)
	{
		printk(KERN_INFO "PTR allocation failed\n");
		goto exit;
	} 
	printk(KERN_INFO "allocating ptr successfull\n");

	latency.return_ptr = kmalloc(latency.len,__GFP_NORETRY);
	if (!latency.return_ptr)
	{
		printk(KERN_INFO "return_PTR allocation failed\n");
		goto exit;
	}
	printk(KERN_INFO "allocating return_ptr successfull\n");
	int i;	
	for (i =0; i<loops; i++)
	{
		scif_send(latency.remote_endpoint, &latency.ptr[0], latency.len, SCIF_SEND_BLOCK);
		scif_recv(latency.remote_endpoint, latency.ptr, latency.len, SCIF_RECV_BLOCK);
	}
	printk(KERN_INFO "MIC Code finished\n");
	return 0;

	exit:
		kfree(latency.return_ptr);
		kfree(latency.ptr);
		kfree(latency.placeholder);
		scif_close(latency.endpoint);
		return -1;
}
static void __exit scif_end(void)
{
        kfree(latency.return_ptr);
        kfree(latency.ptr);
	kfree(latency.placeholder);
        scif_close(latency.endpoint);

	printk(KERN_INFO "Scif finished....\n");
}
module_init(scif_start);
module_exit(scif_end);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("M. Radtke");
