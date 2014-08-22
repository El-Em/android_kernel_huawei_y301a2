/* kernel\drivers\power\hw_bms_battery_debug.h
 * this file is used by bms to get battery data from  /data/battery_debug.txt
 * Copyright (C) 2010 HUAWEI Technology Co., BMS.
 * 
 * Date: 2012/08/09
 * By sunchenggang
 * 
 */
#ifndef _HW_BMS_BATTERY_DEBUG
#define _HW_BMS_BATTERY_DEBUG

#include <linux/syscalls.h>

#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/err.h>

/* modify for 1.7232 baseline upgrade */
#include <linux/mfd/pm8xxx/batterydata-lib.h>
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

 
#define HW_BATTERY_DEBUG_DATA  "/data/battery_debug.txt"
#define HW_READ_LINE_MAX_LENGTH 200

bool bms_battery_debug_get_para(struct bms_battery_data* debug_battery_table);
#endif 

