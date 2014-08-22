/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <hsad/config_interface.h>
#include <../drivers/huawei/hsad/config_mgr.h>

#define AUDIO_INFO_BUFF_LEN 30
#define BOARD_ID_LEN 20

/* Tag name for audio parameter version in audio info */
#define AUDIO_INFO_TAG_PARAM_VER "aud_param_ver"
/* Default value for audio parameter version in audio info */
#define AUDIO_INFO_DEFAULT_VALUE_PARAM_VER "undefined"

struct proc_dir_entry *audio_info;

static int audio_read_proc(char *page, char **start, off_t off,
				 int count, int *eof, void *data)
{
    int total_len = 0;
    int len = 0;
    char audio_info_buff[AUDIO_INFO_BUFF_LEN] = {0};
    
    get_audio_property(audio_info_buff);
    total_len = snprintf(page, PAGE_SIZE, 
        "audio_property:\n%s\n", audio_info_buff);
        
    if(total_len >= PAGE_SIZE)
    {
        printk(KERN_ERR "audio_read_proc error, insufficient memory\n");
        return PAGE_SIZE - 1;
    }

    memset(audio_info_buff, 0, sizeof(audio_info_buff));
    if(get_hw_config_string("audio/aud_param_ver", 
        audio_info_buff, AUDIO_INFO_BUFF_LEN, NULL) == false)
    {
        printk(KERN_ERR "audio_read_proc error, get aud_param_ver failed\n");
        snprintf(audio_info_buff, 
            sizeof(audio_info_buff), "%s", AUDIO_INFO_DEFAULT_VALUE_PARAM_VER);
    }
    
    len = snprintf(page + total_len, PAGE_SIZE - total_len, 
        "%s:\n%s\n", AUDIO_INFO_TAG_PARAM_VER, audio_info_buff);
    
    if(len >= PAGE_SIZE - total_len)
    {
        printk(KERN_ERR "audio_read_proc error, insufficient memory\n");
        return PAGE_SIZE - 1;
    }
    
    total_len += len;
    
    return total_len;
}

static int boardid_read_proc(char *page, char **start, off_t off,
				 int count, int *eof, void *data)
{
    int len1 = 0;
    char idarray[BOARD_ID_LEN];  
    memset(idarray, 0, BOARD_ID_LEN);  
    get_hw_config_string("audio/board_id", idarray, BOARD_ID_LEN, NULL);
    len1 = snprintf(page, PAGE_SIZE,idarray);
    return len1;
}

static int __init audio_app_info_init(void)
{
    audio_info = proc_mkdir("audio_info",NULL);
    create_proc_read_entry("audio_info", 0, audio_info, audio_read_proc, NULL);
    create_proc_read_entry("boardid_info", 0, audio_info, boardid_read_proc, NULL);
    return 0;
}
module_init(audio_app_info_init);
