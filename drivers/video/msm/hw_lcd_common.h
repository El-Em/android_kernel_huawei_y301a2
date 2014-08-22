/* Copyright (c), Code HUAWEI. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef HW_LCD_COMMON_H
#define HW_LCD_COMMON_H
#include "msm_fb.h"
#include <hsad/config_interface.h>
#include "lcd_hw_debug.h"
#include "mipi_dsi.h"


/* Move from the every LCD file ,those are common */
#define TRACE_LCD_DEBUG 1
#if TRACE_LCD_DEBUG
#define LCD_DEBUG(x...) printk(KERN_ERR "[LCD_DEBUG] " x)
#else
#define LCD_DEBUG(x...) do {} while (0)
#endif
/* LCD_MDELAY will select mdelay or msleep according value */
#define LCD_MDELAY(time_ms)   	\
	do							\
	{ 							\
		if (time_ms>10)			\
			msleep(time_ms);	\
		else					\
			mdelay(time_ms);	\
	}while(0)	
#define TYPE_COMMAND	         (1<<0)
#define TYPE_PARAMETER           (1<<1)
#define START_BYTE_COMMAND 		0x00
#define START_BYTE_PARAMETER	0x01
#define MDDI_MULTI_WRITE_END    0xFFFFFFFF
/* MIPI_DCS_COMMAND : == TYPE_COMMAND MIPI DCS COMMAND
 * MIPI_GEN_COMMAND : 4 MIPI GCS COMMAND
 * MIPI_TYPE_END: 0XFF 
 */
#define MIPI_DCS_COMMAND (1<<0)
#define MIPI_GEN_COMMAND 4
#define MIPI_TYPE_END 0XFF
/* MDDI output bpp type */

#define DISP_RST_GPIO 58

struct lcd_state_type
{
	boolean disp_initialized;
	boolean display_on;
	boolean disp_powered_up;
};

/*delete is_panel_support_dynamic_gamma(),is_panel_support_auto_cabc function(),
judge from panel driver setting*/

#ifdef CONFIG_FB_MSM_MIPI_DSI
void process_mipi_table(struct msm_fb_data_type *mfd,struct dsi_buf *tp, 
					const struct sequence *table, size_t count, lcd_panel_type lcd_panel);
#endif

void lcd_reset(void);

#endif
