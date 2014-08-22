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

#define NFC_PROC_DIR	"nfc_feature"

struct proc_dir_entry    *nfc_dir;


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
static int nfc_read_proc_support_feature(char *page, char **start, off_t offset,
                    int count, int *eof, void *data)
{
    int nfc_support_feature = 0 ;
	
    nfc_support_feature = get_nfc_support();

    return sprintf(page, "%d", nfc_support_feature);
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
static int nfc_write_proc_support_feature(struct file *file, const char *buffer,
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
static int nfc_read_proc_device_type(char *page, char **start, off_t offset,
                    int count, int *eof, void *data)
{
    int nfc_device_type = 0;
    
    nfc_device_type = get_nfc_chipset_type();

    return sprintf(page, "%d", nfc_device_type);
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
static int nfc_write_proc_device_type(struct file *file, const char *buffer,
                    unsigned long count, void *data)
{
    /* the function is not used now.  */
    return 0;
}

/**
 * Initializes the module.
 * @return On success, 0. On error, -1
 *
 */
static int __init nfcfeaturetrans_init(void)
{
    int retval = 0;
    struct proc_dir_entry *ent = NULL;

    printk("NFC DEVICE FEATURE DETECT\n");

    /* create device_feature directory for nfc chip info */
    nfc_dir = proc_mkdir(NFC_PROC_DIR, NULL);
    if (NULL == nfc_dir)
    {
        printk("Unable to create /proc/nfc_feature directory");
        retval =  -ENOMEM;
        goto fail;
    }

    /* Creating read/write "support" entry*/
    ent = create_proc_entry("support", 0, nfc_dir);
    if (NULL == ent) 
    {
        printk("Unable to create /proc/%s/support entry\n", NFC_PROC_DIR);
        retval = -ENOMEM;
        goto fail;
    }

    ent->read_proc = nfc_read_proc_support_feature;
    ent->write_proc = nfc_write_proc_support_feature;

    /* Creating read/write "type" entry*/
    ent = create_proc_entry("type", 0, nfc_dir);
    if (NULL == ent) 
    {
        printk("Unable to create /proc/%s/type entry \n", NFC_PROC_DIR);
        retval = -ENOMEM;
        goto fail;
    }

    ent->read_proc = nfc_read_proc_device_type;
    ent->write_proc = nfc_write_proc_device_type;
  
    return 0;

fail:
    remove_proc_entry("support", nfc_dir);
    remove_proc_entry("type", nfc_dir);
    remove_proc_entry("nfc_feature", 0);

    return retval;
}


/**
 * Cleans up the module.
 */
static void __exit nfcfeaturetrans_exit(void)
{
    remove_proc_entry("support", nfc_dir);
    remove_proc_entry("type", nfc_dir);
    remove_proc_entry("nfc_feature", 0);
}



device_initcall_sync(nfcfeaturetrans_init);
module_exit(nfcfeaturetrans_exit);

MODULE_DESCRIPTION("NFC FEATURE");
#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif
