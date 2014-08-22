/*

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.


   Copyright (C) 2011-2012  Huawei Corporation
*/

#include <linux/module.h>	/* kernel module definitions */
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/notifier.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>

#include <linux/irq.h>
#include <linux/param.h>
#include <linux/termios.h>

#include <net/bluetooth/bluetooth.h>
#include <asm/uaccess.h>
#include <hsad/config_interface.h>

#define BT_DEVICE_DBG
#ifndef BT_DEVICE_DBG
#define BT_DBG(fmt, arg...)
#endif

#define LOG_TAG "FeatureTransfer"
/*
 * Defines
 */

#define VERSION		"1.0"
#define PROC_DIR	"device_feature"
#define MAX_BT_FEATURE_LEN 20
#define MAX_BT_FW_VER_LEN 128
struct proc_dir_entry *device_dir, *bt_dir, *fm_dir;


static int featuretransfer_remove(struct platform_device *pdev)
{
	BT_INFO("devicefeature removed.");
	return 0;
}

static struct platform_driver featuretransfer_driver = {
	.remove = featuretransfer_remove,
	.driver = {
		.name = "featuretransfer",
		.owner = THIS_MODULE,
	},
};


/* --------------------------- BT Device Type --------------------------*/
struct bt_fm_device
{
    bt_fm_device_type chip_type;
    char *dev_name;
};

/* Bt chip name defination.  */
const struct bt_fm_device bt_fm_device_array[] = 
{
	{ BT_FM_BROADCOM_BCM4330, "1.2" },
	{ BT_FM_QUALCOMM_WCN3660, "2.2" },
	{ BT_FM_UNKNOWN_DEVICE, "Unknown" }
};

/**
 * Get bt/fm device type.
 * @param no.
 * @return On success, the number of bytes written. On error, -1, and
 * <code>errno</code> is set appropriately.
 */
static char *get_bt_fm_device_name(void)
{  
	int i = 0;
	int arry_size = sizeof(bt_fm_device_array)/sizeof(bt_fm_device_array[0]);
    int bt_fm_chip_type = (int)BT_FM_UNKNOWN_DEVICE;

	/* get bt/fm chip type from the device feature configuration (.xml file) */
    bt_fm_chip_type = get_bt_fm_device_type();
	if(-1 == bt_fm_chip_type)
	{
		printk("BT-FM, Get chip type fail.\n");
		return bt_fm_device_array[arry_size - 1].dev_name;
	}
	
    /* lookup bt_device_model in bt_device_array[] */
    for(i = 0; i < arry_size; i++)
    {
        if(bt_fm_chip_type == bt_fm_device_array[i].chip_type)
        {
            break; 
        }
    }
	/* If we get invalid type name, return "Unknown".*/
	if( i == arry_size)
	{
		printk("BT-FM, Get chip type fail.\n");
		return bt_fm_device_array[arry_size - 1].dev_name;
	}

	return bt_fm_device_array[i].dev_name;
}


/**
 * Read the <code>BT_WAKE</code> GPIO pin value via the proc interface.
 * When this function returns, <code>page</code> will contain a 1 if the
 * pin is high, 0 otherwise.
 * @param page Buffer for writing data.
 * @param start Not used.
 * @param offset Not used.
 * @param count Not used.
 * @param eof Whether or not there is more data to be read.
 * @param data Not used.
 * @return The number of bytes written.
 */
static int devicefeature_read_proc_chiptype(char *page, char **start, off_t offset,
					int count, int *eof, void *data)
{
    char *bt_fm_device_name = NULL;
    
    *eof = 1;
	
    bt_fm_device_name = get_bt_fm_device_name();
	/* Only the chip type info such as 1.2 or 2.2 is recorded. */
    return sprintf(page, "%s", bt_fm_device_name);
}
/**
 * Write the <code>product_id</code> via the proc interface.
 * @param file Not used.
 * @param buffer The buffer to read from.
 * @param count The number of bytes to be written.
 * @param data Not used.
 * @return On success, the number of bytes written. On error, -1, and
 * <code>errno</code> is set appropriately.
 */
static int devicefeature_write_proc_product_id(struct file *file, const char *buffer,
					unsigned long count, void *data)
{
	/* the function is not used now.  */
	return 0;
}

static int devicefeature_read_proc_product_id(char *page, char **start, off_t offset,
					int count, int *eof, void *data)
{
    int ret = -1;
    char bt_product_id[MAX_BT_FEATURE_LEN] = {'\0'};
    
    *eof = 1;
	
    ret = get_bt_product_id(bt_product_id,MAX_BT_FEATURE_LEN);
    if(-1 == ret)
    {
        BT_ERR("Get Bt product id failed.\n");
        strncpy(bt_product_id,"Unknown",sizeof("Unknown"));
    }
	/* Only the chip type info such as 1.2 or 2.2 is recorded. */
    return sprintf(page, "%s", bt_product_id);
}
/**
 * Write the <code>BT_WAKE</code> GPIO pin value via the proc interface.
 * @param file Not used.
 * @param buffer The buffer to read from.
 * @param count The number of bytes to be written.
 * @param data Not used.
 * @return On success, the number of bytes written. On error, -1, and
 * <code>errno</code> is set appropriately.
 */
static int devicefeature_write_proc_chiptype(struct file *file, const char *buffer,
					unsigned long count, void *data)
{
	/* the function is not used now.  */
	return 0;
}

/**
 * Read the <code>BT_WAKE</code> GPIO pin value via the proc interface.
 * When this function returns, <code>page</code> will contain a 1 if the
 * pin is high, 0 otherwise.
 * @param page Buffer for writing data.
 * @param start Not used.
 * @param offset Not used.
 * @param count Not used.
 * @param eof Whether or not there is more data to be read.
 * @param data Not used.
 * @return The number of bytes written.
 */
static int devicefeature_read_proc_sinr(char *page, char **start, off_t offset,
					int count, int *eof, void *data)
{
	int sinr = FM_SINR_MAX;
	*eof = 1;
	
	sinr = get_fm_sinr();
	if(-1 == sinr)
	{
		// 7 is the default value
		BT_ERR("Get FM SINR failed and will use default value 7.\n");
		sinr = FM_SINR_7;
	}

	return sprintf(page, "%d\n",sinr);
}
/**
 * Write the <code>BT_WAKE</code> GPIO pin value via the proc interface.
 * @param file Not used.
 * @param buffer The buffer to read from.
 * @param count The number of bytes to be written.
 * @param data Not used.
 * @return On success, the number of bytes written. On error, -1, and
 * <code>errno</code> is set appropriately.
 */
static int devicefeature_write_proc_sinr(struct file *file, const char *buffer,
					unsigned long count, void *data)
{
	/* devicefeature_write_proc_sinr function is not used now.*/
	return 0;
}

/* Get FM RSSI threshold and write to the node */
static int devicefeature_read_proc_rssi(char *page, char **start, off_t offset,
					int count, int *eof, void *data)
{
	int rssi = FM_RSSI_MAX;
	*eof = 1;
	
	rssi = get_fm_rssi();
	if(-1 == rssi)
	{
		// 100 is the default value
		BT_ERR("Get FM RSSI failed and will use default value 100.\n");
		rssi = FM_RSSI_100;
	}

	return sprintf(page, "%d\n",rssi);
}

static int devicefeature_write_proc_rssi(struct file *file, const char *buffer,
					unsigned long count, void *data)
{
	/* devicefeature_write_proc_rssi function is not used now.*/
	return 0;
}

/* Get BT/FM firmware version and write to the node */
static int devicefeature_read_proc_bt_fm_fw_ver(char *page, char **start, off_t offset,
					int count, int *eof, void *data)
{
    int ret = -1;
    char bt_fm_fw_ver[MAX_BT_FW_VER_LEN] = {'\0'};
    
    *eof = 1;
	
    ret = get_bt_fm_fw_ver(bt_fm_fw_ver,MAX_BT_FW_VER_LEN);
    if(-1 == ret)
    {
        BT_ERR("Get Bt fw_ver  failed.\n");
        strncpy(bt_fm_fw_ver,"Unknown",sizeof("Unknown"));
    }
    
    return sprintf(page, "%s", bt_fm_fw_ver);
}

/**
 * Initializes the module.
 * @return On success, 0. On error, -1, and <code>errno</code> is set
 * appropriately.
 */
static int __init featuretransfer_init(void)
{
	int retval = 0;
	struct proc_dir_entry *ent = NULL;

	BT_ERR("BT DEVICE FEATURE VERSION: %s", VERSION);

	/* Driver Register */
	retval = platform_driver_register(&featuretransfer_driver);
	if (0 != retval)
	{
		BT_ERR("[%s],featurntransfer driver register fail.",LOG_TAG);
		return retval;
	}

	/* create device_feature directory for bt chip info */
	device_dir = proc_mkdir("device_feature", NULL);
	if (NULL == device_dir)
	{
		BT_ERR("Unable to create /proc/device_feature directory");
		return -ENOMEM;
	}

	/* create bt_feature for bluetooth feature */
	bt_dir = proc_mkdir("bt_feature", device_dir);
	if (NULL == bt_dir)
	{
		BT_ERR("Unable to create /proc/%s directory", PROC_DIR);
		return -ENOMEM;
	}

	/* Creating read/write "chiptype" entry for bluetooth chip type*/
	ent = create_proc_entry("chiptype", 0, bt_dir);
	if (NULL == ent) 
	{
		BT_ERR("Unable to create /proc/%s/chiptype entry", PROC_DIR);
		retval = -ENOMEM;
		goto fail;
	}
	ent->read_proc = devicefeature_read_proc_chiptype;
	ent->write_proc = devicefeature_write_proc_chiptype;
	/* Creating read/write "product_id" entry for bluetooth chip type*/
	ent = create_proc_entry("product_id", 0, bt_dir);
	if (NULL == ent) 
	{
		BT_ERR("Unable to create /proc/%s/product_id entry", PROC_DIR);
		retval = -ENOMEM;
		goto fail;
	}
	ent->read_proc = devicefeature_read_proc_product_id;
	ent->write_proc = devicefeature_write_proc_product_id;
    /*Creating read/write "bt fm fw_ver" entry for bluetooth chip type*/
    ent = create_proc_entry("bt_fm_fw_ver", 0, bt_dir);
	if (NULL == ent) 
	{
		BT_ERR("Unable to create /proc/%s/bt_fm_fw_ver entry", PROC_DIR);
		retval = -ENOMEM;
		goto fail;
	}
	ent->read_proc = devicefeature_read_proc_bt_fm_fw_ver;
	/* create fm_feature for fm feature */
	fm_dir = proc_mkdir("fm_feature", device_dir);
	if (NULL == bt_dir)
	{
		BT_ERR("Unable to create /proc/%s directory", PROC_DIR);
		return -ENOMEM;
	}

	/* Creating read/write "sinr" entry for bluetooth chip type*/
	ent = create_proc_entry("sinr", 0, fm_dir);
	if (NULL == ent) 
	{
		BT_ERR("Unable to create /proc/%s/sinr entry", PROC_DIR);
		retval = -ENOMEM;
		goto fail;
	}
	ent->read_proc = devicefeature_read_proc_sinr;
	ent->write_proc = devicefeature_write_proc_sinr;

	/* Creating read/write "rssi" entry for bcm4330 fm*/
	ent = create_proc_entry("rssi", 0, fm_dir);
	if (NULL == ent) 
	{
		BT_ERR("Unable to create /proc/%s/rssi entry", PROC_DIR);
		retval = -ENOMEM;
		goto fail;
	}
	ent->read_proc = devicefeature_read_proc_rssi;
	ent->write_proc = devicefeature_write_proc_rssi;
	
	return 0;

fail:
	remove_proc_entry("chiptype", bt_dir);
	remove_proc_entry("bt_fm_fw_ver", bt_dir);
	remove_proc_entry("sinr", fm_dir);
    remove_proc_entry("rssi", fm_dir);
	remove_proc_entry("bt_feature", device_dir);
	remove_proc_entry("fm_feature", device_dir);
	remove_proc_entry("device_feature", 0);
	return retval;
}


/**
 * Cleans up the module.
 */
static void __exit featuretransfer_exit(void)
{
	platform_driver_unregister(&featuretransfer_driver);

	remove_proc_entry("chiptype", bt_dir);
	remove_proc_entry("bt_fm_fw_ver", bt_dir);
	remove_proc_entry("sinr", fm_dir);
	remove_proc_entry("rssi", fm_dir);
	remove_proc_entry("bt_feature", device_dir);
	remove_proc_entry("fm_feature", device_dir);
	remove_proc_entry("device_feature", 0);
}



module_init(featuretransfer_init);
module_exit(featuretransfer_exit);

MODULE_DESCRIPTION("BT DEVICE FEATURE VERSION: %s " VERSION);
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif
