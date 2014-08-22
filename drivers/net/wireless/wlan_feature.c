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
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/platform_device.h>
#include <hsad/config_interface.h>

#define WLAN_DEVICE_VERSION		"1.0"
#define PROC_DIR	"wlan_feature"
#define WLAN_MAX_PUBFILE_ID_LEN    128
#define MAX_WLAN_FW_VER_LEN 128


struct proc_dir_entry    *wlan_dir, *devtype_dir, *pubfd_dir;

static int wlan_featuretrans_remove(struct platform_device *pdev)
{
    printk("wlan devicefeature removed.");
    return 0;
}

static struct platform_driver wlan_featuretrans_driver = {
    .remove = wlan_featuretrans_remove,
    .driver = {
        .name = "wlanfeaturetrans",
        .owner = THIS_MODULE,
    },
};

/* --------------------------- Wifi Device Type --------------------------*/
struct wifi_device
{
    hw_wifi_device_type chip_type;
    char *dev_name;
};

/* wifi chip name defination.  */
/*modify the chip type*/
const struct wifi_device wifi_device_array[] = 
{
    { WIFI_BROADCOM_4330, "1.2" },
    { WIFI_BROADCOM_4330X, "1.3" },
    { WIFI_QUALCOMM_WCN3660, "2.2" },
    { WIFI_TYPE_UNKNOWN, "Unknown" }
};

/**
 * Get wifi device type.
 * @param no.
 * @return On success, the number of bytes written. On error, -1, and
 * <code>errno</code> is set appropriately.
 */
char *get_wifi_device_type(void)
{  
    int i = 0;
    int arry_size = sizeof(wifi_device_array)/sizeof(wifi_device_array[0]);
    int wifi_chip_type = (int)WIFI_TYPE_UNKNOWN;

    /* get wifi chip type from the device feature configuration (.xml file) */
    wifi_chip_type = get_hw_wifi_device_type();
    if(-1 == wifi_chip_type)
    {
        printk("WIFI, Get chip type fail.\n");
        return wifi_device_array[arry_size - 1].dev_name;
    }
	
    /* lookup wifi_device_model in wifi_device_array[] */
    for(i = 0; i < arry_size; i++)
    {
        if(wifi_chip_type == wifi_device_array[i].chip_type)
        {
            break; 
        }
    }
    /* If we get invalid type name, return "Unknown".*/
    if( i == arry_size)
    {
        printk("WIFI, Get chip type fail.\n");
        return wifi_device_array[arry_size - 1].dev_name;
    }

    return wifi_device_array[i].dev_name;
}

/**
 * Read the data via the proc interface.
 * @param page Buffer for writing data.
 * @param start Not used.
 * @param offset Not used.
 * @param count Not used.
 * @param eof Whether or not there is more data to be read.
 * @param data Not used.
 * @return The number of bytes written.
 */
static int wlanfeature_read_proc_device_type(char *page, char **start, off_t offset,
                    int count, int *eof, void *data)
{
    char *wifi_device_type = NULL;
    
    *eof = 1;
	
    wifi_device_type = get_wifi_device_type();

    /* Only the chip type info such as 1.2 or 2.2 is recorded. */
    return sprintf(page, "%s", wifi_device_type);
}

/**
 * Write the data via the proc interface.
 * @param file Not used.
 * @param buffer The buffer to read from.
 * @param count The number of bytes to be written.
 * @param data Not used.
 * @return On success, the number of bytes written. On error, -1
 *
 */
static int wlanfeature_write_proc_device_type(struct file *file, const char *buffer,
                    unsigned long count, void *data)
{
    /* the function is not used now.  */
    return 0;
}

/**
 * Read the data via the proc interface.
 * @param page Buffer for writing data.
 * @param start Not used.
 * @param offset Not used.
 * @param count Not used.
 * @param eof Whether or not there is more data to be read.
 * @param data Not used.
 * @return The number of bytes written.
 */
static int wlanfeature_read_proc_pubfile_id(char *page, char **start, off_t offset,
                    int count, int *eof, void *data)
{
    char wifi_pubfile_id[WLAN_MAX_PUBFILE_ID_LEN] = {0};
    
    *eof = 1;
	
    get_hw_wifi_pubfile_id(wifi_pubfile_id, WLAN_MAX_PUBFILE_ID_LEN);

    /* Only the chip type info such as 1.2 or 2.2 is recorded. */
    return sprintf(page, "%s", wifi_pubfile_id);
}

/**
 * Write the data via the proc interface.
 * @param file Not used.
 * @param buffer The buffer to read from.
 * @param count The number of bytes to be written.
 * @param data Not used.
 * @return On success, the number of bytes written. On error, -1
 *
 */
static int wlanfeature_write_proc_pubfile_id(struct file *file, const char *buffer,
                    unsigned long count, void *data)
{
    /* the function is not used now.  */
    return 0;
}

/**
 * Read the data via the proc interface.
 * @param page Buffer for writing data.
 * @param start Not used.
 * @param offset Not used.
 * @param count Not used.
 * @param eof Whether or not there is more data to be read.
 * @param data Not used.
 * @return The number of bytes written.
 */
static int wlanfeature_read_proc_wlan_fw_ver(char *page, char **start, off_t offset,
					int count, int *eof, void *data)
{
    int ret = -1;
    char wlan_fw_ver[MAX_WLAN_FW_VER_LEN] = {'\0'};
    
    *eof = 1;
	
    ret = get_wlan_fw_ver(wlan_fw_ver,MAX_WLAN_FW_VER_LEN);
    if(-1 == ret)
    {
        printk("Get wifi fw_ver  failed.\n");
        strncpy(wlan_fw_ver,"Unknown",sizeof("Unknown"));
    }
    
    return sprintf(page, "%s", wlan_fw_ver);
}

/**
 * Initializes the module.
 * @return On success, 0. On error, -1
 *
 */
static int __init wlanfeaturetrans_init(void)
{
    int retval = 0;
    struct proc_dir_entry *ent = NULL;

    printk("WIFI DEVICE FEATURE VERSION: %s", WLAN_DEVICE_VERSION);

    /* Driver Register */
    retval = platform_driver_register(&wlan_featuretrans_driver);
    if (0 != retval)
    {
        printk("[%s],featurntransfer driver register fail.", __func__);
        return retval;
    }

    /* create device_feature directory for wifi chip info */
    wlan_dir = proc_mkdir("wlan_feature", NULL);
    if (NULL == wlan_dir)
    {
        printk("Unable to create /proc/wlan_feature directory");
        retval =  -ENOMEM;
        goto fail;
    }

    /* Creating read/write "devtype" entry*/
    ent = create_proc_entry("devtype", 0, wlan_dir);
    if (NULL == ent) 
    {
        printk("Unable to create /proc/%s/devtype entry", PROC_DIR);
        retval = -ENOMEM;
        goto fail;
    }

    ent->read_proc = wlanfeature_read_proc_device_type;
    ent->write_proc = wlanfeature_write_proc_device_type;

    /* Creating read/write "pubfd" entry*/
    ent = create_proc_entry("pubfd", 0, wlan_dir);
    if (NULL == ent) 
    {
        printk("Unable to create /proc/%s/pubfd entry", PROC_DIR);
        retval = -ENOMEM;
        goto fail;
    }

    ent->read_proc = wlanfeature_read_proc_pubfile_id;
    ent->write_proc = wlanfeature_write_proc_pubfile_id;
    
    /* Creating read/write "wlan_fw_ver" entry for wifi chip type*/
    ent = create_proc_entry("wlan_fw_ver", 0, wlan_dir);
    if (NULL == ent) 
    {
	printk("Unable to create /proc/%s/wlan_fw_ver entry", PROC_DIR);
	retval = -ENOMEM;
	goto fail;
    }
	ent->read_proc = wlanfeature_read_proc_wlan_fw_ver;
  
    return 0;

fail:
    platform_driver_unregister(&wlan_featuretrans_driver);
    remove_proc_entry("devtype", wlan_dir);
    remove_proc_entry("pubfd", wlan_dir);
    remove_proc_entry("wlan_fw_ver", wlan_dir);
    remove_proc_entry("wlan_feature", 0);

    return retval;
}


/**
 * Cleans up the module.
 */
static void __exit wlanfeaturetrans_exit(void)
{
    platform_driver_unregister(&wlan_featuretrans_driver);
    remove_proc_entry("devtype", wlan_dir);
    remove_proc_entry("pubfd", wlan_dir);
    remove_proc_entry("wlan_fw_ver", wlan_dir);
    remove_proc_entry("wlan_feature", 0);
}



module_init(wlanfeaturetrans_init);
module_exit(wlanfeaturetrans_exit);

MODULE_DESCRIPTION("WIFI DEVICE FEATURE VERSION: %s " WLAN_DEVICE_VERSION);
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif
