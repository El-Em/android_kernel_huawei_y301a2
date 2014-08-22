/* add r63308 720P LCD driver*/
/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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
#include <linux/leds.h>
#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mdp4.h"
#include "hw_lcd_common.h"
#include <hsad/config_interface.h>

#include <mach/gpio.h>
#include <linux/hw_backlight.h>
static struct msm_panel_info pinfo;
lcd_panel_type lcd_panel_720p = LCD_NONE;

static struct dsi_buf r63308_tx_buf;

static struct sequence * r63308_lcd_init_table_debug = NULL;
static int wled_trigger_initialized;

static int mipi_r63308_lcd_init(void);
static void mipi_r63308_set_backlight(struct msm_fb_data_type *mfd);

extern void setup_power(void);
#define LCD_PANEL_NAME	"mipi_r63308_toshiba_720p"

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* bit_clk 402.17Mhz, 720*1280, RGB888, 4 Lane 60 fps video mode */
    /* regulator */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing */
	{0x66, 0x26, 0x1a, 0x00, 0x32, 0x91, 0x1e, 0x2b,
	0x1c, 0x03, 0x04, 0xa0},
    /* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
    /* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x09, 0x76, 0x30, 0xc7, 0x00, 0x20, 0x0c, 0x62,
	0x41, 0x0f, 0x03,
	0x00, 0x1a, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02 },
};


const struct sequence r63308_720p_standby_enter_table[]= 
{
	/*close Vsync singal,when lcd sleep in*/
	{0x00028,MIPI_DCS_COMMAND,0}, //28h
	{0x00000,TYPE_PARAMETER,0},
	/*delay time is not very correctly right*/
	{0x00010,MIPI_DCS_COMMAND,20},
	{0x00000,TYPE_PARAMETER,0},
	{0x00029,MIPI_TYPE_END,200}, // add new command for 
};
const struct sequence r63308_720p_standby_exit_table[] =
{
  
    {0x000B0,MIPI_GEN_COMMAND,0}, //B0h
    {0x00000,TYPE_PARAMETER,0},
  
    {0x000B2,MIPI_GEN_COMMAND,10}, //B0h
    {0x00001,TYPE_PARAMETER,0},
  
    {0x000B3,MIPI_GEN_COMMAND,10}, //B3h
    {0x0000C,TYPE_PARAMETER,0},
    
    {0x000B4,MIPI_GEN_COMMAND,10 }, //B6h 
    {0x00002,TYPE_PARAMETER,0},
    
    {0x000BB,MIPI_GEN_COMMAND,10}, 
    {0x00008,TYPE_PARAMETER,0},
     
    {0x000C0,MIPI_GEN_COMMAND,10}, //C1h
    {0x00040,TYPE_PARAMETER,0}, 
    {0x00002,TYPE_PARAMETER,0}, 
    {0x0007F,TYPE_PARAMETER,0}, 
    {0x000C8,TYPE_PARAMETER,0}, 
    {0x00008,TYPE_PARAMETER,0},
    
    /* resolve the tear screen and display inversion problem*/
    {0x000C1,MIPI_GEN_COMMAND,10}, //C1h
    {0x00000,TYPE_PARAMETER,0}, 
    {0x000A8,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0},
    {0x00000,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0}, 
    {0x0009C,TYPE_PARAMETER,0},
    {0x00008,TYPE_PARAMETER,0}, 
    {0x00024,TYPE_PARAMETER,0},
    {0x0000B,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0},
    
    {0x000C2,MIPI_GEN_COMMAND,10}, //C2h
    {0x00000,TYPE_PARAMETER,0},
    {0x00000,TYPE_PARAMETER,0},
    {0x0000B,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0},
    {0x00000,TYPE_PARAMETER,0}, 

    {0x000C3,MIPI_GEN_COMMAND, 10}, //C3h
    {0x00004,TYPE_PARAMETER,0},

	{0x000C4,MIPI_GEN_COMMAND, 10}, //C4h
    {0x0004D,TYPE_PARAMETER,0},
	{0x00083,TYPE_PARAMETER,0},
    {0x00000,TYPE_PARAMETER,0},

	
    {0x000C6,MIPI_GEN_COMMAND,10}, //C6h
    {0x00013,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0}, 
    {0x00008,TYPE_PARAMETER,0}, 
    {0x00071,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0},
    {0x00000,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0},
    {0x00000,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0},
    
    {0x000C7,MIPI_GEN_COMMAND, 10}, //C7h
    {0x00022,TYPE_PARAMETER,0},
    
    {0x000C8,MIPI_GEN_COMMAND,10}, //C8h
    {0x00000,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0}, 
    
    {0x000C9,MIPI_GEN_COMMAND,10}, //C9h
    {0x0000F,TYPE_PARAMETER,0}, 
    {0x00014,TYPE_PARAMETER,0},
    {0x00021,TYPE_PARAMETER,0},  
    {0x0002E,TYPE_PARAMETER,0}, 
    {0x00032,TYPE_PARAMETER,0}, 
    {0x0002E,TYPE_PARAMETER,0}, 
    {0x0003A,TYPE_PARAMETER,0}, 
    {0x00045,TYPE_PARAMETER,0}, 
    {0x0003F,TYPE_PARAMETER,0}, 
    {0x00042,TYPE_PARAMETER,0}, 
    {0x00052,TYPE_PARAMETER,0}, 
    {0x00039,TYPE_PARAMETER,0}, 
    {0x00033,TYPE_PARAMETER,0}, 
    
    {0x000CA,MIPI_GEN_COMMAND,10}, //CAh
    {0x00030,TYPE_PARAMETER,0}, 
    {0x0002B,TYPE_PARAMETER,0}, 
    {0x0003E,TYPE_PARAMETER,0}, 
    {0x00031,TYPE_PARAMETER,0}, 
    {0x0002D,TYPE_PARAMETER,0},
    {0x00031,TYPE_PARAMETER,0},
    {0x00025,TYPE_PARAMETER,0}, 
    {0x0001A,TYPE_PARAMETER,0}, 
    {0x00020,TYPE_PARAMETER,0}, 
    {0x0001D,TYPE_PARAMETER,0}, 
    {0x0000D,TYPE_PARAMETER,0}, 
    {0x00006,TYPE_PARAMETER,0}, 
    {0x0000C,TYPE_PARAMETER,0}, 
     
	
    {0x000CB,MIPI_GEN_COMMAND,10}, //Cbh
    {0x0000F,TYPE_PARAMETER,0}, 
    {0x00014,TYPE_PARAMETER,0}, 
    {0x00021,TYPE_PARAMETER,0}, 
    {0x0002E,TYPE_PARAMETER,0}, 
    {0x00032,TYPE_PARAMETER,0}, 
    {0x0002E,TYPE_PARAMETER,0}, 
    {0x0003A,TYPE_PARAMETER,0}, 
    {0x00045,TYPE_PARAMETER,0}, 
    {0x0003F,TYPE_PARAMETER,0}, 
    {0x00042,TYPE_PARAMETER,0}, 
    {0x00052,TYPE_PARAMETER,0}, 
    {0x00039,TYPE_PARAMETER,0}, 
    {0x00033,TYPE_PARAMETER,0}, 
    
    {0x000CC,MIPI_GEN_COMMAND,10}, //CCh
    {0x00030,TYPE_PARAMETER,0}, 
    {0x0002B,TYPE_PARAMETER,0}, 
    {0x0003E,TYPE_PARAMETER,0}, 
    {0x00031,TYPE_PARAMETER,0}, 
    {0x0002D,TYPE_PARAMETER,0}, 
    {0x00031,TYPE_PARAMETER,0}, 
    {0x00025,TYPE_PARAMETER,0}, 
    {0x0001A,TYPE_PARAMETER,0}, 
    {0x00020,TYPE_PARAMETER,0}, 
    {0x0001D,TYPE_PARAMETER,0}, 
    {0x0000D,TYPE_PARAMETER,0}, 
    {0x00006,TYPE_PARAMETER,0}, 
    {0x0000C,TYPE_PARAMETER,0}, 


	{0x000CD,MIPI_GEN_COMMAND,10}, //Cdh
    {0x0000F,TYPE_PARAMETER,0}, 
    {0x00014,TYPE_PARAMETER,0}, 
    {0x00021,TYPE_PARAMETER,0}, 
    {0x0002E,TYPE_PARAMETER,0}, 
    {0x00032,TYPE_PARAMETER,0}, 
    {0x0002E,TYPE_PARAMETER,0}, 
    {0x0003A,TYPE_PARAMETER,0}, 
    {0x00045,TYPE_PARAMETER,0}, 
    {0x0003F,TYPE_PARAMETER,0}, 
    {0x00042,TYPE_PARAMETER,0}, 
    {0x00052,TYPE_PARAMETER,0}, 
    {0x00039,TYPE_PARAMETER,0}, 
    {0x00033,TYPE_PARAMETER,0}, 
    
    {0x000CE,MIPI_GEN_COMMAND,10}, //CEh
    {0x00030,TYPE_PARAMETER,0}, 
    {0x0002B,TYPE_PARAMETER,0}, 
    {0x0003E,TYPE_PARAMETER,0}, 
    {0x00031,TYPE_PARAMETER,0}, 
    {0x0002D,TYPE_PARAMETER,0}, 
    {0x00031,TYPE_PARAMETER,0}, 
    {0x00025,TYPE_PARAMETER,0}, 
    {0x0001A,TYPE_PARAMETER,0}, 
    {0x00020,TYPE_PARAMETER,0}, 
    {0x0001D,TYPE_PARAMETER,0}, 
    {0x0000D,TYPE_PARAMETER,0}, 
    {0x00006,TYPE_PARAMETER,0}, 
    {0x0000C,TYPE_PARAMETER,0}, 


    {0x000D0,MIPI_GEN_COMMAND,10}, //D0h
    {0x00069,TYPE_PARAMETER,0}, 
    {0x00065,TYPE_PARAMETER,0}, 
    {0x00001,TYPE_PARAMETER,0},
    
    {0x000D1,MIPI_GEN_COMMAND,10}, //D1h
    {0x00077,TYPE_PARAMETER,0}, 
    {0x000D4,TYPE_PARAMETER,0}, 
    
    {0x000D3,MIPI_GEN_COMMAND,10}, //D3h
    {0x00033,TYPE_PARAMETER,0},
    
    {0x000D5,MIPI_GEN_COMMAND,10}, //D5h
    {0x0000F,TYPE_PARAMETER,0}, 
    {0x0000F,TYPE_PARAMETER,0},
    
    
    {0x000D8,MIPI_GEN_COMMAND,10}, //DEh
    {0x00034,TYPE_PARAMETER,0},
    {0x00064,TYPE_PARAMETER,0},
    {0x00023,TYPE_PARAMETER,0},
    {0x00025,TYPE_PARAMETER,0},
    {0x00062,TYPE_PARAMETER,0},
    {0x00032,TYPE_PARAMETER,0},
	
    {0x000DE,MIPI_GEN_COMMAND,10}, //DEh
    {0x00001,TYPE_PARAMETER,0},
    {0x00000,TYPE_PARAMETER,0},
    {0x00031,TYPE_PARAMETER,0},
    {0x00003,TYPE_PARAMETER,0},
    {0x00000,TYPE_PARAMETER,0},
    {0x00000,TYPE_PARAMETER,0},
    {0x00000,TYPE_PARAMETER,0},
    {0x00000,TYPE_PARAMETER,0},
    {0x00000,TYPE_PARAMETER,0}, 
    {0x00000,TYPE_PARAMETER,0},
    {0x00000,TYPE_PARAMETER,0}, 
      
    {0x000E2,MIPI_GEN_COMMAND,10}, //E2h
    {0x00000,TYPE_PARAMETER,0},

    {0x00011,MIPI_DCS_COMMAND,10}, //11h
    
    {0x00029,MIPI_DCS_COMMAND,200}, //29h      
    {0xFFFFF,MIPI_TYPE_END,20}, //the end flag,it don't sent to driver IC
    
};

struct sequence r63308_720p_write_cabc_brightness_table[]= 
{
	/* solve losing control of the backlight */
	{0x000B9,MIPI_GEN_COMMAND,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000A0,TYPE_PARAMETER,0},	
    {0x00002,TYPE_PARAMETER,0},
    {0x00018,TYPE_PARAMETER,0},
	{0x00000,MIPI_TYPE_END,0},
};

static int mipi_r63308_lcd_on(struct platform_device *pdev)
{
	boolean para_debug_flag = FALSE;
	uint32 para_num = 0;
	struct msm_fb_data_type *mfd;
	
	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL; 

	

	para_debug_flag = lcd_debug_malloc_get_para( "r63308_lcd_init_table_debug", 
            (void**)&r63308_lcd_init_table_debug,&para_num);

    if( (TRUE == para_debug_flag) && (NULL != r63308_lcd_init_table_debug))
    {
        process_mipi_table(mfd,&r63308_tx_buf,r63308_lcd_init_table_debug,
		     para_num, lcd_panel_720p);
    }
	else
	{
		mipi_set_tx_power_mode(1);
		process_mipi_table(mfd,&r63308_tx_buf,(struct sequence*)&r63308_720p_standby_exit_table,
			 ARRAY_SIZE(r63308_720p_standby_exit_table), lcd_panel_720p);
		mipi_set_tx_power_mode(0);

	}

	if((TRUE == para_debug_flag)&&(NULL != r63308_lcd_init_table_debug))
	{
		lcd_debug_free_para((void *)r63308_lcd_init_table_debug);
	}
	pr_info("leave mipi_r63308_lcd_on \n");

	return 0;
}

static int mipi_r63308_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	process_mipi_table(mfd,&r63308_tx_buf,(struct sequence*)&r63308_720p_standby_enter_table,
		 ARRAY_SIZE(r63308_720p_standby_enter_table), lcd_panel_720p);
	pr_info("leave mipi_r63308_lcd_off \n");
	return 0;
}

DEFINE_LED_TRIGGER(bkl_led_trigger);

void mipi_r63308_set_backlight(struct msm_fb_data_type *mfd)
{
	
	struct mipi_panel_info *mipi;
	if (wled_trigger_initialized) 
	{
		led_trigger_event(bkl_led_trigger, mfd->bl_level);
		//printk("In %s,level = %d\n",__func__,mfd->bl_level);
		return;
	}
	else
	{
		mipi  = &mfd->panel_info.mipi;
		mutex_lock(&mfd->dma->ov_mutex);
		/* slove compile issue for baseline 224008 */
		#if 0
		if (mdp4_overlay_dsi_state_get() <= ST_DSI_SUSPEND) 
		{
			mutex_unlock(&mfd->dma->ov_mutex);
			return;
		}
		/*these function are obsolete in JB baseline,need to check later*/
		/* mdp4_dsi_cmd_busy_wait: will turn on dsi clock also */
		mdp4_dsi_cmd_dma_busy_wait(mfd);
		mdp4_dsi_blt_dmap_busy_wait(mfd);
		mipi_dsi_mdp_busy_wait(mfd);
		#endif

		r63308_720p_write_cabc_brightness_table[2].reg = mfd->bl_level; 
		process_mipi_table(mfd,&r63308_tx_buf,(struct sequence*)&r63308_720p_write_cabc_brightness_table,
			 ARRAY_SIZE(r63308_720p_write_cabc_brightness_table), lcd_panel_720p);
		mutex_unlock(&mfd->dma->ov_mutex);
		return;
	}
}


static int __devinit mipi_r63308_lcd_probe(struct platform_device *pdev)
{
	struct platform_device *current_pdev = NULL;

	current_pdev = msm_fb_add_device(pdev);

	if (current_pdev == NULL) 
	{
		printk("error in %s\n",__func__);
	}
	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_r63308_lcd_probe,
	.driver = {
		.name   = "mipi_r63308",
	},
};

struct msm_fb_panel_data r63308_panel_data = {
	.on		= mipi_r63308_lcd_on,
	.off		= mipi_r63308_lcd_off,
	.set_backlight = pwm_set_backlight,
	.set_brightness = mipi_r63308_set_backlight,
};

static int ch_used[3];

int mipi_r63308_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;
	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;
	ret = mipi_r63308_lcd_init();
	if (ret) 
	{
		pr_err("mipi_r63308_lcd_init() failed with ret %u\n", ret);
		goto driver_failed;
	}
	
	pdev = platform_device_alloc("mipi_r63308", (panel << 8)|channel);
	if (!pdev)
	{
		ret = -ENOMEM;
		goto driver_failed;
	}

	r63308_panel_data.panel_info = *pinfo;
	ret = platform_device_add_data(pdev, &r63308_panel_data,sizeof(r63308_panel_data));
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}
	ret = platform_device_add(pdev);
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}
	return 0;

err_device_put:
	platform_device_put(pdev);

driver_failed:
	kfree(r63308_tx_buf.data);
    memset(&r63308_tx_buf,0,sizeof(struct dsi_buf));
	return ret;
}

static int mipi_r63308_lcd_init(void)
{
	led_trigger_register_simple("bkl_trigger", &bkl_led_trigger);
	pr_info("%s: SUCCESS (WLED TRIGGER)\n", __func__);
	wled_trigger_initialized = 1;

	mipi_dsi_buf_alloc(&r63308_tx_buf, DSI_BUF_SIZE);
	return platform_driver_register(&this_driver);
}

static int __init mipi_video_r63308_720p_init(void)
{
	int ret;
	if(0 != lcd_detect_panel(LCD_PANEL_NAME))
	{
        return 0;
	}
	pr_info("enter mipi_video_r63308_720p_init \n");
	setup_power();

	pinfo.xres = 720;
	pinfo.yres = 1280;
	pinfo.type = MIPI_VIDEO_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;
	pinfo.lcdc.h_back_porch = 64;
	pinfo.lcdc.h_front_porch = 96;
	pinfo.lcdc.h_pulse_width = 16;
	pinfo.lcdc.v_back_porch = 10;
	pinfo.lcdc.v_front_porch = 12;
	pinfo.lcdc.v_pulse_width = 2;

	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 30;		
	pinfo.fb_num = 2;
	/*over*/	
	pinfo.clk_rate = 402170000;
	pinfo.mipi.mode=DSI_VIDEO_MODE;
	pinfo.mipi.pulse_mode_hsa_he = TRUE;
	pinfo.mipi.hfp_power_stop = FALSE;
	pinfo.mipi.hbp_power_stop = FALSE;
	pinfo.mipi.hsa_power_stop = FALSE;
	pinfo.mipi.eof_bllp_power_stop = TRUE;
	pinfo.mipi.bllp_power_stop = TRUE;
	pinfo.mipi.traffic_mode = DSI_NON_BURST_SYNCH_EVENT;
	pinfo.mipi.dst_format=DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo.mipi.vc = 0;
	pinfo.mipi.rgb_swap=DSI_RGB_SWAP_RGB;	
	pinfo.mipi.data_lane0=TRUE;
	pinfo.mipi.data_lane1=TRUE; 
	pinfo.mipi.data_lane2=TRUE;
	pinfo.mipi.data_lane3=TRUE;
	pinfo.mipi.t_clk_post = 0x19;
	pinfo.mipi.t_clk_pre = 0x2e;
	pinfo.mipi.stream = 0; /* dma_p */
		
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;
		
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo.mipi.esc_byte_ratio=4;
					
	pinfo.mipi.dlane_swap = 0;

	ret = mipi_r63308_device_register(&pinfo, MIPI_DSI_PRIM,MIPI_DSI_PANEL_WVGA);
	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	return ret;
}

module_init(mipi_video_r63308_720p_init);


