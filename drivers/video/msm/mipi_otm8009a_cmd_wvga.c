#include <linux/leds.h>
#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mdp4.h"
#include "hw_lcd_common.h"
#include <hsad/config_interface.h>
#include <linux/hw_backlight.h>
#include <linux/msm_mdp.h>
static struct msm_panel_info pinfo;
static lcd_panel_type lcd_panel_wvga = LCD_NONE;

static struct dsi_buf otm8009a_tx_buf;

static struct sequence * otm8009a_lcd_init_table_debug = NULL;
/*backlight control mode,1 means by WLED*/
static int wled_trigger_initialized;

static int mipi_otm8009a_lcd_init(void);
static void mipi_otm8009a_set_backlight(struct msm_fb_data_type *mfd);

#define LCD_PANEL_CHIMEI	"mipi_otm8009a_chimei_wvga"
#ifdef CONFIG_HW_ESD_DETECT
#define CMI_OTM8009A_PANEL_ALIVE    (1)
#define CMI_OTM8009A_PANEL_DEAD     (0)
#endif
#ifdef CONFIG_FB_DYNAMIC_GAMMA
static const struct sequence *otm8900a_gamma_25 = NULL;
static const struct sequence *otm8900a_gamma_22 = NULL;
static size_t gamma_25_len = 0;
static size_t gamma_22_len = 0;
#endif

DEFINE_LED_TRIGGER(bkl_led_trigger);
static struct mipi_dsi_phy_ctrl dsi_cmd_mode_phy_db = 
{
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

const struct sequence otm8009a_wvga_standby_enter_table[]= 
{
	/*close Vsync singal,when lcd sleep in*/

	{0x00028,MIPI_DCS_COMMAND,0}, //28h
	{0x0010,MIPI_DCS_COMMAND,20},
	{0x0010,MIPI_TYPE_END,150}, // add new command for 
};
const struct sequence otm8009a_wvga_standby_exit_table[] =
{
	{0x00011,MIPI_GEN_COMMAND,0},
	{0x00029,MIPI_GEN_COMMAND,150},
	{0x00029,MIPI_TYPE_END,20},	        
};
#ifdef CONFIG_FB_DYNAMIC_GAMMA
static const struct sequence otm8009a_gamma_25[]=
{
	                                                                         
	//GAMMA 2.5                                                                                   
    //address E500h                         
	{0x00,TYPE_PARAMETER,0},
	{0x00,MIPI_GEN_COMMAND,0},
    {0xE5,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x0B,TYPE_PARAMETER,0},
  	{0x11,TYPE_PARAMETER,0},
    {0x0F,TYPE_PARAMETER,0},
    {0x07,TYPE_PARAMETER,0},
    {0x0D,TYPE_PARAMETER,0},
    {0x06,TYPE_PARAMETER,0},
    {0x03,TYPE_PARAMETER,0},
  	{0x09,TYPE_PARAMETER,0},
    {0x0B,TYPE_PARAMETER,0},
    {0x0E,TYPE_PARAMETER,0},
    {0x02,TYPE_PARAMETER,0},
    {0x10,TYPE_PARAMETER,0},
    {0x0E,TYPE_PARAMETER,0},
    {0x07,TYPE_PARAMETER,0},
    {0x06,TYPE_PARAMETER,0},    
    
    //address E600h                                 
    {0x00,MIPI_GEN_COMMAND,0},
	{0x00,TYPE_PARAMETER,0},
	{0xE6,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},
    {0x0D,TYPE_PARAMETER,0},
    {0x11,TYPE_PARAMETER,0},
  	{0x0D,TYPE_PARAMETER,0},
    {0x06,TYPE_PARAMETER,0},
    {0x0D,TYPE_PARAMETER,0},
    {0x0B,TYPE_PARAMETER,0},
    {0x0B,TYPE_PARAMETER,0},
    {0x04,TYPE_PARAMETER,0},
	{0x09,TYPE_PARAMETER,0},
    {0x0D,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x07,TYPE_PARAMETER,0},
    {0x09,TYPE_PARAMETER,0},
    {0x03,TYPE_PARAMETER,0},
    {0x01,TYPE_PARAMETER,0},     
};

static const struct sequence otm8009a_gamma_22[]=
{
	//GAMMA 2.2 // 20120504                                                                                                   
    //address E100h                         
    {0x00,MIPI_GEN_COMMAND,0},
	{0x00,TYPE_PARAMETER,0},
	{0xE1,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},
    {0x07,TYPE_PARAMETER,0},
    {0x0F,TYPE_PARAMETER,0},
  	{0x11,TYPE_PARAMETER,0},
    {0x09,TYPE_PARAMETER,0},
    {0x0E,TYPE_PARAMETER,0},
    {0x09,TYPE_PARAMETER,0},
    {0x05,TYPE_PARAMETER,0},
    {0x06,TYPE_PARAMETER,0},
  	{0x08,TYPE_PARAMETER,0},
    {0x0D,TYPE_PARAMETER,0},
    {0x06,TYPE_PARAMETER,0},
    {0x1D,TYPE_PARAMETER,0},
    {0x17,TYPE_PARAMETER,0},
    {0x0D,TYPE_PARAMETER,0},
    {0x06,TYPE_PARAMETER,0},
    
    //address E200h                                 
    {0x00,MIPI_GEN_COMMAND,0},
	{0x00,TYPE_PARAMETER,0},
	{0xE2,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},
    {0x07,TYPE_PARAMETER,0},
    {0x0F,TYPE_PARAMETER,0},
  	{0x11,TYPE_PARAMETER,0},
    {0x09,TYPE_PARAMETER,0},
    {0x0C,TYPE_PARAMETER,0},
    {0x0E,TYPE_PARAMETER,0},
    {0x0F,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x06,TYPE_PARAMETER,0},
    {0x0F,TYPE_PARAMETER,0},
    {0x0C,TYPE_PARAMETER,0},
    {0x13,TYPE_PARAMETER,0},
    {0x0D,TYPE_PARAMETER,0},
    {0x09,TYPE_PARAMETER,0},
    {0x01,TYPE_PARAMETER,0},         
};
#ifdef CONFIG_HW_ESD_DETECT
/* CHIMEI LCD init code */
static struct sequence otm8009a_chimei_lcd_init_table[] =
{
	//  address FF00h         set password
	{0x00,MIPI_GEN_COMMAND,0},
	{0x00,TYPE_PARAMETER,0},
	{0xFF,MIPI_GEN_COMMAND,0},         
    {0x80,TYPE_PARAMETER,0},
	{0x09,TYPE_PARAMETER,0},
	{0x01,TYPE_PARAMETER,0},
            
    //address FF80h
    {0x00,MIPI_GEN_COMMAND,0},
	{0x80,TYPE_PARAMETER,0},
	{0xFF,MIPI_GEN_COMMAND,0},         
    {0x80,TYPE_PARAMETER,0},
	{0x09,TYPE_PARAMETER,0},
               
    //For power off sequence Power delay 2 frame OFF.                            
    {0x00,MIPI_GEN_COMMAND,0},
	{0x80,TYPE_PARAMETER,0},
	{0xF5,MIPI_GEN_COMMAND,0},
    {0x01,TYPE_PARAMETER,0},
    {0x18,TYPE_PARAMETER,0},
    {0x02,TYPE_PARAMETER,0},
  	{0x18,TYPE_PARAMETER,0},
    {0x10,TYPE_PARAMETER,0},
    {0x18,TYPE_PARAMETER,0},
	{0x02,TYPE_PARAMETER,0},
    {0x18,TYPE_PARAMETER,0},
    {0x0e,TYPE_PARAMETER,0},
	{0x18,TYPE_PARAMETER,0},
    {0x0f,TYPE_PARAMETER,0},
    {0x20,TYPE_PARAMETER,0},
                   
    //For power off sequence Power delay 2 frame OFF.                               
	{0x00,MIPI_GEN_COMMAND,0},
	{0x90,TYPE_PARAMETER,0},
	{0xF5,MIPI_GEN_COMMAND,0},
    {0x02,TYPE_PARAMETER,0},
    {0x18,TYPE_PARAMETER,0},
    {0x08,TYPE_PARAMETER,0},
  	{0x18,TYPE_PARAMETER,0},
    {0x06,TYPE_PARAMETER,0},
    {0x18,TYPE_PARAMETER,0},
	{0x0d,TYPE_PARAMETER,0},
	{0x18,TYPE_PARAMETER,0},
    {0x0b,TYPE_PARAMETER,0},
    {0x18,TYPE_PARAMETER,0},
	   
    //For power off sequence Power delay 2 frame OFF.                 
    {0x00,MIPI_GEN_COMMAND,0},
	{0xA0,TYPE_PARAMETER,0},
	{0xF5,MIPI_GEN_COMMAND,0},
    {0x10,TYPE_PARAMETER,0},
    {0x18,TYPE_PARAMETER,0},
    {0x01,TYPE_PARAMETER,0},
  	{0x18,TYPE_PARAMETER,0},
    {0x14,TYPE_PARAMETER,0},
    {0x18,TYPE_PARAMETER,0},
	{0x14,TYPE_PARAMETER,0},
	{0x18,TYPE_PARAMETER,0},
	        
    //For power off sequence Power delay 2 frame OFF.                 
    {0x00,MIPI_GEN_COMMAND,0},
	{0xB0,TYPE_PARAMETER,0},
	{0xF5,MIPI_GEN_COMMAND,0},
    {0x14,TYPE_PARAMETER,0},
    {0x18,TYPE_PARAMETER,0},
    {0x12,TYPE_PARAMETER,0},
  	{0x18,TYPE_PARAMETER,0},
    {0x13,TYPE_PARAMETER,0},
    {0x18,TYPE_PARAMETER,0},
	{0x11,TYPE_PARAMETER,0},
	{0x18,TYPE_PARAMETER,0},
	{0x13,TYPE_PARAMETER,0},
    {0x18,TYPE_PARAMETER,0},
	{0x00,TYPE_PARAMETER,0},
	{0x00,TYPE_PARAMETER,0},                        

    //address C0B4h 00h=>1 dot inversion 11h=>2 dot inversion 55h=>column inversion
    {0x00,MIPI_GEN_COMMAND,0},
    {0xB4,TYPE_PARAMETER,0},
    {0xC0,MIPI_GEN_COMMAND,0},
    {0x55,TYPE_PARAMETER,0},                 
           
    //address C582h                 
    {0x00,MIPI_GEN_COMMAND,0},
    {0x82,TYPE_PARAMETER,0},
    {0xC5,MIPI_GEN_COMMAND,0},
    {0xA3,TYPE_PARAMETER,0},    
    
    //address C590h
    {0x00,MIPI_GEN_COMMAND,0},
    {0x90,TYPE_PARAMETER,0},
    {0xC5,MIPI_GEN_COMMAND,0},
    {0x76,TYPE_PARAMETER,0},          

    //address D800h Set  GVDD/NGVDD
    {0x00,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},
    {0xD8,MIPI_GEN_COMMAND,0},
    {0xAF,TYPE_PARAMETER,0},
    {0xAF,TYPE_PARAMETER,0},                  
    
    //address D900h VCOM Voltage setti. 
    {0x00,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},
    {0xD9,MIPI_GEN_COMMAND,0},
    {0x85,TYPE_PARAMETER,0},                             

    //address C181h
    //lcd frame_rate 60 HZ
    {0x00,MIPI_GEN_COMMAND,0},
    {0x81,TYPE_PARAMETER,0},
    {0xC1,MIPI_GEN_COMMAND,0},
    {0x55,TYPE_PARAMETER,0},                         
    
    //address C1A1h 
    {0x00,MIPI_GEN_COMMAND,0},
    {0xA1,TYPE_PARAMETER,0},
    {0xC1,MIPI_GEN_COMMAND,0},
    {0x08,TYPE_PARAMETER,0},                
    
    //address C0A3h
    {0x00,MIPI_GEN_COMMAND,0},
    {0xA3,TYPE_PARAMETER,0},
    {0xC0,MIPI_GEN_COMMAND,0},
    {0x1B,TYPE_PARAMETER,0},                  

	//address C480h
    {0x00,MIPI_GEN_COMMAND,0},
    {0x80,TYPE_PARAMETER,0},
    {0xC4,MIPI_GEN_COMMAND,0},
    {0x30,TYPE_PARAMETER,0}, 
		
    //address C481h
    {0x00,MIPI_GEN_COMMAND,0},
    {0x81,TYPE_PARAMETER,0},
    {0xC4,MIPI_GEN_COMMAND,0},
    {0x83,TYPE_PARAMETER,0},                    

	//address C48Ah
    {0x00,MIPI_GEN_COMMAND,0},
    {0x8A,TYPE_PARAMETER,0},
    {0xC4,MIPI_GEN_COMMAND,0},
    {0x40,TYPE_PARAMETER,0}, 
		
    //address C592h
    {0x00,MIPI_GEN_COMMAND,0},
    {0x92,TYPE_PARAMETER,0},
    {0xC5,MIPI_GEN_COMMAND,0},
    {0x01,TYPE_PARAMETER,0},                  
    
    //address C5B1h
    {0x00,MIPI_GEN_COMMAND,0},
    {0xB1,TYPE_PARAMETER,0},
    {0xC5,MIPI_GEN_COMMAND,0},
    {0xA9,TYPE_PARAMETER,0},                    

	//address B390h
	{0x00,MIPI_GEN_COMMAND,0},
    {0x90,TYPE_PARAMETER,0},
    {0xB3,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0}, 
		
	//address B392h
	{0x00,MIPI_GEN_COMMAND,0},
    {0x92,TYPE_PARAMETER,0},
    {0xB3,MIPI_GEN_COMMAND,0},
    {0x05,TYPE_PARAMETER,0},

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
    //address CE80h
    {0x00,MIPI_GEN_COMMAND,0},
    {0x80,TYPE_PARAMETER,0},
    {0xCE,MIPI_GEN_COMMAND,0},
    {0x84,TYPE_PARAMETER,0},
    {0x03,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x83,TYPE_PARAMETER,0},
    {0x03,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},   
          
    //address CE90h//
    {0x00,MIPI_GEN_COMMAND,0},
	{0x90,TYPE_PARAMETER,0},
	{0xCE,MIPI_GEN_COMMAND,0},
    {0x33,TYPE_PARAMETER,0},
    {0x27,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x33,TYPE_PARAMETER,0},
    {0x28,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},                                  
    
    //address CEA0h         
    {0x00,MIPI_GEN_COMMAND,0},
	{0xA0,TYPE_PARAMETER,0},
	{0xCE,MIPI_GEN_COMMAND,0},
    {0x38,TYPE_PARAMETER,0},
    {0x02,TYPE_PARAMETER,0},
    {0x03,TYPE_PARAMETER,0},
  	{0x21,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x38,TYPE_PARAMETER,0},
    {0x01,TYPE_PARAMETER,0},
  	{0x03,TYPE_PARAMETER,0},
    {0x22,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},         
    
    //address CEB0h                  
    {0x00,MIPI_GEN_COMMAND,0},
	{0xB0,TYPE_PARAMETER,0},
	{0xCE,MIPI_GEN_COMMAND,0},
    {0x38,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x03,TYPE_PARAMETER,0},
  	{0x23,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x30,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x03,TYPE_PARAMETER,0},
    {0x24,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},                          
    
    //address CEC0h        //                 
    {0x00,MIPI_GEN_COMMAND,0},
	{0xC0,TYPE_PARAMETER,0},
	{0xCE,MIPI_GEN_COMMAND,0},
    {0x30,TYPE_PARAMETER,0},
    {0x01,TYPE_PARAMETER,0},
    {0x03,TYPE_PARAMETER,0},
  	{0x25,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x30,TYPE_PARAMETER,0},
    {0x02,TYPE_PARAMETER,0},
  	{0x03,TYPE_PARAMETER,0},
    {0x26,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},           
    
    //address CED0h        //                         
    {0x00,MIPI_GEN_COMMAND,0},
	{0xD0,TYPE_PARAMETER,0},
	{0xCE,MIPI_GEN_COMMAND,0},
    {0x30,TYPE_PARAMETER,0},
    {0x03,TYPE_PARAMETER,0},
    {0x03,TYPE_PARAMETER,0},
  	{0x27,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x30,TYPE_PARAMETER,0},
    {0x04,TYPE_PARAMETER,0},
  	{0x03,TYPE_PARAMETER,0},
    {0x28,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},     
    
    //address CFC7h
    {0x00,MIPI_GEN_COMMAND,0},
	{0xC7,TYPE_PARAMETER,0},
	{0xCF,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},                 

    //address CBC0h        //For power off sequence        //GOA timing delay 2 frame OFF.                                 
    {0x00,MIPI_GEN_COMMAND,0},
	{0xC0,TYPE_PARAMETER,0},
	{0xCB,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x00,TYPE_PARAMETER,0},
    {0x54,TYPE_PARAMETER,0},
    {0x54,TYPE_PARAMETER,0},
    {0x54,TYPE_PARAMETER,0},
    {0x54,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x54,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x54,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},                
    
    //address CBD0h        //For power off sequence        //GOA timing delay 2 frame OFF.                         
    {0x00,MIPI_GEN_COMMAND,0},
	{0xD0,TYPE_PARAMETER,0},
	{0xCB,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x54,TYPE_PARAMETER,0},
    {0x54,TYPE_PARAMETER,0},
    {0x54,TYPE_PARAMETER,0},
    {0x54,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x54,TYPE_PARAMETER,0},    
    
    //address CBE0h        //For power off sequence        //GOA timing delay 2 frame OFF.                         
    {0x00,MIPI_GEN_COMMAND,0},
	{0xE0,TYPE_PARAMETER,0},
	{0xCB,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},
    {0x54,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    
    //address CC80h                         
    {0x00,MIPI_GEN_COMMAND,0},
	{0x80,TYPE_PARAMETER,0},
	{0xCC,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x00,TYPE_PARAMETER,0},
    {0x0C,TYPE_PARAMETER,0},
    {0x0A,TYPE_PARAMETER,0},
    {0x10,TYPE_PARAMETER,0},
    {0x0E,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x02,TYPE_PARAMETER,0},
    
    //address CC90h         
    {0x00,MIPI_GEN_COMMAND,0},
	{0x90,TYPE_PARAMETER,0},
	{0xCC,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},
    {0x06,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x0B,TYPE_PARAMETER,0},  
    
    //address CCA0h                                 
    {0x00,MIPI_GEN_COMMAND,0},
	{0xA0,TYPE_PARAMETER,0},
	{0xCC,MIPI_GEN_COMMAND,0},
    {0x09,TYPE_PARAMETER,0},
    {0x0F,TYPE_PARAMETER,0},
    {0x0D,TYPE_PARAMETER,0},
  	{0x00,TYPE_PARAMETER,0},
    {0x01,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x05,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    
    //address CCB0h                                 
    {0x00,MIPI_GEN_COMMAND,0},
	{0xB0,TYPE_PARAMETER,0},
	{0xCC,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x00,TYPE_PARAMETER,0},
    {0x0D,TYPE_PARAMETER,0},
    {0x0F,TYPE_PARAMETER,0},
    {0x09,TYPE_PARAMETER,0},
    {0x0B,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x05,TYPE_PARAMETER,0},
    
    //address CCC0h                         
    {0x00,MIPI_GEN_COMMAND,0},
	{0xC0,TYPE_PARAMETER,0},
	{0xCC,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},
    {0x01,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x0E,TYPE_PARAMETER,0}, 
      
    //address CCD0h                         
    {0x00,MIPI_GEN_COMMAND,0},
	{0xD0,TYPE_PARAMETER,0},
	{0xCC,MIPI_GEN_COMMAND,0},
    {0x10,TYPE_PARAMETER,0},
    {0x0A,TYPE_PARAMETER,0},
    {0x0C,TYPE_PARAMETER,0},
  	{0x00,TYPE_PARAMETER,0},
    {0x06,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x02,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
  	{0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},
    {0x00,TYPE_PARAMETER,0},   

	//GAMMA 2.2 // 20120504                                                                                                   
    //address E100h                         
    {0x00,MIPI_GEN_COMMAND,0},
	{0x00,TYPE_PARAMETER,0},
	{0xE1,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},
    {0x14,TYPE_PARAMETER,0},
    {0x18,TYPE_PARAMETER,0},
  	{0x0E,TYPE_PARAMETER,0},
    {0x07,TYPE_PARAMETER,0},
    {0x11,TYPE_PARAMETER,0},
    {0x0B,TYPE_PARAMETER,0},
    {0x09,TYPE_PARAMETER,0},
    {0x04,TYPE_PARAMETER,0},
  	{0x07,TYPE_PARAMETER,0},
    {0x0C,TYPE_PARAMETER,0},
    {0x09,TYPE_PARAMETER,0},
    {0x10,TYPE_PARAMETER,0},
    {0x0C,TYPE_PARAMETER,0},
    {0x09,TYPE_PARAMETER,0},
    {0x06,TYPE_PARAMETER,0},
    
    //address E200h                                 
    {0x00,MIPI_GEN_COMMAND,0},
	{0x00,TYPE_PARAMETER,0},
	{0xE2,MIPI_GEN_COMMAND,0},
    {0x00,TYPE_PARAMETER,0},
    {0x14,TYPE_PARAMETER,0},
    {0x18,TYPE_PARAMETER,0},
  	{0x0E,TYPE_PARAMETER,0},
    {0x07,TYPE_PARAMETER,0},
    {0x11,TYPE_PARAMETER,0},
    {0x0B,TYPE_PARAMETER,0},
    {0x09,TYPE_PARAMETER,0},
    {0x04,TYPE_PARAMETER,0},
  	{0x07,TYPE_PARAMETER,0},
    {0x0C,TYPE_PARAMETER,0},
    {0x09,TYPE_PARAMETER,0},
    {0x10,TYPE_PARAMETER,0},
    {0x0C,TYPE_PARAMETER,0},
    {0x09,TYPE_PARAMETER,0},
    {0x06,TYPE_PARAMETER,0},     

	{0x00036,MIPI_GEN_COMMAND,0},// mx=1
	{0x00040,TYPE_PARAMETER,0},
    //enable lcd output pwm
	{0x00051,MIPI_DCS_COMMAND,0}, // displaybright
	{0x0008f,TYPE_PARAMETER,0}, 
	{0x00053,MIPI_DCS_COMMAND,0}, // ctrldisplay1
	{0x00024,TYPE_PARAMETER,0}, 
	{0x00055,MIPI_DCS_COMMAND,0}, // ctrldisplay2
	{0x00001,TYPE_PARAMETER,0},
	{0x0005E,MIPI_DCS_COMMAND,0},
	{0x00030,TYPE_PARAMETER,0},
	
	{0x00011,MIPI_DCS_COMMAND,0},	//sleep out 
	{0x00029,MIPI_DCS_COMMAND,150}, //Display ON
	{0x00000,MIPI_TYPE_END,20}, //end flag
};
static char otm8009a_read_display_power_mode[2] = {0x0A, 0x00};
static char otm8009a_read_display_MADCTR[2] = {0x0B, 0x00};
static char otm8009a_read_display_pixel_format[2] = {0x0C, 0x00};
static char otm8009a_read_display_image_mode[2] = {0x0D, 0x00};

static struct dsi_cmd_desc otm8009a_read_display_power_mode_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 3, sizeof(otm8009a_read_display_power_mode), otm8009a_read_display_power_mode};
static struct dsi_cmd_desc otm8009a_read_display_MADCTR_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 3, sizeof(otm8009a_read_display_MADCTR), otm8009a_read_display_MADCTR};
static struct dsi_cmd_desc otm8009a_read_display_pixel_format_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 3, sizeof(otm8009a_read_display_pixel_format), otm8009a_read_display_pixel_format};
static struct dsi_cmd_desc otm8009a_read_display_image_mode_cmd = {
	DTYPE_DCS_READ, 1, 0, 1, 3, sizeof(otm8009a_read_display_image_mode), otm8009a_read_display_image_mode};

static struct dcs_cmd_req cmdreq;
static int panel_read_flag = -1;
#define PANEL_READ_ERROR 0
#define PANEL_READ_OK 1
static void panel_otm8009a_check_live_status_cb(char* data)
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
	cmdreq.cb_hw= panel_otm8009a_check_live_status_cb; 
	mipi_dsi_cmdlist_put(&cmdreq);
}
/*if Panel IC works well, return 1, else return 0 */
static int panel_otm8009a_check_live_status(struct msm_fb_data_type *mfd) 
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
		send_req_cmd(&otm8009a_read_display_power_mode_cmd,1);
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
		send_req_cmd(&otm8009a_read_display_MADCTR_cmd,1);
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
		send_req_cmd(&otm8009a_read_display_pixel_format_cmd,1);
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
		send_req_cmd(&otm8009a_read_display_image_mode_cmd,1);
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
int mipi_otm8009a_set_dynamic_gamma(enum danymic_gamma_mode  gamma_mode,struct msm_fb_data_type *mfd)
{
	int ret = 0;
    
	if((!otm8900a_gamma_25)||(!otm8900a_gamma_25))
	{
		LCD_DEBUG("%s:don't support dynamic_gamma\n",__func__);
		return -EPERM;
	}
	switch(gamma_mode)
	{
		case GAMMA25:
			process_mipi_table(mfd,&otm8009a_tx_buf,otm8009a_gamma_25,
						ARRAY_SIZE(otm8009a_gamma_25), lcd_panel_wvga);
			break ;
		case GAMMA22:
			process_mipi_table(mfd,&otm8009a_tx_buf,otm8009a_gamma_22,
						ARRAY_SIZE(otm8009a_gamma_22), lcd_panel_wvga);
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

struct sequence otm8009a_wvga_write_cabc_brightness_table[]= 
{
	/* solve losing control of the backlight */
	{0x00051,MIPI_DCS_COMMAND,0},
	{0x00064,TYPE_PARAMETER,0},
	{0x00,MIPI_TYPE_END,0},
};

static int mipi_otm8009a_lcd_on(struct platform_device *pdev)
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
		process_mipi_table(mfd,&otm8009a_tx_buf,otm8009a_chimei_lcd_init_table,
		 	ARRAY_SIZE(otm8009a_chimei_lcd_init_table), lcd_panel_wvga);
		mipi_set_tx_power_mode(0);
		LCD_DEBUG("%s: panel not alive, reset and reinit end.\n", __func__);
		return 0;
	}
	#endif
	para_debug_flag = lcd_debug_malloc_get_para( "otm8009a_lcd_init_table_debug", 
			(void**)&otm8009a_lcd_init_table_debug,&para_num);

	if( (TRUE == para_debug_flag) && (NULL != otm8009a_lcd_init_table_debug))
	{
		process_mipi_table(mfd,&otm8009a_tx_buf,otm8009a_lcd_init_table_debug,
			 para_num, lcd_panel_wvga);
	}
	else
	{
		mipi_set_tx_power_mode(1);
		process_mipi_table(mfd,&otm8009a_tx_buf,otm8009a_wvga_standby_exit_table,
		 	ARRAY_SIZE(otm8009a_wvga_standby_exit_table), lcd_panel_wvga);
		mipi_set_tx_power_mode(0);
	}

	if((TRUE == para_debug_flag)&&(NULL != otm8009a_lcd_init_table_debug))
	{
		lcd_debug_free_para((void *)otm8009a_lcd_init_table_debug);
	}
	pr_info("leave mipi_otm8009a_lcd_on \n");

	return 0;
}

static int mipi_otm8009a_lcd_off(struct platform_device *pdev)
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

	process_mipi_table(mfd,&otm8009a_tx_buf,otm8009a_wvga_standby_enter_table,
		 ARRAY_SIZE(otm8009a_wvga_standby_enter_table), lcd_panel_wvga);
	pr_info("leave mipi_otm8009a_lcd_off \n");
	return 0;
}

#ifdef CONFIG_FB_AUTO_CABC
static struct sequence otm8009a_auto_cabc_set_table[] =
{
	{0x00055,MIPI_DCS_COMMAND,0}, 
	{0x00001,TYPE_PARAMETER,0},
	{0x00053,MIPI_DCS_COMMAND,0}, 
	{0x00024,TYPE_PARAMETER,0},
	{0x00029,MIPI_TYPE_END,0}, 
};
/***************************************************************
Function: otm8009a_config_cabc
Description: Set CABC configuration
Parameters:
	struct msmfb_cabc_config cabc_cfg: CABC configuration struct
Return:
	0: success
***************************************************************/
static int otm8009a_config_auto_cabc(struct msmfb_cabc_config cabc_cfg,struct msm_fb_data_type *mfd)
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
			otm8009a_auto_cabc_set_table[1].reg = 0x0001;
			otm8009a_auto_cabc_set_table[3].reg = 0x00024;
			break;
		case CABC_MODE_MOVING:
		case CABC_MODE_STILL:
			otm8009a_auto_cabc_set_table[1].reg = 0x0003;
			otm8009a_auto_cabc_set_table[3].reg = 0x0002c;
			break;
		default:
			LCD_DEBUG("%s: invalid cabc mode: %d\n", __func__, cabc_cfg.mode);
			ret = -EINVAL;
			break;
	}
	if(likely(0 == ret))
	{
		process_mipi_table(mfd,&otm8009a_tx_buf,otm8009a_auto_cabc_set_table,
			 ARRAY_SIZE(otm8009a_auto_cabc_set_table), lcd_panel_wvga);
	}

	LCD_DEBUG("%s: change cabc mode to %d\n",__func__,cabc_cfg.mode);
	return ret;
}
#endif // CONFIG_FB_AUTO_CABC

void mipi_otm8009a_set_backlight(struct msm_fb_data_type *mfd)
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
		otm8009a_wvga_write_cabc_brightness_table[1].reg = mfd->bl_level; 
		process_mipi_table(mfd,&otm8009a_tx_buf,otm8009a_wvga_write_cabc_brightness_table,
			 ARRAY_SIZE(otm8009a_wvga_write_cabc_brightness_table), lcd_panel_wvga);
		return;
	}
}


static int __devinit mipi_otm8009a_lcd_probe(struct platform_device *pdev)
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
static int mipi_otm8009a_check_live_status(struct msm_fb_data_type *mfd)
{
	int ret_bta = 0;
	int panel_status = -1;
	static int check_cnt = 0;
	#define IGNORE_COUNT 3
	/* do not check while booting, ignore the first IGNORE_COUNT times */
	if (check_cnt < IGNORE_COUNT)
	{
		check_cnt++;
		return CMI_OTM8009A_PANEL_ALIVE;
	}

	ret_bta = mipi_dsi_wait_for_bta_ack();
	//ret_bta = 0;
	/* read registers of LCD IC when bta check return good */
	if (likely(ret_bta > 0))
	{
		panel_status = panel_otm8009a_check_live_status(mfd);
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
	return ((panel_status == 0) ? CMI_OTM8009A_PANEL_DEAD : ret_bta);//ok, >0, else =0

}
#else
static int mipi_otm8009a_check_live_status(struct msm_fb_data_type *mfd)
{
	return mipi_dsi_wait_for_bta_ack();
}
#endif
#endif

static struct platform_driver this_driver = {
	.probe  = mipi_otm8009a_lcd_probe,
	.driver = {
		.name   = "mipi_otm8009a",
	},
};

struct msm_fb_panel_data otm8009a_panel_data = {
	.on		= mipi_otm8009a_lcd_on,
	.off		= mipi_otm8009a_lcd_off,
	.set_backlight = pwm_set_backlight,
	.set_brightness = mipi_otm8009a_set_backlight,
	#ifdef CONFIG_FB_AUTO_CABC
	.config_cabc = otm8009a_config_auto_cabc,
#endif
	#ifdef CONFIG_FB_DYNAMIC_GAMMA
	.set_dynamic_gamma = mipi_otm8009a_set_dynamic_gamma,
	#endif
	#ifdef CONFIG_HW_ESD_DETECT
	/*add qcom patch to solve esd issue*/
	.check_live_status = mipi_otm8009a_check_live_status,
	#endif
};

static int ch_used[3];

int mipi_otm8009a_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;
	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;
	ret = mipi_otm8009a_lcd_init();
	if (ret) 
	{
		pr_err("mipi_otm8009a_lcd_init() failed with ret %u\n", ret);
		goto driver_failed;
	}
	
	pdev = platform_device_alloc("mipi_otm8009a", (panel << 8)|channel);
	if (!pdev)
	{
		ret = -ENOMEM;
		goto driver_failed;
	}

	otm8009a_panel_data.panel_info = *pinfo;
	ret = platform_device_add_data(pdev, &otm8009a_panel_data,sizeof(otm8009a_panel_data));
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
	kfree(otm8009a_tx_buf.data);
	memset(&otm8009a_tx_buf,0,sizeof(struct dsi_buf));
	return ret;
}

static int mipi_otm8009a_lcd_init(void)
{
	led_trigger_register_simple("bkl_trigger", &bkl_led_trigger);
	pr_info("%s: SUCCESS (WLED TRIGGER)\n", __func__);
#ifdef CONFIG_FB_PM8038_CABC_PIN
	wled_trigger_initialized = 0;
#else
	wled_trigger_initialized = 1;
#endif
	mipi_dsi_buf_alloc(&otm8009a_tx_buf, DSI_BUF_SIZE);
	return platform_driver_register(&this_driver);
}

static int __init mipi_cmd_otm8009a_wvga_init(void)
{
	int ret;
	if(0 == lcd_detect_panel(LCD_PANEL_CHIMEI))
	{
	    #ifdef CONFIG_FB_DYNAMIC_GAMMA
		otm8900a_gamma_25 = otm8009a_gamma_25;
		gamma_25_len     = ARRAY_SIZE(otm8009a_gamma_25);
		otm8900a_gamma_22 = otm8009a_gamma_22;
		gamma_22_len     = ARRAY_SIZE(otm8009a_gamma_22);
		#endif
	}
	else
	{
		return 0;
	}
	pr_info("enter mipi_cmd_otm8009a_wvga_init \n");

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

	ret = mipi_otm8009a_device_register(&pinfo, MIPI_DSI_PRIM,MIPI_DSI_PANEL_WVGA);
	if (ret)
		pr_err("%s: failed to register device!\n", __func__);

	return ret;
}

module_init(mipi_cmd_otm8009a_wvga_init);
