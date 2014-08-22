#include <linux/leds.h>
#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mdp4.h"
#include "hw_lcd_common.h"
#include <hsad/config_interface.h>
#include <linux/hw_backlight.h>
#include <linux/msm_mdp.h>
static struct msm_panel_info pinfo;
static lcd_panel_type lcd_panel_qhd = LCD_NONE;

static struct dsi_buf otm9608a_tx_buf;

static struct sequence * otm9608a_lcd_init_table_debug = NULL;
/*backlight control mode,1 means by WLED*/
static int wled_trigger_initialized;

static int mipi_otm9608a_lcd_init(void);
static void mipi_otm9608a_set_backlight(struct msm_fb_data_type *mfd);

#define LCD_PANEL_NAME	"mipi_otm9608a_chimei_qhd"
#ifdef CONFIG_HW_ESD_DETECT
#define CMI_OTM9608A_PANEL_ALIVE    (1)
#define CMI_OTM9608A_PANEL_DEAD     (0)
#endif
DEFINE_LED_TRIGGER(bkl_led_trigger);
/*Change mipi clk to prevent RF interference*/
static struct mipi_dsi_phy_ctrl dsi_cmd_mode_phy_db = {
	/* 540*960 DSI Bit Clock at 459 MHz, 2 lane, RGB888 */
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

const struct sequence otm9608a_qhd_standby_enter_table[]= 
{
	/*close Vsync singal,when lcd sleep in*/

	{0x00028,MIPI_DCS_COMMAND,0}, //28h
	{0x0010,MIPI_DCS_COMMAND,20},
	{0x00029,MIPI_TYPE_END,150}, // add new command for 
};
const struct sequence otm9608a_qhd_standby_exit_table[] =
{
	{0x00011,MIPI_DCS_COMMAND,0},	//sleep out 
	{0x00029,MIPI_DCS_COMMAND,150}, //Display ON
	{0x00000,MIPI_TYPE_END,20}, //end flag
};

#ifdef CONFIG_HW_ESD_DETECT
/* CHIMEI LCD init code */
static struct sequence otm9608a_chimei_lcd_init_table[] =
{

	//ff00h Command 2 enable: Read and write,only access when Orise mode enable 
	//{0x00000,MIPI_DCS_COMMAND,200},
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000FF,MIPI_GEN_COMMAND,0},
	{0x00096,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0}, 

	//ff80h Orise CMD enable
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00080,TYPE_PARAMETER,0},
	{0x000FF,MIPI_GEN_COMMAND,0},
	{0x00096,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	// only into Orise Mode, the OTM9608 registers can be setted

	// B0b2h  CMI repley:adjust CLK glitch width  add:2013.02.18 
	/*move from back to solve read issue and LR flip issue,From Orise FAE wh*/
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000b1,TYPE_PARAMETER,0},
	{0x000b0,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00006,TYPE_PARAMETER,0},

	//a000h OTP select region()
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000a0,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},

	//b380h Command Set Option Parameter
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00080,TYPE_PARAMETER,0}, 
	{0x000b3,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00021,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//b392h IOPAD Parameter:These registers are used to replace the hardware PIN
	/*to solve first batch RL flip issue, From Orise FAE wh*/
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00090,TYPE_PARAMETER,0},
	{0x000b3,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},


	//c080h TCON Setting Parameter
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00080,TYPE_PARAMETER,0}, 
	{0x000C0,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00048,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00047,TYPE_PARAMETER,0},
	{0x0001f,TYPE_PARAMETER,0},
	{0x0001f,TYPE_PARAMETER,0},

	//address C582h
	/*
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00082,TYPE_PARAMETER,0},
	{0x000C5,MIPI_GEN_COMMAND,0},
	{0x000A3,TYPE_PARAMETER,0},
	*/

	//c092h PTSP1:Panel Timing Setting Parameter1
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00092,TYPE_PARAMETER,0}, 
	{0x000C0,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0000e,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00011,TYPE_PARAMETER,0},

	//c0a2h PTSP3:Panel Timing Setting Parameter3
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000a2,TYPE_PARAMETER,0},
	{0x000c0,MIPI_GEN_COMMAND,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	/*set column inversion for saving power*/
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000B4,TYPE_PARAMETER,0},
	{0x000C0,MIPI_GEN_COMMAND,0},
	{0x00050,TYPE_PARAMETER,0},
	
	//c181h Oscillator Adjustment for Idle/Normal Mode
	//This command is used to set the Oscillator frequency in normal mode and idle mode 
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00081,TYPE_PARAMETER,0}, 
	{0x000C1,MIPI_GEN_COMMAND,0},
	//{0x00077,TYPE_PARAMETER,0}, //70hz
	{0x00055,TYPE_PARAMETER,0}, //60hz
	
	//c480h Source driver precharge control 
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00080,TYPE_PARAMETER,0},
	{0x000c4,MIPI_GEN_COMMAND,0},
	/*source drive to GND during porch*/
	{0x00030,TYPE_PARAMETER,0},

	//c488h
	/*CMI add,source pull low when switch between sleep in/out*/
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00088,TYPE_PARAMETER,0},
	{0x000c4,MIPI_GEN_COMMAND,0},
	{0x00040,TYPE_PARAMETER,0},

	//address C0A3h
	/*
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000A3,TYPE_PARAMETER,0},
	{0x000C0,MIPI_GEN_COMMAND,0},
	{0x0001B,TYPE_PARAMETER,0}, 
	*/

	//c4a0h DC2DC Setting
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000a0,TYPE_PARAMETER,0}, 
	{0x000C4,MIPI_GEN_COMMAND,0},
	{0x00033,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0x00090,TYPE_PARAMETER,0},
	{0x0002b,TYPE_PARAMETER,0},
	{0x00033,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0x00090,TYPE_PARAMETER,0},
	{0x00054,TYPE_PARAMETER,0}, 


	//c580h Power control setting:is used to adjust analog power behavior
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00080,TYPE_PARAMETER,0}, 		
	{0x000C5,MIPI_GEN_COMMAND,0},
	{0x00008,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00090,TYPE_PARAMETER,0},
	{0x00011,TYPE_PARAMETER,0},  


	//c590h PWR_CTRL2:Power Control Setting 2 for Normal Mode	
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00090,TYPE_PARAMETER,0}, 		
	{0x000C5,MIPI_GEN_COMMAND,0},
	{0x00084,TYPE_PARAMETER,0},
	{0x00076,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00076,TYPE_PARAMETER,0},
	{0x00033,TYPE_PARAMETER,0},
	{0x00033,TYPE_PARAMETER,0},
	{0x00034,TYPE_PARAMETER,0},   


	//c5a0h PWR_CTRL3:Power Control Setting 3 for Idlel Mode	
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000a0,TYPE_PARAMETER,0}, 	
	{0x000C5,MIPI_GEN_COMMAND,0},
	{0x00096,TYPE_PARAMETER,0},
	{0x00076,TYPE_PARAMETER,0},
	{0x00006,TYPE_PARAMETER,0},
	{0x00076,TYPE_PARAMETER,0},
	{0x00033,TYPE_PARAMETER,0},
	{0x00033,TYPE_PARAMETER,0},
	{0x00034,TYPE_PARAMETER,0},  


	//c5b0h PWR_CTRL4:Power Power Control Setting 4 for DC Volatge Settings 
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000b0,TYPE_PARAMETER,0}, 	
	{0x000C5,MIPI_GEN_COMMAND,0},
	{0x00004,TYPE_PARAMETER,0},
	/*change vdd,lvdsvdd to increase self refresh rate*/
	//{0x000f8,TYPE_PARAMETER,0}, //vdd,lvdsvdd 1.55
	{0x00008,TYPE_PARAMETER,0}, //1.7

	//f5c1h Sample hold 
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000c1,TYPE_PARAMETER,0}, 	
	{0x000f5,MIPI_GEN_COMMAND,0},
	{0x00094,TYPE_PARAMETER,0},

	//c680h ABC Parameter1:is used to set the internal founction block of LABC
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00080,TYPE_PARAMETER,0}, 	
	{0x000C6,MIPI_GEN_COMMAND,0},
	{0x00064,TYPE_PARAMETER,0}, 

	/*change lcd output pwm frequence from 8k to 22k*/
	//c6b0h ABC Parameter2
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000b0,TYPE_PARAMETER,0},  
	{0x000C6,MIPI_GEN_COMMAND,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},//22.786kHz
	{0x00000,TYPE_PARAMETER,0},
	{0x0005f,TYPE_PARAMETER,0},
	{0x00012,TYPE_PARAMETER,0},

	//c0e1h HSYNC output control
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000e1,TYPE_PARAMETER,0}, 	
	{0x000C0,MIPI_GEN_COMMAND,0},
	{0x0009f,TYPE_PARAMETER,0},

	//d000h ID1:can programme 4 times
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000d0,MIPI_GEN_COMMAND,0},
	{0x00001,TYPE_PARAMETER,0},  

	//d100h ID2 & ID3:can programme 4 times 
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000d1,MIPI_GEN_COMMAND,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},

	//b0b7h 	
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000b7,TYPE_PARAMETER,0},
	{0x000b0,MIPI_GEN_COMMAND,0},
	{0x00010,TYPE_PARAMETER,0},

	//b0c0h 
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000c0,TYPE_PARAMETER,0},
	{0x000b0,MIPI_GEN_COMMAND,0},
	{0x00055,TYPE_PARAMETER,0},

	/*move to front to solve read issue,From Orise FAE wh*/

	//cb80h TCON_GOA_WAVE (panel timing state control)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00080,TYPE_PARAMETER,0},
	{0x000Cb,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//cb90h TCON_GOA_WAVE (panel timing state control)		
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00090,TYPE_PARAMETER,0},
	{0x000Cb,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//cba0h TCON_GOA_WAVE (panel timing state control)rol)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000a0,TYPE_PARAMETER,0},
	{0x000cb,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//cbb0h TCON_GOA_WAVE (panel timing state control)l)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000b0,TYPE_PARAMETER,0},
	{0x000Cb,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//cbc0h TCON_GOA_WAVE (panel timing state control)rol)rol)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000c0,TYPE_PARAMETER,0},
	{0x000Cb,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
   
	//cbd0h TCON_GOA_WAVE (panel timing state control)rol)rol)rol)		
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000d0,TYPE_PARAMETER,0},
	{0x000cb,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},//pd040ia-05d gama2.2	


	//cbe0h TCON_GOA_WAVE (panel timing state control)l)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000e0,TYPE_PARAMETER,0},
	{0x000cb,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0}, //pd040ia-05d gama2.2


	//cbf0h TCON_GOA_WAVE (panel timing state control)ol)l)
	{0x00000,MIPI_DCS_COMMAND,0}, 	
	{0x000f0,TYPE_PARAMETER,0},
	{0x000cb,MIPI_GEN_COMMAND,0},
	{0x000ff,TYPE_PARAMETER,0},
	{0x000ff,TYPE_PARAMETER,0},
	{0x000ff,TYPE_PARAMETER,0},
	{0x000ff,TYPE_PARAMETER,0},
	{0x000ff,TYPE_PARAMETER,0},
	{0x000ff,TYPE_PARAMETER,0},
	{0x000ff,TYPE_PARAMETER,0},
	{0x000ff,TYPE_PARAMETER,0},
	{0x000ff,TYPE_PARAMETER,0},
	{0x000ff,TYPE_PARAMETER,0}, //800-->320  //854 -->0x356 //864 -->0x360


	//cc80h TCON_GOA_WAVE (panel pad mapping control)ol)l)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00080,TYPE_PARAMETER,0}, 
	{0x000cc,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0000a,TYPE_PARAMETER,0},
	{0x0000e,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

		
	//cc90h TCON_GOA_WAVE (panel timing state control)rol)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00090,TYPE_PARAMETER,0}, 
	{0x000cc,MIPI_GEN_COMMAND,0},
	{0x0000c,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x00006,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},

	//cca0h TCON_GOA_WAVE (panel timing state control)rol)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000a0,TYPE_PARAMETER,0}, 
	{0x000cc,MIPI_GEN_COMMAND,0},
	{0x0000d,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0000b,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0}, 	

	//ccb0h TCON_GOA_WAVE (panel pad mapping control)ol)l)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000b0,TYPE_PARAMETER,0},   
	{0x000cc,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0000a,TYPE_PARAMETER,0},
	{0x0000e,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//ccc0h TCON_GOA_WAVE (panel timing state control)rol)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000c0,TYPE_PARAMETER,0}, 
	{0x000cc,MIPI_GEN_COMMAND,0},
	{0x0000c,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x00006,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},

	//ccd0h TCON_GOA_WAVE (panel timing state control)rol)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000d0,TYPE_PARAMETER,0}, 
	{0x000cc,MIPI_GEN_COMMAND,0},
	{0x0000d,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0000b,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//ce80h GOA VST Settimg
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00080,TYPE_PARAMETER,0}, 
	{0x000ce,MIPI_GEN_COMMAND,0},
	{0x00084,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00083,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//ce90h GOA VEND Setting  GOA Group Setting
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00090,TYPE_PARAMETER,0}, 
	{0x000ce,MIPI_GEN_COMMAND,0},
	{0x00033,TYPE_PARAMETER,0},
	{0x000bf,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00033,TYPE_PARAMETER,0},
	{0x000c0,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x000f0,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000f0,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//cea0h GOA CLKA1 Setting  GOA CLKA2 Setting
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000a0,TYPE_PARAMETER,0}, 
	{0x000ce,MIPI_GEN_COMMAND,0},
	{0x00038,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000c1,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00038,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000c2,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//ceb0h GOA CLKA3 Setting  GOA CLKA4 Setting
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000b0,TYPE_PARAMETER,0}, 
	{0x000ce,MIPI_GEN_COMMAND,0},
	{0x00038,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000c3,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00030,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000c4,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//cec0h GOA CLKB1 Setting  GOA CLKB2 Setting
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000c0,TYPE_PARAMETER,0}, 
	{0x000ce,MIPI_GEN_COMMAND,0},
	{0x00030,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000c5,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00030,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000c6,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//ced0h GOA CLKB3 Setting  GOA CLKB4 Setting
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000d0,TYPE_PARAMETER,0}, 
	{0x000ce,MIPI_GEN_COMMAND,0},
	{0x00030,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000c7,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00030,TYPE_PARAMETER,0},
	{0x00004,TYPE_PARAMETER,0},
	{0x00003,TYPE_PARAMETER,0},
	{0x000c8,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00018,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//cf80h GOA CLKC1 Setting  GOA CLKC2 Setting
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00080,TYPE_PARAMETER,0}, 
	{0x000cf,MIPI_GEN_COMMAND,0},
	{0x000f0,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000f0,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//cf90h GOA CLKC3 Setting  GOA CLKC4 Setting
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00090,TYPE_PARAMETER,0}, 
	{0x000cf,MIPI_GEN_COMMAND,0},
	{0x000f0,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000f0,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//cfa0h GOA CLKD1 Setting  GOA CLKD2 Setting
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000a0,TYPE_PARAMETER,0}, 
	{0x000cf,MIPI_GEN_COMMAND,0},
	{0x000f0,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000f0,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//cfb0h GOA CLKD3 Setting  GOA CLKD4 Setting
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000b0,TYPE_PARAMETER,0}, 
	{0x000cf,MIPI_GEN_COMMAND,0},
	{0x000f0,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000f0,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//cfc0h GOA ECLK Setting  GOA Signal Toggle Option Setting
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x000c0,TYPE_PARAMETER,0}, 
	{0x000cf,MIPI_GEN_COMMAND,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00020,TYPE_PARAMETER,0},
	{0x00020,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00002,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x00000,TYPE_PARAMETER,0},

	//d680h CE Enable
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00080,TYPE_PARAMETER,0}, 
	{0x000D6,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},

	//d700h AIE Enable
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0}, 
	{0x000D7,MIPI_GEN_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},

	//d800 GVDDSET:GVDD/NGVDD
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000d8,MIPI_GEN_COMMAND,0},
	{0x0006f,TYPE_PARAMETER,0},
	{0x0006f,TYPE_PARAMETER,0}, 

	/*remove vcom setting*/
	/*use gamma 2.5 as default effect parameter*/
	//e100h Gamma Correction Characteristics Setting(2.5+)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0}, 
	{0x000e1,MIPI_GEN_COMMAND,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00007,TYPE_PARAMETER,0},
	{0x0000b,TYPE_PARAMETER,0},
	{0x0000c,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x0000d,TYPE_PARAMETER,0},
	{0x0000a,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	{0x00006,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x0000b,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},


	//e200h Gamma Correction Characteristics Setting(2.5-)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0}, 
	{0x000e2,MIPI_GEN_COMMAND,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00007,TYPE_PARAMETER,0},
	{0x0000b,TYPE_PARAMETER,0},
	{0x0000c,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x0000d,TYPE_PARAMETER,0},
	{0x0000a,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	{0x00006,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x0000b,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},

	{0x00036,MIPI_GEN_COMMAND,0},// mx=1
	{0x00040,TYPE_PARAMETER,0}, 

	//ff00h Command 2 Disable
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0}, 
	{0x000ff,MIPI_GEN_COMMAND,0},
	{0x000ff,TYPE_PARAMETER,0},
	{0x000ff,TYPE_PARAMETER,0},
	{0x000ff,TYPE_PARAMETER,0},

	//enable lcd output pwm
	{0x00051,MIPI_DCS_COMMAND,0}, // displaybright
	{0x000FF,TYPE_PARAMETER,0}, 
	{0x00053,MIPI_DCS_COMMAND,0}, // ctrldisplay1
	{0x00024,TYPE_PARAMETER,0}, 
	{0x00055,MIPI_DCS_COMMAND,0}, // ctrldisplay2
	{0x00001,TYPE_PARAMETER,0},
	/*Write CABC minimum brightness,prevent flicker*/
	{0x0005E,MIPI_DCS_COMMAND,0},
	{0x000D0,TYPE_PARAMETER,0},
	
	{0x00011,MIPI_DCS_COMMAND,0},	//sleep out 
	{0x00029,MIPI_DCS_COMMAND,150}, //Display ON
	{0x00000,MIPI_TYPE_END,20}, //end flag
};

static char otm9608a_read_display_power_mode[2] = {0x0A, 0x00};
static char otm9608a_read_display_MADCTR[2] = {0x0B, 0x00};
static char otm9608a_read_display_pixel_format[2] = {0x0C, 0x00};
static char otm9608a_read_display_image_mode[2] = {0x0D, 0x00};

static struct dsi_cmd_desc otm9608a_read_display_power_mode_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 3, sizeof(otm9608a_read_display_power_mode), otm9608a_read_display_power_mode};
static struct dsi_cmd_desc otm9608a_read_display_MADCTR_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 3, sizeof(otm9608a_read_display_MADCTR), otm9608a_read_display_MADCTR};
static struct dsi_cmd_desc otm9608a_read_display_pixel_format_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 3, sizeof(otm9608a_read_display_pixel_format), otm9608a_read_display_pixel_format};
static struct dsi_cmd_desc otm9608a_read_display_image_mode_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 3, sizeof(otm9608a_read_display_image_mode), otm9608a_read_display_image_mode};

static struct dcs_cmd_req cmdreq;
static int panel_read_flag = -1;
#define PANEL_READ_ERROR 0
#define PANEL_READ_OK 1
static void panel_otm9608a_check_live_status_cb(char* data)
{
	if( (cmdreq.cmds==NULL)||(cmdreq.cmds->payload==NULL) ||(data==NULL) )
	{
		LCD_DEBUG("%s: Fatal error,NULL pointer\n", __func__);
		return;
	}
	switch(cmdreq.cmds->payload[0])
	{
		case 0x0A:
			{
				if(unlikely(data[0] != 0x9C))
				{
					printk("panel ic error: in %s,  reg 0x%02X should be 0x9C, but read data =0x%x \n",__func__,cmdreq.cmds->payload[0],data[0]);
					panel_read_flag = PANEL_READ_ERROR;
				}
				else
				{
					panel_read_flag = PANEL_READ_OK;
				}
				break;
			}
		case 0x0B:
			{
				if(unlikely(data[0] != 0x40))
				{
					printk("panel ic error: in %s,  reg 0x%02X should be 0x40, but read data =0x%x \n",__func__,cmdreq.cmds->payload[0],data[0]);
					panel_read_flag = PANEL_READ_ERROR;
				}
				else
				{
					panel_read_flag = PANEL_READ_OK;
				}
				break;
			}
		case 0x0C:
			{
				if(unlikely(data[0] != 0x07))
				{
					printk("panel ic error: in %s,  reg 0x%02X should be 0x07, but read data =0x%x \n",__func__,cmdreq.cmds->payload[0],data[0]);
					panel_read_flag = PANEL_READ_ERROR;
				}
				else
				{
					panel_read_flag = PANEL_READ_OK;
				}
				break;
			}
		case 0x0D:
			{
				if(unlikely(data[0] != 0x00))
				{
					printk("panel ic error: in %s,  reg 0x%02X should be 0x00, but read data =0x%x \n",__func__,cmdreq.cmds->payload[0],data[0]);
					panel_read_flag = PANEL_READ_ERROR;
				}
				else
				{
					panel_read_flag = PANEL_READ_OK;
				}
				break;
			}
		default:
			panel_read_flag = PANEL_READ_OK;
			break;
	}
}
static void send_req_cmd(struct dsi_cmd_desc *dsi_cmd, int rec_len)
{
	cmdreq.cmds = dsi_cmd;
	cmdreq.cmds_cnt = 1;
	cmdreq.flags = CMD_REQ_RX | CMD_REQ_COMMIT | CMD_CLK_CTRL;
	cmdreq.rlen = rec_len;/*read 1 byte*/
	cmdreq.cb_hw= panel_otm9608a_check_live_status_cb; 
	mipi_dsi_cmdlist_put(&cmdreq);
}
/*if Panel IC works well, return 1, else return 0 */
static int panel_otm9608a_check_live_status(struct msm_fb_data_type *mfd) 
{
	int i = 0;
	int noerr = 0;
	#define REPEAT_COUNT 5
	if (!mfd->is_panel_alive)
	{
		LCD_DEBUG("%s: panel not alive.\n", __func__);
		return noerr;
	}
	for (i=0; i<REPEAT_COUNT; i++)
	{
		send_req_cmd(&otm9608a_read_display_power_mode_cmd,1);
		if(likely(panel_read_flag == PANEL_READ_OK))
		{
			noerr = 1;
			break;
		}
		else
		{
			noerr = 0;
			if(i == REPEAT_COUNT-1)
			{
				return noerr;
			}
		}
		
	}

	for (i=0; i<REPEAT_COUNT; i++)
	{
		send_req_cmd(&otm9608a_read_display_MADCTR_cmd,1);
		if(likely(panel_read_flag == PANEL_READ_OK))
		{	
			noerr = 1;
			break;
		}
		else
		{
			noerr = 0;
			if(i == REPEAT_COUNT-1)
			{
				return noerr;
			}
		}
	}

	for (i=0; i<REPEAT_COUNT; i++)
	{
		send_req_cmd(&otm9608a_read_display_pixel_format_cmd,1);
		if(likely(panel_read_flag == PANEL_READ_OK))
		{	
			noerr = 1;
			break;
		}
		else
		{
			noerr = 0;
			if(i == REPEAT_COUNT-1)
			{
				return noerr;
			}
		}
	}

	for (i=0; i<REPEAT_COUNT; i++)
	{
		send_req_cmd(&otm9608a_read_display_image_mode_cmd,1);
		if(likely(panel_read_flag == PANEL_READ_OK))
		{	
			noerr = 1;
			break;
		}
		else
		{
			noerr = 0;
			if(i == REPEAT_COUNT-1)
			{
				return noerr;
			}
		}
	}
	return noerr;
}

#endif

/*add dynamic gamma from CMI*/
#ifdef CONFIG_FB_DYNAMIC_GAMMA
static const struct sequence otm9608a_gamma_25[]=
{
	//e100h Gamma Correction Characteristics Setting(2.5+)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000e1,MIPI_GEN_COMMAND,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00007,TYPE_PARAMETER,0},
	{0x0000b,TYPE_PARAMETER,0},
	{0x0000c,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x0000d,TYPE_PARAMETER,0},
	{0x0000a,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	{0x00006,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x0000b,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},

	//e200h Gamma Correction Characteristics Setting(2.5-)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000e2,MIPI_GEN_COMMAND,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00007,TYPE_PARAMETER,0},
	{0x0000b,TYPE_PARAMETER,0},
	{0x0000c,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x0000d,TYPE_PARAMETER,0},
	{0x0000a,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	{0x00006,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00010,TYPE_PARAMETER,0},
	{0x0000b,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	{0xFFFFF,MIPI_TYPE_END,0},
};

static const struct sequence otm9608a_gamma_22[]=
{
	//e100h Gamma Correction Characteristics Setting(2.2+)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000e1,MIPI_GEN_COMMAND,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00006,TYPE_PARAMETER,0},
	{0x0000a,TYPE_PARAMETER,0},
	{0x0000c,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x0000c,TYPE_PARAMETER,0},
	{0x0000a,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00015,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},

	//e200h Gamma Correction Characteristics Setting(2.2-)
	{0x00000,MIPI_DCS_COMMAND,0},
	{0x00000,TYPE_PARAMETER,0},
	{0x000e2,MIPI_GEN_COMMAND,0},
	{0x00001,TYPE_PARAMETER,0},
	{0x00006,TYPE_PARAMETER,0},
	{0x0000a,TYPE_PARAMETER,0},
	{0x0000c,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x0000c,TYPE_PARAMETER,0},
	{0x0000a,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0x00005,TYPE_PARAMETER,0},
	{0x00008,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00015,TYPE_PARAMETER,0},
	{0x0000f,TYPE_PARAMETER,0},
	{0x00009,TYPE_PARAMETER,0},
	{0xFFFFF,MIPI_TYPE_END,0},
};

int mipi_otm9608a_set_dynamic_gamma(enum danymic_gamma_mode  gamma_mode,struct msm_fb_data_type *mfd)
{
	int ret = 0;

	switch(gamma_mode)
	{
		case GAMMA25:
			process_mipi_table(mfd,&otm9608a_tx_buf,otm9608a_gamma_25,
						ARRAY_SIZE(otm9608a_gamma_25), lcd_panel_qhd);
			break ;
		case GAMMA22:
			process_mipi_table(mfd,&otm9608a_tx_buf,otm9608a_gamma_22,
						ARRAY_SIZE(otm9608a_gamma_22), lcd_panel_qhd);
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

struct sequence otm9608a_qhd_write_cabc_brightness_table[]= 
{
	/* solve losing control of the backlight */
	{0x00051,MIPI_DCS_COMMAND,0},
	{0x00064,TYPE_PARAMETER,0},
	{0x00,MIPI_TYPE_END,0},
};

static int mipi_otm9608a_lcd_on(struct platform_device *pdev)
{
	boolean para_debug_flag = FALSE;
	uint32 para_num = 0;
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL; 
	#ifdef CONFIG_HW_ESD_DETECT
	if (!mfd->is_panel_alive)
	{
		LCD_DEBUG("%s: panel not alive, reset and reinit begin.\n", __func__);
		lcd_reset();
		/*from sleep out mode, need 120ms delay before send data, for panel IC stable*/
		msleep(120);
		/* change to LP mode then send initial code */
		mipi_set_tx_power_mode(1);
		process_mipi_table(mfd,&otm9608a_tx_buf,otm9608a_chimei_lcd_init_table,
		 	ARRAY_SIZE(otm9608a_chimei_lcd_init_table), lcd_panel_qhd);
		mipi_set_tx_power_mode(0);
		LCD_DEBUG("%s: panel not alive, reset and reinit end.\n", __func__);
		return 0;
	}
	#endif
	para_debug_flag = lcd_debug_malloc_get_para( "otm9608a_lcd_init_table_debug", 
			(void**)&otm9608a_lcd_init_table_debug,&para_num);

	if( (TRUE == para_debug_flag) && (NULL != otm9608a_lcd_init_table_debug))
	{
		process_mipi_table(mfd,&otm9608a_tx_buf,otm9608a_lcd_init_table_debug,
			 para_num, lcd_panel_qhd);
	}
	else
	{
		mipi_set_tx_power_mode(1);
		process_mipi_table(mfd,&otm9608a_tx_buf,otm9608a_qhd_standby_exit_table,
		 	ARRAY_SIZE(otm9608a_qhd_standby_exit_table), lcd_panel_qhd);
		mipi_set_tx_power_mode(0);
	}

	if((TRUE == para_debug_flag)&&(NULL != otm9608a_lcd_init_table_debug))
	{
		lcd_debug_free_para((void *)otm9608a_lcd_init_table_debug);
	}
	pr_info("leave mipi_otm9608a_lcd_on \n");

	return 0;
}

static int mipi_otm9608a_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	mfd = platform_get_drvdata(pdev);
	#ifdef CONFIG_HW_ESD_DETECT
	if (!mfd->is_panel_alive)
	{
		LCD_DEBUG("%s: panel not alive.\n", __func__);
		return 0;
	}
	#endif
	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	process_mipi_table(mfd,&otm9608a_tx_buf,otm9608a_qhd_standby_enter_table,
		 ARRAY_SIZE(otm9608a_qhd_standby_enter_table), lcd_panel_qhd);
	pr_info("leave mipi_otm9608a_lcd_off \n");
	return 0;
}

#ifdef CONFIG_FB_AUTO_CABC
static struct sequence otm9608a_auto_cabc_set_table[] =
{
	{0x00055,MIPI_DCS_COMMAND,0}, 
	{0x00001,TYPE_PARAMETER,0},
	{0x00029,MIPI_TYPE_END,0}, 
};
/***************************************************************
Function: otm9608a_config_cabc
Description: Set CABC configuration
Parameters:
	struct msmfb_cabc_config cabc_cfg: CABC configuration struct
Return:
	0: success
***************************************************************/
static int otm9608a_config_auto_cabc(struct msmfb_cabc_config cabc_cfg,struct msm_fb_data_type *mfd)
{
	int ret = 0;
	#ifdef CONFIG_HW_ESD_DETECT
	if (!mfd->is_panel_alive)
	{
		LCD_DEBUG("%s: panel not alive.\n", __func__);
		return 0;
	}
	#endif
	switch(cabc_cfg.mode)
	{
		case CABC_MODE_UI:
			otm9608a_auto_cabc_set_table[1].reg = 0x0001;
			break;
		case CABC_MODE_MOVING:
		case CABC_MODE_STILL:
			otm9608a_auto_cabc_set_table[1].reg = 0x0003;
			break;
		default:
			LCD_DEBUG("%s: invalid cabc mode: %d\n", __func__, cabc_cfg.mode);
			ret = -EINVAL;
			break;
	}
	if(likely(0 == ret))
	{
		process_mipi_table(mfd,&otm9608a_tx_buf,otm9608a_auto_cabc_set_table,
			 ARRAY_SIZE(otm9608a_auto_cabc_set_table), lcd_panel_qhd);
	}

	LCD_DEBUG("%s: change cabc mode to %d\n",__func__,cabc_cfg.mode);
	return ret;
}
#endif // CONFIG_FB_AUTO_CABC

void mipi_otm9608a_set_backlight(struct msm_fb_data_type *mfd)
{
	
	static int bl_level_flag = 0;
	#ifdef CONFIG_HW_ESD_DETECT
	if (!mfd->is_panel_alive)
	{
		LCD_DEBUG("%s: panel not alive.\n", __func__);
		return ;
	}
	#endif
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
		otm9608a_qhd_write_cabc_brightness_table[1].reg = mfd->bl_level; 
		process_mipi_table(mfd,&otm9608a_tx_buf,otm9608a_qhd_write_cabc_brightness_table,
			 ARRAY_SIZE(otm9608a_qhd_write_cabc_brightness_table), lcd_panel_qhd);
		return;
	}
}


static int __devinit mipi_otm9608a_lcd_probe(struct platform_device *pdev)
{
	struct platform_device *current_pdev = NULL;

	current_pdev = msm_fb_add_device(pdev);

	if (current_pdev == NULL) 
	{
		printk("error in %s\n",__func__);
	}
	return 0;
}

#ifdef CONFIG_HW_ESD_DETECT
#ifdef CONFIG_HUAWEI_KERNEL
/*add qcom patch to solve esd issue*/
/*if the mipi dsi works well, then check LCD panel regs, if they are all good, regard the display route ok*/
static int mipi_otm9608a_check_live_status(struct msm_fb_data_type *mfd)
{
	int ret_bta = 0;
	int panel_status = -1;
	static int check_cnt = 0;
	#define IGNORE_COUNT 3
	/* do not check while booting, ignore the first IGNORE_COUNT times */
	if (check_cnt < IGNORE_COUNT)
	{
		check_cnt++;
		return CMI_OTM9608A_PANEL_ALIVE;
	}

	ret_bta = mipi_dsi_wait_for_bta_ack();
	//ret_bta = 0;//liguoshengfred
	/* read registers of LCD IC when bta check return good */
	if (likely(ret_bta > 0))
	{
		panel_status = panel_otm9608a_check_live_status(mfd);
		if (unlikely(panel_status == 0))
		{
			//mfd->is_panel_alive = FALSE;
			LCD_DEBUG("display status : bta=%d, panel=%d.\n", ret_bta, panel_status);
		}
		else
		{
			//mfd->is_panel_alive = TRUE;
		}
	}
	else
	{
		//mfd->is_panel_alive = FALSE;
		LCD_DEBUG("mipi dsi status : bta=%d.\n", ret_bta);
	}
	return ((panel_status == 0) ? CMI_OTM9608A_PANEL_DEAD : ret_bta);//ok, >0, else =0

}
#else
static int mipi_otm9608a_check_live_status(struct msm_fb_data_type *mfd)
{
	return mipi_dsi_wait_for_bta_ack();
}
#endif
#endif

static struct platform_driver this_driver = {
	.probe  = mipi_otm9608a_lcd_probe,
	.driver = {
		.name   = "mipi_otm9608a",
	},
};

struct msm_fb_panel_data otm9608a_panel_data = {
	.on		= mipi_otm9608a_lcd_on,
	.off		= mipi_otm9608a_lcd_off,
	.set_backlight = pwm_set_backlight,
	.set_brightness = mipi_otm9608a_set_backlight,
	#ifdef CONFIG_FB_AUTO_CABC
	.config_cabc = otm9608a_config_auto_cabc,
	#endif
	/*add dynamic gamma interface*/
	#ifdef CONFIG_FB_DYNAMIC_GAMMA
	.set_dynamic_gamma = mipi_otm9608a_set_dynamic_gamma,
	#endif
	#ifdef CONFIG_HW_ESD_DETECT
	/*add qcom patch to solve esd issue*/
	.check_live_status = mipi_otm9608a_check_live_status,
	#endif
};

static int ch_used[3];

int mipi_otm9608a_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;
	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;
	ret = mipi_otm9608a_lcd_init();
	if (ret) 
	{
		pr_err("mipi_otm9608a_lcd_init() failed with ret %u\n", ret);
		goto driver_failed;
	}
	
	pdev = platform_device_alloc("mipi_otm9608a", (panel << 8)|channel);
	if (!pdev)
	{
		ret = -ENOMEM;
		goto driver_failed;
	}

	otm9608a_panel_data.panel_info = *pinfo;
	ret = platform_device_add_data(pdev, &otm9608a_panel_data,sizeof(otm9608a_panel_data));
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
	kfree(otm9608a_tx_buf.data);
	memset(&otm9608a_tx_buf,0,sizeof(struct dsi_buf));
	return ret;
}

static int mipi_otm9608a_lcd_init(void)
{
	led_trigger_register_simple("bkl_trigger", &bkl_led_trigger);
	pr_info("%s: SUCCESS (WLED TRIGGER)\n", __func__);
#ifdef CONFIG_FB_PM8038_CABC_PIN
	wled_trigger_initialized = 0;
#else
	wled_trigger_initialized = 1;
#endif
	mipi_dsi_buf_alloc(&otm9608a_tx_buf, DSI_BUF_SIZE);
	return platform_driver_register(&this_driver);
}

static int __init mipi_cmd_otm9608a_qhd_init(void)
{
	int ret;
	if(0 == lcd_detect_panel(LCD_PANEL_NAME))
	{
	}
	else
	{
		return 0;
	}
	pr_info("enter mipi_cmd_otm9608a_qhd_init \n");

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

	pinfo.mipi.mode=DSI_CMD_MODE;
	pinfo.mipi.dst_format=DSI_CMD_DST_FORMAT_RGB888;
	pinfo.mipi.rgb_swap=DSI_RGB_SWAP_RGB;
	pinfo.mipi.dlane_swap = 0x01;

	ret = mipi_otm9608a_device_register(&pinfo, MIPI_DSI_PRIM,MIPI_DSI_PANEL_QHD_PT);
	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	return ret;
}

module_init(mipi_cmd_otm9608a_qhd_init);

