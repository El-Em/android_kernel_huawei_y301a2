
#include <linux/leds.h>
#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mdp4.h"
#include "hw_lcd_common.h"
#include <hsad/config_interface.h>
#include <linux/msm_mdp.h>
#include <linux/hw_backlight.h>
/*use qcom cmdlist interface in process_mipi_table()*/
static struct msm_panel_info pinfo;
static lcd_panel_type lcd_panel_wvga = LCD_NONE;

static struct dsi_buf hx8369a_tx_buf;

/*use qcom cmdlist interface in process_mipi_table()*/
static struct sequence * hx8369a_lcd_init_table_debug = NULL;
/*backlight control mode,1 means by WLED*/
static int wled_trigger_initialized;

static int mipi_hx8369a_lcd_init(void);
/*use qcom cmdlist interface in process_mipi_table()*/
static void mipi_hx8369a_set_backlight(struct msm_fb_data_type *mfd);

/*lk setup power*/
#ifdef CONFIG_FB_DYNAMIC_GAMMA
static const struct sequence *hx8369a_gamma_25 = NULL;
static const struct sequence *hx8369a_gamma_22 = NULL;
static size_t gamma_25_len = 0;
static size_t gamma_22_len = 0;
#endif
DEFINE_LED_TRIGGER(bkl_led_trigger);

#define LCD_PANEL_NAME	"mipi_hx8369a_tianma_wvga"

/*use same phy array in whole project*/
/*change to 459M to prevent RF interrupt*/
static struct mipi_dsi_phy_ctrl dsi_cmd_mode_phy_db = {
	/* 480*800, RGB888, 2 Lane 60 fps command mode,459M */
	/* regulator */
	{0x09, 0x08, 0x05, 0x00, 0x20},
	/* timing */       /*increase HS_EXIT*/
	{0x77, 0x1b, 0x11, 0x00, 0x3F, 0x38, 0x16, 0x1f,
	0x20, 0x03, 0x04, 0xa0},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x0, 0x10, 0x30, 0xc0, 0x00, 0x20, 0x07, 0x62,
	0x41, 0x0f, 0x01,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};

const struct sequence hx8369a_wvga_standby_enter_table[]= 
{
	{0x00028,MIPI_DCS_COMMAND,0}, //28h
	{0x0010,MIPI_DCS_COMMAND,20},
	{0x00029,MIPI_TYPE_END,150},
};
const struct sequence hx8369a_wvga_standby_exit_table[] =
{
	
	/* SET DISPLAY ON */
	{0x11,MIPI_DCS_COMMAND,0},
	{0x29,MIPI_DCS_COMMAND,150},
	{0x2C,MIPI_DCS_COMMAND,20},// 2C memory start  
	{0x29,MIPI_TYPE_END,20}, //the end flag,it don't sent to driver IC
};
/*use qcom cmdlist interface in process_mipi_table()*/
struct sequence hx8369a_wvga_write_cabc_brightness_table[]= 
{
	{0x00051,MIPI_DCS_COMMAND,0},
	{0x00064,TYPE_PARAMETER,0},
	{0x00029,MIPI_TYPE_END,0},
};

#ifdef CONFIG_FB_DYNAMIC_GAMMA
int mipi_hx8369a_set_dynamic_gamma(enum danymic_gamma_mode  gamma_mode,struct msm_fb_data_type *mfd)
{
	int ret = 0;

	if((!hx8369a_gamma_25)||(!hx8369a_gamma_22))
	{
		LCD_DEBUG("%s:don't support dynamic_gamma\n",__func__);
		return -EPERM;
	}

	switch(gamma_mode)
	{
		case GAMMA25:
			process_mipi_table(mfd,&hx8369a_tx_buf,hx8369a_gamma_25,
						gamma_25_len, lcd_panel_wvga);
			break ;
		case GAMMA22:
			process_mipi_table(mfd,&hx8369a_tx_buf,hx8369a_gamma_22,
						gamma_22_len, lcd_panel_wvga);
			break;
		default:
			LCD_DEBUG("%s: invalid dynamic_gamma: %d\n", __func__,gamma_mode);
			ret = -EINVAL;
			break;
	}
	LCD_DEBUG("%s: change gamma mode to %d\n",__func__,gamma_mode);
	return ret;
}
#endif

static int mipi_hx8369a_lcd_on(struct platform_device *pdev)
{
	/*use qcom cmdlist interface in process_mipi_table()*/
	boolean para_debug_flag = FALSE;
	uint32 para_num = 0;
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL; 

	para_debug_flag = lcd_debug_malloc_get_para( "hx8369a_lcd_init_table_debug", 
			(void**)&hx8369a_lcd_init_table_debug,&para_num);

	if( (TRUE == para_debug_flag) && (NULL != hx8369a_lcd_init_table_debug))
	{
		process_mipi_table(mfd,&hx8369a_tx_buf,hx8369a_lcd_init_table_debug,
			 para_num, lcd_panel_wvga);
	}
	else
	{
		mipi_set_tx_power_mode(1);
		process_mipi_table(mfd,&hx8369a_tx_buf,hx8369a_wvga_standby_exit_table,
		 	ARRAY_SIZE(hx8369a_wvga_standby_exit_table), lcd_panel_wvga);
		mipi_set_tx_power_mode(0);
	}

	if((TRUE == para_debug_flag)&&(NULL != hx8369a_lcd_init_table_debug))
	{
		lcd_debug_free_para((void *)hx8369a_lcd_init_table_debug);
	}

	pr_info("leave mipi_hx8369a_lcd_on \n");

	return 0;
}

static int mipi_hx8369a_lcd_off(struct platform_device *pdev)
{
	/*use qcom cmdlist interface in process_mipi_table()*/
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	process_mipi_table(mfd,&hx8369a_tx_buf,hx8369a_wvga_standby_enter_table,

		 ARRAY_SIZE(hx8369a_wvga_standby_enter_table), lcd_panel_wvga);

	pr_info("leave mipi_hx8369a_lcd_off \n");
	return 0;
}

#ifdef CONFIG_FB_AUTO_CABC
/*use qcom cmdlist interface in process_mipi_table()*/
static struct sequence hx8369a_auto_cabc_set_table[] =
{
	{0x00055,MIPI_DCS_COMMAND,0}, 
	{0x00001,TYPE_PARAMETER,0},
	{0x00029,MIPI_TYPE_END,0}, 
};


/***************************************************************
Function: hx8369a_config_cabc
Description: Set CABC configuration
Parameters:
	struct msmfb_cabc_config cabc_cfg: CABC configuration struct
Return:
	0: success
***************************************************************/
static int hx8369a_config_auto_cabc(struct msmfb_cabc_config cabc_cfg,struct msm_fb_data_type *mfd)
{
	int ret = 0;

	switch(cabc_cfg.mode)
	{
		case CABC_MODE_UI:
			hx8369a_auto_cabc_set_table[1].reg = 0x0001;
			break;
		case CABC_MODE_MOVING:
		case CABC_MODE_STILL:
			hx8369a_auto_cabc_set_table[1].reg = 0x0003;
			break;
		default:
			LCD_DEBUG("%s: invalid cabc mode: %d\n", __func__, cabc_cfg.mode);
			ret = -EINVAL;
			break;
	}
	if(likely(0 == ret))
	{
		process_mipi_table(mfd,&hx8369a_tx_buf,hx8369a_auto_cabc_set_table,
			 ARRAY_SIZE(hx8369a_auto_cabc_set_table), lcd_panel_wvga);
	}

	LCD_DEBUG("%s: change cabc mode to %d\n",__func__,cabc_cfg.mode);
	return ret;
}
#endif // CONFIG_FB_AUTO_CABC

/*move one line before*/
/*use qcom cmdlist interface in process_mipi_table()*/
void mipi_hx8369a_set_backlight(struct msm_fb_data_type *mfd)
{
	static int bl_level_flag = 0;
	/*qcom cmdlist interface can prevent dead lock,so delete some invalid operation*/
	if (wled_trigger_initialized) 
	{
		led_trigger_event(bkl_led_trigger, mfd->bl_level);
		//printk("In %s,level = %d\n",__func__,mfd->bl_level);
		return;
	}
	else
	{
		if( 0 == mfd->bl_level )
		{
			led_trigger_event(bkl_led_trigger, LED_OFF);
			bl_level_flag = 1;
		}
		else 
		{
			if(1 == bl_level_flag){
				led_trigger_event(bkl_led_trigger, LED_FULL);
				bl_level_flag = 0;
			}
		}
		hx8369a_wvga_write_cabc_brightness_table[1].reg = mfd->bl_level; 
		process_mipi_table(mfd,&hx8369a_tx_buf,hx8369a_wvga_write_cabc_brightness_table,
			 ARRAY_SIZE(hx8369a_wvga_write_cabc_brightness_table), lcd_panel_wvga);
		return;
	}
}

static int __devinit mipi_hx8369a_lcd_probe(struct platform_device *pdev)
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
	.probe  = mipi_hx8369a_lcd_probe,
	.driver = {
		.name   = "mipi_hx8369a",
	},
};

struct msm_fb_panel_data hx8369a_panel_data = {
	.on		= mipi_hx8369a_lcd_on,
	.off		= mipi_hx8369a_lcd_off,
	.set_backlight = pwm_set_backlight,
	/*use qcom cmdlist interface in process_mipi_table()*/
	.set_brightness = mipi_hx8369a_set_backlight,
	#ifdef CONFIG_FB_AUTO_CABC
	.config_cabc = hx8369a_config_auto_cabc,
#endif

#ifdef CONFIG_FB_DYNAMIC_GAMMA
	.set_dynamic_gamma = mipi_hx8369a_set_dynamic_gamma,
#endif
};

static int ch_used[3];

int mipi_hx8369a_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;
	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;
	ret = mipi_hx8369a_lcd_init();
	if (ret) 
	{
		pr_err("mipi_hx8369a_lcd_init() failed with ret %u\n", ret);
		goto driver_failed;
	}
	
	pdev = platform_device_alloc("mipi_hx8369a", (panel << 8)|channel);
	if (!pdev)
	{
		ret = -ENOMEM;
		goto driver_failed;
	}

	hx8369a_panel_data.panel_info = *pinfo;
	ret = platform_device_add_data(pdev, &hx8369a_panel_data,sizeof(hx8369a_panel_data));
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
	kfree(hx8369a_tx_buf.data);
	memset(&hx8369a_tx_buf,0,sizeof(struct dsi_buf));
	return ret;
}

static int mipi_hx8369a_lcd_init(void)
{
	led_trigger_register_simple("bkl_trigger", &bkl_led_trigger);
	pr_info("%s: SUCCESS (WLED TRIGGER)\n", __func__);
#ifdef CONFIG_FB_PM8038_CABC_PIN
	wled_trigger_initialized = 0;
#else
	wled_trigger_initialized = 1;
#endif
	mipi_dsi_buf_alloc(&hx8369a_tx_buf, DSI_BUF_SIZE);
	return platform_driver_register(&this_driver);
}

static int __init mipi_cmd_hx8369a_wvga_init(void)
{
	int ret;
	if(0 == lcd_detect_panel(LCD_PANEL_NAME))
	{
	}
	else
	{
		return 0;
	}
	pr_info("enter mipi_cmd_hx8369a_wvga_init \n");
	/*lk setup power*/
	
	pinfo.xres = 480;
	pinfo.yres = 800;
	pinfo.type = MIPI_CMD_PANEL;
	pinfo.pdest = DISPLAY_1;
	pinfo.wait_cycle = 0;
	pinfo.bpp = 24;
	pinfo.lcdc.border_clr = 0;	/* blk */
	pinfo.lcdc.underflow_clr = 0xff;	/* blue */
	pinfo.lcdc.hsync_skew = 0;
	pinfo.bl_max = 255;
	pinfo.bl_min = 30;		
	pinfo.fb_num = 2;
	pinfo.lcd.v_back_porch = 8;
	pinfo.lcd.v_front_porch = 4;
	pinfo.lcd.v_pulse_width = 1;

	/*over*/
	pinfo.lcd.vsync_enable = TRUE;
	pinfo.lcd.hw_vsync_mode = TRUE;
	pinfo.lcd.refx100 = 6000;
	/*change to 459M to prevent RF interrupt*/
	pinfo.clk_rate = 459000000;//459M
	pinfo.mipi.t_clk_post = 0x19;
	pinfo.mipi.t_clk_pre = 0x2e;
	pinfo.mipi.stream = 0; /* dma_p */
	pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
	pinfo.mipi.frame_rate = 60;
	pinfo.mipi.dsi_phy_db = &dsi_cmd_mode_phy_db;
	pinfo.mipi.data_lane0=TRUE;
	pinfo.mipi.data_lane1=TRUE;
	pinfo.mipi.te_sel = 1; /* TE from vsycn gpio */
	pinfo.mipi.interleave_max = 1;
	pinfo.mipi.insert_dcs_cmd = TRUE;
	pinfo.mipi.wr_mem_continue = 0x3c;
	pinfo.mipi.wr_mem_start = 0x2c;
	pinfo.mipi.tx_eot_append = TRUE;
	pinfo.mipi.esc_byte_ratio=4;

	/*test*/
	pinfo.mipi.mode=DSI_CMD_MODE;
	pinfo.mipi.dst_format=DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.rgb_swap=DSI_RGB_SWAP_RGB;
	pinfo.mipi.dlane_swap = 0x01;

	ret = mipi_hx8369a_device_register(&pinfo, MIPI_DSI_PRIM,MIPI_DSI_PANEL_WVGA);
	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	return ret;
}

module_init(mipi_cmd_hx8369a_wvga_init);

