
#include <linux/leds.h>
#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mdp4.h"
#include "hw_lcd_common.h"
#include <hsad/config_interface.h>
#include <linux/msm_mdp.h>
#include <linux/hw_backlight.h>
static struct msm_panel_info pinfo;
static lcd_panel_type lcd_panel_qhd = LCD_NONE;

static struct dsi_buf nt35516_tx_buf;

static struct sequence * nt35516_lcd_init_table_debug = NULL;
/*backlight control mode,1 means by WLED*/
static int wled_trigger_initialized;

static int mipi_nt35516_lcd_init(void);
static void mipi_nt35516_set_backlight(struct msm_fb_data_type *mfd);
#ifdef CONFIG_FB_DYNAMIC_GAMMA
static const struct sequence *nt35516_gamma_25 = NULL;
static const struct sequence *nt35516_gamma_22 = NULL;
static size_t gamma_25_len = 0;
static size_t gamma_22_len = 0;
#endif
DEFINE_LED_TRIGGER(bkl_led_trigger);
#define LCD_PANEL_TIANMA	"mipi_nt35516_tianma_qhd"
/*Change mipi clk to prevent RF interference*/
static struct mipi_dsi_phy_ctrl dsi_cmd_mode_phy_db = {
/* regulator */
	/* DSI Bit Clock at 459 MHz, 2 lane, RGB888 */
	/* regulator */
	{0x09, 0x08, 0x05, 0x00, 0x20},
	/* timing */
	{0x77, 0x1B, 0x11, 0x00, 0x3F, 0x38, 0x16, 0x1F,
	0x20, 0x03, 0x04, 0xa0},
	/* phy ctrl */
	{0x5f, 0x00, 0x00, 0x10},
	/* strength */
	{0xff, 0x00, 0x06, 0x00},
	/* pll control */
	{0x0, 0x10, 0x30, 0xC0, 0x00, 0x50, 0x48, 0x63,
	0x41, 0x0f, 0x07,
	0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};

const struct sequence nt35516_qhd_standby_enter_table[]= 
{
	/*To solve probality flicker*/
	{0x00053,MIPI_GEN_COMMAND,0},
	{0x00020,TYPE_PARAMETER,0},  //close diming and BL circuit now

	//extra cmd2
	{0x000F0,MIPI_GEN_COMMAND,0}, //open cmd2
	{0x00055,TYPE_PARAMETER,0},
	{0x000AA,TYPE_PARAMETER,0},
	{0x00052,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},

	{0x000D4,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000FF,TYPE_PARAMETER,0},

	{0x000D8,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000FF,TYPE_PARAMETER,0},

	{0x000DF,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000FF,TYPE_PARAMETER,0},

	{0x000E3,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000FF,TYPE_PARAMETER,0},

	{0x000E7,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000FF,TYPE_PARAMETER,0},

	{0x000EB,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000FF,TYPE_PARAMETER,0},

	{0x00028,MIPI_DCS_COMMAND,0}, //28h

	{0x000C1,MIPI_GEN_COMMAND,20},
	{0x00001,TYPE_PARAMETER,0},

	{0x00010,MIPI_DCS_COMMAND,40},

	//extra cmd2
	{0x000C1,MIPI_GEN_COMMAND,120},
	{0x00000,TYPE_PARAMETER,0},

	{0x000D4,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},

	{0x000D8,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},

	{0x000DF,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},

	{0x000E3,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},

	{0x000E7,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},

	{0x000EB,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},

	{0x000F0,MIPI_GEN_COMMAND,0}, //close cmd2
	{0x00055,TYPE_PARAMETER,0},
	{0x000AA,TYPE_PARAMETER,0},
	{0x00052,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	{0x00029,MIPI_TYPE_END,20}, // add new command for 
};
const struct sequence nt35516_qhd_standby_exit_table[] =
{
	{0x00011,MIPI_DCS_COMMAND,0},
	{0x00029,MIPI_DCS_COMMAND,120},
	{0x00029,MIPI_TYPE_END,20},
};

/*To solve probality flicker*/
static const struct sequence nt35516_qhd_standby_exit_table_new[]= 
{
	//extra cmd2
	{0x000F0,MIPI_GEN_COMMAND,0}, //open cmd2
	{0x00055,TYPE_PARAMETER,0},
	{0x000AA,TYPE_PARAMETER,0},
	{0x00052,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	
	{0x000C1,MIPI_GEN_COMMAND,0},
	{0x00001,TYPE_PARAMETER,0},

	{0x000D4,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000FF,TYPE_PARAMETER,0},

	{0x000D8,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000FF,TYPE_PARAMETER,0},

	{0x000DF,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000FF,TYPE_PARAMETER,0},

	{0x000E3,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000FF,TYPE_PARAMETER,0},

	{0x000E7,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000FF,TYPE_PARAMETER,0},

	{0x000EB,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000FF,TYPE_PARAMETER,0},

	/*set the delay time 100ms*/
	{0x00011,MIPI_DCS_COMMAND,0}, //29h
	{0x00029,MIPI_DCS_COMMAND,150},
	/* Fixed LCD Flicker */
	//{0x0020,MIPI_DCS_COMMAND,20},

	//extra cmd2
	{0x000C1,MIPI_GEN_COMMAND,40},
	{0x00000,TYPE_PARAMETER,0},

	{0x000D4,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},

	{0x000D8,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},

	{0x000DF,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},

	{0x000E3,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},

	{0x000E7,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},

	{0x000EB,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},

	{0x000F0,MIPI_GEN_COMMAND,0}, //close cmd2
	{0x00055,TYPE_PARAMETER,0},
	{0x000AA,TYPE_PARAMETER,0},
	{0x00052,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	{0x00053,MIPI_GEN_COMMAND,0}, 
	{0x00024,TYPE_PARAMETER,0},  //close diming, open BL circuit

	{0x00029,MIPI_TYPE_END,20}, // add new command for 
};

struct sequence nt35516_qhd_write_cabc_brightness_table[]= 
{
	{0x00051,MIPI_DCS_COMMAND,0},
	{0x000ff,TYPE_PARAMETER,0},
	{0x00029,MIPI_TYPE_END,0},
};
#ifdef CONFIG_FB_DYNAMIC_GAMMA
static const struct sequence nt35516_tianma_gamma_25[]= 
{
	/* Modify LCD init code in order to diaplay normal for modified mask panel */
	/*Optimize gamma2.5 init code*/
	{0x000F0,MIPI_GEN_COMMAND,0},//page1
	{0x00055,TYPE_PARAMETER,0},
	{0x000AA,TYPE_PARAMETER,0},
	{0x00052,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000D1,MIPI_GEN_COMMAND,0},//gamma25
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000A5,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0001E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00027,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0004C,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00074,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000A1,TYPE_PARAMETER,0},//8
	{0x000D2,MIPI_GEN_COMMAND,0},//D2
	{0x00001,TYPE_PARAMETER,0},
	{0x000C3,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x00022,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x00063,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x00096,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00098,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000C6,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000FB,TYPE_PARAMETER,0},
	{0x000D3,MIPI_GEN_COMMAND,0},//D3
	{0x00003,TYPE_PARAMETER,0},
	{0x0001C,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00065,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0008A,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000A2,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C9,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000E3,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F1,TYPE_PARAMETER,0},
	{0x000D4,MIPI_GEN_COMMAND,0},//D4
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000D5,MIPI_GEN_COMMAND,0},//gamma D5
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000A5,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0001E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00027,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0004C,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00074,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000A1,TYPE_PARAMETER,0},
	{0x000D6,MIPI_GEN_COMMAND,0},//D6
	{0x00001,TYPE_PARAMETER,0},
	{0x000C3,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x00022,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x00063,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00096,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00098,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000C6,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000FB,TYPE_PARAMETER,0},
	{0x000D7,MIPI_GEN_COMMAND,0},//D7
	{0x00003,TYPE_PARAMETER,0},
	{0x0001C,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00065,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0008A,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000A2,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C9,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000E3,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F1,TYPE_PARAMETER,0},
	{0x000D8,MIPI_GEN_COMMAND,0},//D8
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000D9,MIPI_GEN_COMMAND,0},//gamma D9
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000A5,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0001E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00027,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0004C,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00074,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000A1,TYPE_PARAMETER,0},
	{0x000DD,MIPI_GEN_COMMAND,0},//DD
	{0x00001,TYPE_PARAMETER,0},
	{0x000C3,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x00022,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x00063,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00096,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00098,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000C6,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000FB,TYPE_PARAMETER,0},
	{0x000DE,MIPI_GEN_COMMAND,0},//DE
	{0x00003,TYPE_PARAMETER,0},
	{0x0001C,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00065,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0008A,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000A2,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C9,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000E3,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F1,TYPE_PARAMETER,0},
	{0x000DF,MIPI_GEN_COMMAND,0},//DF
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000E0,MIPI_GEN_COMMAND,0},//gamma E0
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000A5,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0001E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00027,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0004C,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00074,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000A1,TYPE_PARAMETER,0},
	{0x000E1,MIPI_GEN_COMMAND,0},//E1
	{0x00001,TYPE_PARAMETER,0},
	{0x000C3,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x00022,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x00063,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00096,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00098,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000C6,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000FB,TYPE_PARAMETER,0},
	{0x000E2,MIPI_GEN_COMMAND,0},//E2
	{0x00003,TYPE_PARAMETER,0},
	{0x0001C,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00065,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0008A,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000A2,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C9,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000E3,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F1,TYPE_PARAMETER,0},
	{0x000E3,MIPI_GEN_COMMAND,0},//E3
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000E4,MIPI_GEN_COMMAND,0},//gamma E4
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000A5,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0001E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00027,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0004C,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00074,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000A1,TYPE_PARAMETER,0},
	{0x000E5,MIPI_GEN_COMMAND,0},//E5
	{0x00001,TYPE_PARAMETER,0},
	{0x000C3,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x00022,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x00063,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00096,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00098,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000C6,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000FB,TYPE_PARAMETER,0},
	{0x000E6,MIPI_GEN_COMMAND,0},//E6
	{0x00003,TYPE_PARAMETER,0},
	{0x0001C,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00065,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0008A,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000A2,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C9,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000E3,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F1,TYPE_PARAMETER,0},
	{0x000E7,MIPI_GEN_COMMAND,0},//E7
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000E8,MIPI_GEN_COMMAND,0},//gamma E8
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000A5,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0001E,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00027,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x0004C,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00074,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000A1,TYPE_PARAMETER,0},
	{0x000E9,MIPI_GEN_COMMAND,0},//E9
	{0x00001,TYPE_PARAMETER,0},
	{0x000C3,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x00022,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x00063,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00096,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00098,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000C6,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000FB,TYPE_PARAMETER,0},
	{0x000EA,MIPI_GEN_COMMAND,0},//EA
	{0x00003,TYPE_PARAMETER,0},
	{0x0001C,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00065,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0008A,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000A2,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C9,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000E3,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F1,TYPE_PARAMETER,0},
	{0x000EB,MIPI_GEN_COMMAND,0},//EB
	{0x00003,TYPE_PARAMETER,0},
	{0x000F7,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0xFFFFF,MIPI_TYPE_END,0},
};

static const struct sequence nt35516_tianma_gamma_22[]= 
{
	/* Modify LCD init code in order to diaplay normal for modified mask panel */
	/*Optimize gamma2.2 init code*/
	{0x000F0,MIPI_GEN_COMMAND,0},//page1
	{0x00055,TYPE_PARAMETER,0},
	{0x000AA,TYPE_PARAMETER,0},
	{0x00052,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x000D1,MIPI_GEN_COMMAND,0},//gamma22
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0008F,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00013,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00026,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00067,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},//8
	{0x000D2,MIPI_GEN_COMMAND,0},//D2
	{0x00001,TYPE_PARAMETER,0},
	{0x000B6,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000E9,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x00012,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x00053,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00085,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00087,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000B6,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x000D3,MIPI_GEN_COMMAND,0},//D3
	{0x00003,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0002E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0006B,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00080,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0009E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000B0,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C7,TYPE_PARAMETER,0},
	{0x000D4,MIPI_GEN_COMMAND,0},//D4
	{0x00003,TYPE_PARAMETER,0},
	{0x000E4,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000D5,MIPI_GEN_COMMAND,0},//gamma D5
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0008F,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00013,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00026,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00067,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x000D6,MIPI_GEN_COMMAND,0},//D6
	{0x00001,TYPE_PARAMETER,0},
	{0x000B6,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000E9,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x00012,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x00053,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00085,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00087,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000B6,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x000D7,MIPI_GEN_COMMAND,0},//D7
	{0x00003,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0002E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0006B,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00080,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0009E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000B0,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C7,TYPE_PARAMETER,0},
	{0x000D8,MIPI_GEN_COMMAND,0},//D8
	{0x00003,TYPE_PARAMETER,0},
	{0x000E4,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000D9,MIPI_GEN_COMMAND,0},//gamma D9
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0008F,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00013,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00026,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00067,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x000DD,MIPI_GEN_COMMAND,0},//DD
	{0x00001,TYPE_PARAMETER,0},
	{0x000B6,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000E9,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x00012,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x00053,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00085,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00087,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000B6,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x000DE,MIPI_GEN_COMMAND,0},//DE
	{0x00003,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0002E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0006B,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00080,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0009E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000B0,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C7,TYPE_PARAMETER,0},
	{0x000DF,MIPI_GEN_COMMAND,0},//DF
	{0x00003,TYPE_PARAMETER,0},
	{0x000E4,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000E0,MIPI_GEN_COMMAND,0},//gamma E0
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0008F,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00013,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00026,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00067,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x000E1,MIPI_GEN_COMMAND,0},//E1
	{0x00001,TYPE_PARAMETER,0},
	{0x000B6,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000E9,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x00012,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x00053,TYPE_PARAMETER,0},//12 
	{0x00002,TYPE_PARAMETER,0},
	{0x00085,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00087,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000B6,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x000E2,MIPI_GEN_COMMAND,0},//E2
	{0x00003,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0002E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0006B,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00080,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0009E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000B0,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C7,TYPE_PARAMETER,0},
	{0x000E3,MIPI_GEN_COMMAND,0},//E3
	{0x00003,TYPE_PARAMETER,0},
	{0x000E4,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000E4,MIPI_GEN_COMMAND,0},//gamma E4
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0008F,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00013,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00026,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00067,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x000E5,MIPI_GEN_COMMAND,0},//E5
	{0x00001,TYPE_PARAMETER,0},
	{0x000B6,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000E9,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x00012,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x00053,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00085,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00087,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000B6,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x000E6,MIPI_GEN_COMMAND,0},//E6
	{0x00003,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0002E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0006B,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00080,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0009E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000B0,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C7,TYPE_PARAMETER,0},
	{0x000E7,MIPI_GEN_COMMAND,0},//E7
	{0x00003,TYPE_PARAMETER,0},
	{0x000E4,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0x000E8,MIPI_GEN_COMMAND,0},//gamma E8
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0008F,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00013,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00026,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00067,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00095,TYPE_PARAMETER,0},
	{0x000E9,MIPI_GEN_COMMAND,0},//E9
	{0x00001,TYPE_PARAMETER,0},
	{0x000B6,TYPE_PARAMETER,0},//BA
	{0x00001,TYPE_PARAMETER,0},
	{0x000E9,TYPE_PARAMETER,0},//E5
	{0x00002,TYPE_PARAMETER,0},
	{0x00012,TYPE_PARAMETER,0},//1D
	{0x00002,TYPE_PARAMETER,0},
	{0x00053,TYPE_PARAMETER,0},//12 5f
	{0x00002,TYPE_PARAMETER,0},
	{0x00085,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00087,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000B6,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x000E8,TYPE_PARAMETER,0},
	{0x000EA,MIPI_GEN_COMMAND,0},//EA
	{0x00003,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0002E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0006B,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00080,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x0009E,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000B0,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000C7,TYPE_PARAMETER,0},
	{0x000EB,MIPI_GEN_COMMAND,0},//EB
	{0x00003,TYPE_PARAMETER,0},
	{0x000E4,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000F8,TYPE_PARAMETER,0},
	{0xFFFFF,MIPI_TYPE_END,0},
};
int mipi_nt35516_set_dynamic_gamma(enum danymic_gamma_mode  gamma_mode,struct msm_fb_data_type *mfd)
{
	int ret = 0;

	if((!nt35516_gamma_25)||(!nt35516_gamma_22))
	{
		LCD_DEBUG("%s:don't support dynamic_gamma\n",__func__);
		return -EPERM;
	}

	switch(gamma_mode)
	{
		case GAMMA25:
			process_mipi_table(mfd,&nt35516_tx_buf,nt35516_gamma_25,
						gamma_25_len, lcd_panel_qhd);
			break;
		case GAMMA22:
			process_mipi_table(mfd,&nt35516_tx_buf,nt35516_gamma_22,
						gamma_22_len, lcd_panel_qhd);
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

/*This error is generate by novatek IC when DSI output LP11==> LP01==> LP00,
  There is a timeout,which makes an error flag in IC*/
static char nt35516_manufacture_id[2] = {0x0A, 0x00}; /* DTYPE_DCS_READ */

static struct dsi_cmd_desc nt35516_manufacture_id_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 5, sizeof(nt35516_manufacture_id), nt35516_manufacture_id};

static void mipi_nt35516_manufacture_cb(char *data)
{
	char *lp = NULL;
	int i = 0;
	lp = data;
	/*read 2 byte back and print*/
	for(i = 0;i < 2;i ++)
	{
		if(lp != NULL)
		{
			printk("%s: register_value=0x%02x\n",__func__, *lp);
			lp ++;
		}
	}
}

static void mipi_nt35516_manufacture_id(void)
{
	struct dcs_cmd_req cmdreq;

	cmdreq.cmds = &nt35516_manufacture_id_cmd;
	cmdreq.cmds_cnt = 1;
	/*Qual Baseline Update,avoid backlight problem*/
	cmdreq.flags = CMD_REQ_RX | CMD_REQ_COMMIT | CMD_CLK_CTRL;
	cmdreq.rlen = 2;/*read 2 byte*/
	cmdreq.cb = NULL; 
	cmdreq.cb_hw =  mipi_nt35516_manufacture_cb;
	mipi_dsi_cmdlist_put(&cmdreq);
	/*
	 * blocked here, untill call back called
	 */
}

/*To solve probality flicker*/
static int mipi_nt35516_lcd_on(struct platform_device *pdev)
{
	boolean para_debug_flag = FALSE;
	uint32 para_num = 0;
	static boolean first_call = TRUE;
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL; 

	para_debug_flag = lcd_debug_malloc_get_para( "nt35516_lcd_init_table_debug", 
			(void**)&nt35516_lcd_init_table_debug,&para_num);

	if( (TRUE == para_debug_flag) && (NULL != nt35516_lcd_init_table_debug))
	{
		process_mipi_table(mfd,&nt35516_tx_buf,nt35516_lcd_init_table_debug,
			 para_num, lcd_panel_qhd);
	}
	else
	{
		mipi_set_tx_power_mode(1);

		/* when first call this function, use old sequence */
		if (FALSE == first_call)
		{
			process_mipi_table(mfd,&nt35516_tx_buf,(struct sequence*)&nt35516_qhd_standby_exit_table_new,
				ARRAY_SIZE(nt35516_qhd_standby_exit_table_new), lcd_panel_qhd);
		}
		else
		{
			process_mipi_table(mfd,&nt35516_tx_buf,(struct sequence*)&nt35516_qhd_standby_exit_table,
				ARRAY_SIZE(nt35516_qhd_standby_exit_table), lcd_panel_qhd);
			first_call = FALSE;
		}

		mipi_set_tx_power_mode(0);
	}

	if((TRUE == para_debug_flag)&&(NULL != nt35516_lcd_init_table_debug))
	{
		lcd_debug_free_para((void *)nt35516_lcd_init_table_debug);
	}

	/* clean up ack_err_status by read 0x0A*/
	mipi_nt35516_manufacture_id();
	
	pr_info("leave mipi_nt35516_lcd_on, first_call == %d \n", first_call);

	return 0;
}

static int mipi_nt35516_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	process_mipi_table(mfd,&nt35516_tx_buf,nt35516_qhd_standby_enter_table,
		 ARRAY_SIZE(nt35516_qhd_standby_enter_table), lcd_panel_qhd);
	pr_info("leave mipi_nt35516_lcd_off \n");
	return 0;
}

#ifdef CONFIG_FB_AUTO_CABC
static struct sequence nt35516_auto_cabc_set_table[] =
{
	{0x00055,MIPI_DCS_COMMAND,0}, 
	{0x00001,TYPE_PARAMETER,0},
	/*remove diming function of lcd,let it work in video */
	{0x00053,MIPI_DCS_COMMAND,0}, 
	{0x00024,TYPE_PARAMETER,0},
	{0xFFFFF,MIPI_TYPE_END,0}, //the end flag,it don't sent to driver IC
};
/***************************************************************
Function: nt35516_config_cabc
Description: Set CABC configuration
Parameters:
	struct msmfb_cabc_config cabc_cfg: CABC configuration struct
Return:
	0: success
***************************************************************/
/*remove diming function of lcd,let it work in video */
static int nt35516_config_auto_cabc(struct msmfb_cabc_config cabc_cfg,struct msm_fb_data_type *mfd)
{
	int ret = 0;

	switch(cabc_cfg.mode)
	{
		case CABC_MODE_UI:
			nt35516_auto_cabc_set_table[1].reg = 0x00001;
			nt35516_auto_cabc_set_table[3].reg = 0x00024;
			break;
		case CABC_MODE_MOVING:
		case CABC_MODE_STILL:
			nt35516_auto_cabc_set_table[1].reg = 0x00003;
			nt35516_auto_cabc_set_table[3].reg = 0x0002c;
			break;
		default:
			LCD_DEBUG("%s: invalid cabc mode: %d\n", __func__, cabc_cfg.mode);
			ret = -EINVAL;
			break;
	}
	if(likely(0 == ret))
	{
		process_mipi_table(mfd,&nt35516_tx_buf,nt35516_auto_cabc_set_table,
			 ARRAY_SIZE(nt35516_auto_cabc_set_table), lcd_panel_qhd);
	}

	LCD_DEBUG("%s: change cabc mode to %d\n",__func__,cabc_cfg.mode);
	return ret;
}
#endif // CONFIG_FB_AUTO_CABC

void mipi_nt35516_set_backlight(struct msm_fb_data_type *mfd)
{
	
	static int bl_level_flag = 0;
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
		nt35516_qhd_write_cabc_brightness_table[1].reg = mfd->bl_level; 
		process_mipi_table(mfd,&nt35516_tx_buf,nt35516_qhd_write_cabc_brightness_table,
			 ARRAY_SIZE(nt35516_qhd_write_cabc_brightness_table), lcd_panel_qhd);
		return;
	}
}

static int __devinit mipi_nt35516_lcd_probe(struct platform_device *pdev)
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
	.probe  = mipi_nt35516_lcd_probe,
	.driver = {
		.name   = "mipi_nt35516",
	},
};

struct msm_fb_panel_data nt35516_panel_data = {
	.on		= mipi_nt35516_lcd_on,
	.off		= mipi_nt35516_lcd_off,
	.set_backlight = pwm_set_backlight,
	.set_brightness = mipi_nt35516_set_backlight,
#ifdef CONFIG_FB_AUTO_CABC
	.config_cabc = nt35516_config_auto_cabc,
#endif

#ifdef CONFIG_FB_DYNAMIC_GAMMA
	.set_dynamic_gamma = mipi_nt35516_set_dynamic_gamma,
#endif
};

static int ch_used[3];

int mipi_nt35516_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;
	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;
	ret = mipi_nt35516_lcd_init();
	if (ret) 
	{
		pr_err("mipi_nt35516_lcd_init() failed with ret %u\n", ret);
		goto driver_failed;
	}
	
	pdev = platform_device_alloc("mipi_nt35516", (panel << 8)|channel);
	if (!pdev)
	{
		ret = -ENOMEM;
		goto driver_failed;
	}

	nt35516_panel_data.panel_info = *pinfo;
	ret = platform_device_add_data(pdev, &nt35516_panel_data,sizeof(nt35516_panel_data));
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
	kfree(nt35516_tx_buf.data);
	memset(&nt35516_tx_buf,0,sizeof(struct dsi_buf));
	return ret;
}

static int mipi_nt35516_lcd_init(void)
{
	led_trigger_register_simple("bkl_trigger", &bkl_led_trigger);
	pr_info("%s: SUCCESS (WLED TRIGGER)\n", __func__);
#ifdef CONFIG_FB_PM8038_CABC_PIN
	wled_trigger_initialized = 0;
#else
	wled_trigger_initialized = 1;
#endif
	mipi_dsi_buf_alloc(&nt35516_tx_buf, DSI_BUF_SIZE);
	return platform_driver_register(&this_driver);
}

static int __init mipi_cmd_nt35516_qhd_init(void)
{
	int ret;
	if(0 == lcd_detect_panel(LCD_PANEL_TIANMA))
	{
		#ifdef CONFIG_FB_DYNAMIC_GAMMA
		nt35516_gamma_25 = nt35516_tianma_gamma_25;
		gamma_25_len     = ARRAY_SIZE(nt35516_tianma_gamma_25);
		nt35516_gamma_22 = nt35516_tianma_gamma_22;
		gamma_22_len     = ARRAY_SIZE(nt35516_tianma_gamma_22);
		#endif
	}
	else
	{
		return 0;
	}
	pr_info("enter mipi_cmd_nt35516_qhd_init \n");

	pinfo.xres = 540;
	pinfo.yres = 960;
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
/*Change mipi clk to prevent RF interference*/
	pinfo.clk_rate = 459000000;
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

	pinfo.mipi.mode=DSI_CMD_MODE;
	pinfo.mipi.dst_format=DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.rgb_swap=DSI_RGB_SWAP_RGB;
	pinfo.mipi.dlane_swap = 0x01;

	ret = mipi_nt35516_device_register(&pinfo, MIPI_DSI_PRIM,MIPI_DSI_PANEL_QHD_PT);
	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	return ret;
}

module_init(mipi_cmd_nt35516_qhd_init);

