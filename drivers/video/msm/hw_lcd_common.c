/* Copyright (c) 2009, Code HUAWEI. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora Forum nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * Alternatively, provided that this notice is retained in full, this software
 * may be relicensed by the recipient under the terms of the GNU General Public
 * License version 2 ("GPL") and only version 2, in which case the provisions of
 * the GPL apply INSTEAD OF those given above.  If the recipient relicenses the
 * software under the GPL, then the identification text in the MODULE_LICENSE
 * macro must be changed to reflect "GPLv2" instead of "Dual BSD/GPL".  Once a
 * recipient changes the license terms to the GPL, subsequent recipients shall
 * not relicense under alternate licensing terms, including the BSD or dual
 * BSD/GPL terms.  In addition, the following license statement immediately
 * below and between the words START and END shall also then apply when this
 * software is relicensed under the GPL:
 *
 * START
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 and only version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * END
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <mach/msm_iomap.h>
#include <linux/delay.h>
#include <mach/gpio.h>
#include <asm/io.h>
#include "msm_fb.h"
#include "hw_lcd_common.h"
#include <hsad/config_interface.h>
#include <mach/pmic.h>
#include "msm_fb.h"
#include <linux/mfd/pmic8058.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include "mdp.h"

#define GPIO_OUT_31                    31
#define GPIO_OUT_129                   129
#define SPI_CLK_DELAY                  1
#define SPI_CLK_PULSE_INTERVAL         5
#define MIPI_MAX_BUFFER 128
/* DriverIC ID and write reg and data */ 
#define HX8347D_DEVICE_ID 		0x70 //BS0=1
#define WRITE_REGISTER 			0x00
#define WRITE_CONTENT 			0x02
#ifdef CONFIG_FB_MSM_MIPI_DSI
static char mipi_packet_struct[MIPI_MAX_BUFFER];
#endif
typedef enum
{
    GPIO_LOW_VALUE  = 0,
    GPIO_HIGH_VALUE = 1
} gpio_value_type;



//static int lcd_reset_gpio;

#ifdef CONFIG_FB_MSM_MIPI_DSI
#define USE_DSI_CMDLIST 

/*****************************************
  @brief : transfor struct sequence to struct mipi packet,  
  @param reg:register and param, value: reg type.
  @return none
******************************************/
void mipi_lcd_register_write(struct msm_fb_data_type *mfd,struct dsi_buf *tp,
                                 uint32 reg,uint32 value,uint32 time)
{
	static boolean packet_ok = FALSE; 
	static uint32 param_num = 0;
	static uint32 last_datatype = 0;
	static uint32 last_time = 0;
	uint32 datatype = 0;
	
	struct dsi_cmd_desc dsi_cmd;
	#ifdef USE_DSI_CMDLIST
	struct dcs_cmd_req cmdreq;
	#endif
	if (( (MIPI_DCS_COMMAND == last_datatype) || (MIPI_GEN_COMMAND == last_datatype) )
		&&( TYPE_PARAMETER != value ))
	{
		packet_ok = TRUE;
	}
	else
	{
		packet_ok = FALSE;
	}
	
	if(packet_ok)
	{
		switch (param_num)
   		{
    		case 1:
				if (MIPI_DCS_COMMAND == last_datatype)
				{
					/*DCS MODE*/					
					datatype = DTYPE_DCS_WRITE;
				}
				else if (MIPI_GEN_COMMAND == last_datatype)
				{
					/*GCS MODE*/					
					datatype = DTYPE_GEN_WRITE1;					
				}								
				
				break;
			case 2:		
				if (MIPI_DCS_COMMAND == last_datatype)
				{
					/*DCS MODE*/
					datatype = DTYPE_DCS_WRITE1;
				}
				else if (MIPI_GEN_COMMAND == last_datatype)
				{
					/*GCS MODE*/					
					datatype = DTYPE_GEN_WRITE2;					
				}

				break;
			default:	
				if (MIPI_DCS_COMMAND == last_datatype)
				{
					/*DCS MODE*/
					datatype = DTYPE_DCS_LWRITE;
				}
				else if (MIPI_GEN_COMMAND == last_datatype)
				{
					/*GCS MODE*/					
					datatype = DTYPE_GEN_LWRITE;					
				}
				
				break;
		}
	
		dsi_cmd.dtype = datatype;
		dsi_cmd.last = 1;
		dsi_cmd.vc = 0;
		dsi_cmd.ack = 0;
		dsi_cmd.wait = last_time;
		dsi_cmd.dlen = param_num;
		dsi_cmd.payload = mipi_packet_struct;
		/*use qcom cmdlist interface,it have mipi sync itself*/
		#ifdef USE_DSI_CMDLIST
		cmdreq.cmds = &dsi_cmd;
		cmdreq.cmds_cnt = 1;
		/*Qual Baseline Update,avoid backlight problem*/
		cmdreq.flags = CMD_REQ_COMMIT | CMD_CLK_CTRL;
		cmdreq.rlen = 0;
		cmdreq.cb = NULL;
		mipi_dsi_cmdlist_put(&cmdreq);
		#else
		mipi_dsi_cmds_tx(tp, &dsi_cmd,1);
		#endif
		packet_ok = FALSE;
		param_num = 0;
		last_datatype = 0;
	}  
	
    switch (value)
    {
    	case MIPI_DCS_COMMAND:
		case MIPI_GEN_COMMAND:				
			last_datatype = value;	
			last_time = time;
			mipi_packet_struct[param_num] = reg;
			param_num ++;
			break;
		case TYPE_PARAMETER:
			mipi_packet_struct[param_num] = reg;
			param_num ++;
			break;
		case MIPI_TYPE_END:
			packet_ok = FALSE;
			param_num = 0;
			last_datatype = 0;
			break;
		default :
			break;

    }
	
}

/*****************************************
  @brief   process mipi sequence table
  @param table: lcd init code, count: sizeof(table), lcd_panel: lcd type
			    mfd:mipi need ,tp: process mipi buffer.
  @return none
******************************************/
void process_mipi_table(struct msm_fb_data_type *mfd,struct dsi_buf *tp,
					const struct sequence *table, size_t count, lcd_panel_type lcd_panel)
{
	unsigned int i = 0;
	uint32 reg = 0;
	uint32 value = 0;
	uint32 time = 0;

	for (i = 0; i < count; i++)
	{
	    reg = table[i].reg;
        value = table[i].value;
        time = table[i].time;
		mipi_lcd_register_write(mfd,tp,reg,value,0);
		if (time != 0)
        {
            LCD_MDELAY(time);
        }
	}
			
}
#endif
/*delete is_panel_support_dynamic_gamma(),is_panel_support_auto_cabc function(),
judge from panel driver setting*/

void lcd_reset(void)
{
	int ret;
	gpio_request(DISP_RST_GPIO, "disp_rst_n");
	ret = gpio_tlmm_config(GPIO_CFG(DISP_RST_GPIO, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if(ret)
	{
		printk("DISP_RST_GPIO config error!");
		gpio_free(DISP_RST_GPIO);
		return;
	}
	ret = gpio_direction_output(DISP_RST_GPIO,1);
	usleep(10);
	ret = gpio_direction_output(DISP_RST_GPIO,0);
	msleep(100);
	ret = gpio_direction_output(DISP_RST_GPIO,1);
	if(ret)
	{
		printk("DISP_RST_GPIO set error!");
		gpio_free(DISP_RST_GPIO);
		return;
	}
}
