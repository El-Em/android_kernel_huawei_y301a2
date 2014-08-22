/* Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifdef CONFIG_HUAWEI_KERNEL


#include "board-8930.h"
#include <hsad/config_interface.h>

//function declear
extern void __init bt_bcm4330_power_init(void);


typedef struct
{
    bt_fm_device_type device_type;
    void (*pfun_init)(void);
}BT_POWER_INIT_FUN_S;

const BT_POWER_INIT_FUN_S g_strBtPowerInit[] = 
{
    {BT_FM_BROADCOM_BCM4330,bt_bcm4330_power_init},
    {BT_FM_QUALCOMM_WCN3660,NULL},
};


//external interface for all bluetooth chip power init
void __init bluetooth_power_init(void)
{
    int i = 0;
    int bt_fm_chip_type = (int)BT_FM_UNKNOWN_DEVICE;
	
    /* get bt/fm chip type from the device feature configuration (.xml file) */
    bt_fm_chip_type = get_bt_fm_device_type();
    if(-1 == bt_fm_chip_type)
    {
	printk(KERN_ERR "Get Bt chip type failed.\n");
	return;
    }

    printk(KERN_ERR "Current Bt chip is %d.\n",bt_fm_chip_type);
    
    for(i = 0; i < sizeof(g_strBtPowerInit)/sizeof(BT_POWER_INIT_FUN_S); i++)
    {
        if(bt_fm_chip_type != g_strBtPowerInit[i].device_type)
        {
            continue;
        }

        if(NULL != g_strBtPowerInit[i].pfun_init)
        {
            g_strBtPowerInit[i].pfun_init();
            return;
        }
        else
        {
            printk(KERN_ERR "No need do power init for the chip(%d)\n",bt_fm_chip_type);
			/* return if no need to do power init */
			return;
        }
    }

    if(i == sizeof(g_strBtPowerInit)/sizeof(BT_POWER_INIT_FUN_S))
    {
        printk(KERN_ERR "Bluetooth power init fun was not found\n");
    }
    
	return;
}


#endif //CONFIG_HUAWEI_KERNEL
