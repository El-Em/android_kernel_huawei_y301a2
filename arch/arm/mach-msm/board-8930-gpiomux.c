/* Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
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

#include <linux/gpio.h>
#include <asm/mach-types.h>
#include <mach/gpiomux.h>
#include <mach/socinfo.h>
#include "devices.h"
#include "board-8930.h"
#ifdef CONFIG_HUAWEI_GPIO_UNITE
struct msm_gpiomux_config msm8930_gpio_configs[NR_GPIO_IRQS];
#endif

/* The SPI configurations apply to GSBI 1*/
static struct gpiomux_setting spi_active = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_12MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting spi_suspended_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting gsbi3_suspended_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_KEEPER,
};

static struct gpiomux_setting gsbi3_active_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting gsbi5 = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting gsbi9 = {
	.func = GPIOMUX_FUNC_2,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting gsbi10 = {
	.func = GPIOMUX_FUNC_2,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting gsbi12 = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting cdc_mclk = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting audio_auxpcm[] = {
	/* Suspended state */
	{
		.func = GPIOMUX_FUNC_GPIO,
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},
	/* Active state */
	{
		.func = GPIOMUX_FUNC_1,
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},
};

static struct gpiomux_setting audio_mbhc = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting audio_spkr_boost = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

#if defined(CONFIG_KS8851) || defined(CONFIG_KS8851_MODULE)
static struct gpiomux_setting gpio_eth_suspend_1_cfg = {
	.pull = GPIOMUX_PULL_DOWN,
	.drv = GPIOMUX_DRV_2MA,
	.func = GPIOMUX_FUNC_GPIO,
};

static struct gpiomux_setting gpio_eth_suspend_2_cfg = {
	.pull = GPIOMUX_PULL_NONE,
	.drv = GPIOMUX_DRV_2MA,
	.func = GPIOMUX_FUNC_GPIO,
};
#endif

static struct gpiomux_setting slimbus = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_KEEPER,
};

static struct gpiomux_setting wcnss_5wire_suspend_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv  = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting wcnss_5wire_active_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv  = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_DOWN,
};

#if 0
#ifdef CONFIG_HUAWEI_NFC_PN544
static struct gpiomux_setting nfc_ven_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_HIGH,
};

static struct gpiomux_setting nfc_int_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
	.dir = GPIOMUX_IN,
};

static struct gpiomux_setting nfc_dlnd_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_LOW,
};

#if 0
static struct gpiomux_setting nfc_ven_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
	.dir = GPIOMUX_OUT_HIGH,
};

static struct gpiomux_setting nfc_int_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN,
};

static struct gpiomux_setting nfc_dlnd_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_LOW,
};
#endif
#endif
#endif

static struct gpiomux_setting atmel_resout_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting atmel_resout_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting atmel_ldo_en_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting atmel_ldo_en_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting atmel_int_act_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting atmel_int_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};
#ifdef MSM8930_PHASE_2
static struct gpiomux_setting hsusb_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_LOW,
};
static struct msm_gpiomux_config msm8930_hsusb_configs[] = {
/* Avoid the issue BT SCO path cannot be created */
#if 0
	{
		.gpio = 63,     /* HSUSB_EXTERNAL_5V_LDO_EN */
		.settings = {
			[GPIOMUX_SUSPENDED] = &hsusb_sus_cfg,
		},
	},
#endif
	{
		.gpio = 97,     /* HSUSB_5V_EN */
		.settings = {
			[GPIOMUX_SUSPENDED] = &hsusb_sus_cfg,
		},
	},
};
#endif

/* Merge MI2S patch */
#if 0
static struct gpiomux_setting hap_lvl_shft_suspended_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting hap_lvl_shft_active_config = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
};
#endif

static struct gpiomux_setting ap2mdm_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting mdm2ap_status_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting mdm2ap_errfatal_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_16MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting ap2mdm_kpdpwr_n_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting mdp_vsync_suspend_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting mdp_vsync_active_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
static struct gpiomux_setting hdmi_suspend_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting hdmi_active_1_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting hdmi_active_2_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting hdmi_active_3_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
	.dir = GPIOMUX_IN,
};

static struct gpiomux_setting hdmi_active_4_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
	.dir = GPIOMUX_OUT_HIGH,
};

static struct gpiomux_setting hdmi_active_5_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
	.dir = GPIOMUX_OUT_HIGH,
};

#endif

static struct gpiomux_setting sitar_reset = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_6MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_OUT_LOW,
};

#if defined(CONFIG_KS8851) || defined(CONFIG_KS8851_MODULE)
static struct msm_gpiomux_config msm8960_ethernet_configs[] = {
	{
		.gpio = 89,
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_eth_suspend_1_cfg,
		}
	},
	{
		.gpio = 90,
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_eth_suspend_2_cfg,
		}
	},
};
#endif

static struct msm_gpiomux_config msm8960_gsbi_configs[] __initdata = {
	{
		.gpio      = 6,		/* GSBI1 QUP SPI_DATA_MOSI */
		.settings = {
			[GPIOMUX_SUSPENDED] = &spi_suspended_config,
			[GPIOMUX_ACTIVE] = &spi_active,
		},
	},
	{
		.gpio      = 7,		/* GSBI1 QUP SPI_DATA_MISO */
		.settings = {
			[GPIOMUX_SUSPENDED] = &spi_suspended_config,
			[GPIOMUX_ACTIVE] = &spi_active,
		},
	},
	{
		.gpio      = 8,		/* GSBI1 QUP SPI_CS_N */
		.settings = {
			[GPIOMUX_SUSPENDED] = &spi_suspended_config,
			[GPIOMUX_ACTIVE] = &spi_active,
		},
	},
	{
		.gpio      = 9,		/* GSBI1 QUP SPI_CLK */
		.settings = {
			[GPIOMUX_SUSPENDED] = &spi_suspended_config,
			[GPIOMUX_ACTIVE] = &spi_active,
		},
	},
	{
		.gpio      = 16,	/* GSBI3 I2C QUP SDA */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi3_suspended_cfg,
			[GPIOMUX_ACTIVE] = &gsbi3_active_cfg,
		},
	},
	{
		.gpio      = 17,	/* GSBI3 I2C QUP SCL */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi3_suspended_cfg,
			[GPIOMUX_ACTIVE] = &gsbi3_active_cfg,
		},
	},
	{
		.gpio      = 22,	/* GSBI5 UART2 */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi5,
		},
	},
	{
		.gpio      = 23,	/* GSBI5 UART2 */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi5,
		},
	},
	{
		.gpio      = 44,	/* GSBI12 I2C QUP SDA */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi12,
		},
	},
	{
		.gpio      = 95,	/* GSBI9 I2C QUP SDA */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi9,
		},
	},
	{
		.gpio      = 96,	/* GSBI12 I2C QUP SCL */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi9,
		},
	},
	{
		.gpio      = 45,	/* GSBI12 I2C QUP SCL */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi12,
		},
	},
	{
		.gpio      = 73,	/* GSBI10 I2C QUP SDA */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi10,
		},
	},
	{
		.gpio      = 74,	/* GSBI10 I2C QUP SCL */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gsbi10,
		},
	},
};

static struct msm_gpiomux_config msm8960_slimbus_config[] __initdata = {
	{
		.gpio	= 60,		/* slimbus data */
		.settings = {
			[GPIOMUX_SUSPENDED] = &slimbus,
		},
	},
	{
		.gpio	= 61,		/* slimbus clk */
		.settings = {
			[GPIOMUX_SUSPENDED] = &slimbus,
		},
	},
};

static struct msm_gpiomux_config msm8960_audio_codec_configs[] __initdata = {
	{
		.gpio = 59,
		.settings = {
			[GPIOMUX_SUSPENDED] = &cdc_mclk,
		},
	},
};

static struct msm_gpiomux_config msm8960_audio_mbhc_configs[] __initdata = {
	{
		.gpio = 37,
		.settings = {
			[GPIOMUX_SUSPENDED] = &audio_mbhc,
		},
	},
};

static struct msm_gpiomux_config msm8960_audio_spkr_configs[] __initdata = {
	{
		.gpio = 15,
		.settings = {
			[GPIOMUX_SUSPENDED] = &audio_spkr_boost,
		},
	},
};


static struct msm_gpiomux_config msm8960_audio_auxpcm_configs[] __initdata = {
	{
		.gpio = 63,
		.settings = {
			[GPIOMUX_SUSPENDED] = &audio_auxpcm[0],
			[GPIOMUX_ACTIVE] = &audio_auxpcm[1],
		},
	},
	{
		.gpio = 64,
		.settings = {
			[GPIOMUX_SUSPENDED] = &audio_auxpcm[0],
			[GPIOMUX_ACTIVE] = &audio_auxpcm[1],
		},
	},
	{
		.gpio = 65,
		.settings = {
			[GPIOMUX_SUSPENDED] = &audio_auxpcm[0],
			[GPIOMUX_ACTIVE] = &audio_auxpcm[1],
		},
	},
	{
		.gpio = 66,
		.settings = {
			[GPIOMUX_SUSPENDED] = &audio_auxpcm[0],
			[GPIOMUX_ACTIVE] = &audio_auxpcm[1],
		},
	},
};

/* Merge MI2S patch */
static struct gpiomux_setting  mi2s_act_cfg = {
	.func = GPIOMUX_FUNC_1,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct gpiomux_setting  mi2s_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct msm_gpiomux_config msm8930_mi2s_configs[] __initdata = {
	{
		.gpio	= 47,		/* mi2s ws */
		.settings = {
			[GPIOMUX_SUSPENDED] = &mi2s_sus_cfg,
			[GPIOMUX_ACTIVE] = &mi2s_act_cfg,
		},
	},
	{
		.gpio	= 48,		/* mi2s sclk */
		.settings = {
			[GPIOMUX_SUSPENDED] = &mi2s_sus_cfg,
			[GPIOMUX_ACTIVE] = &mi2s_act_cfg,
		},
	},
	{
		.gpio	= 49,		/* mi2s dout3 */
		.settings = {
			[GPIOMUX_SUSPENDED] = &mi2s_sus_cfg,
			[GPIOMUX_ACTIVE] = &mi2s_act_cfg,
		},
	},
#if 0
	{
		.gpio	= 50,		/* mi2s dout2 */
		.settings = {
			[GPIOMUX_SUSPENDED] = &mi2s_sus_cfg,
			[GPIOMUX_ACTIVE] = &mi2s_act_cfg,
		},
	},
	{
		.gpio	= 51,		/* mi2s dout1 */
		.settings = {
			[GPIOMUX_SUSPENDED] = &mi2s_sus_cfg,
			[GPIOMUX_ACTIVE] = &mi2s_act_cfg,
		},
	},
	{
		.gpio	= 52,		/* mi2s dout0 */
		.settings = {
			[GPIOMUX_SUSPENDED] = &mi2s_sus_cfg,
			[GPIOMUX_ACTIVE] = &mi2s_act_cfg,
		},
	},

	{
		.gpio	= 53,		/* mi2s mclk */
		.settings = {
			[GPIOMUX_SUSPENDED] = &mi2s_sus_cfg,
			[GPIOMUX_ACTIVE] = &mi2s_act_cfg,
		},
	},
#endif
};

static struct msm_gpiomux_config wcnss_5wire_interface[] = {
	{
		.gpio = 84,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
	{
		.gpio = 85,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
	{
		.gpio = 86,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
	{
		.gpio = 87,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
	{
		.gpio = 88,
		.settings = {
			[GPIOMUX_ACTIVE]    = &wcnss_5wire_active_cfg,
			[GPIOMUX_SUSPENDED] = &wcnss_5wire_suspend_cfg,
		},
	},
};

/*In order to save powert,make the GPIO except clk be PULL_UP state when suspend state*/
#ifdef CONFIG_HUAWEI_KERNEL
static struct gpiomux_setting sdc4_suspend_cfg = {
	.func = GPIOMUX_FUNC_2,
	.drv  = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct gpiomux_setting sdc4_clk_suspend_cfg = {
	.func = GPIOMUX_FUNC_2,
	.drv  = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct gpiomux_setting sdc4_active_cfg = {
	.func = GPIOMUX_FUNC_2,
	.drv  = GPIOMUX_DRV_16MA,
	.pull = GPIOMUX_PULL_UP,
};

static struct msm_gpiomux_config sdc4_interface[] = {
	{
		.gpio = 83,
		.settings = {
			[GPIOMUX_ACTIVE]    = &sdc4_active_cfg,
			[GPIOMUX_SUSPENDED] = &sdc4_suspend_cfg,
		},
	},
	{
		.gpio = 84,
		.settings = {
			[GPIOMUX_ACTIVE]    = &sdc4_active_cfg,
			[GPIOMUX_SUSPENDED] = &sdc4_suspend_cfg,
		},
	},
	{
		.gpio = 85,
		.settings = {
			[GPIOMUX_ACTIVE]    = &sdc4_active_cfg,
			[GPIOMUX_SUSPENDED] = &sdc4_suspend_cfg,
		},
	},
	{
		.gpio = 86,
		.settings = {
			[GPIOMUX_ACTIVE]    = &sdc4_active_cfg,
			[GPIOMUX_SUSPENDED] = &sdc4_suspend_cfg,
		},
	},
	{
		.gpio = 87,
		.settings = {
			[GPIOMUX_ACTIVE]    = &sdc4_active_cfg,
			[GPIOMUX_SUSPENDED] = &sdc4_suspend_cfg,
		},
	},
	{
		.gpio = 88,
		.settings = {
			[GPIOMUX_ACTIVE]    = &sdc4_active_cfg,
			[GPIOMUX_SUSPENDED] = &sdc4_clk_suspend_cfg,
		},
	},
};
#endif

#if 0
#ifdef CONFIG_HUAWEI_NFC_PN544
static struct msm_gpiomux_config msm8930_nfc_configs[] __initdata = {
	{	/* NFC VEN */
		.gpio = 25,
		.settings = {
//			[GPIOMUX_ACTIVE]    = &nfc_ven_act_cfg,
			[GPIOMUX_SUSPENDED] = &nfc_ven_sus_cfg,
		},
	},
	{	/* NFC INTERRUPT */
		.gpio = 106,
		.settings = {
//			[GPIOMUX_ACTIVE]    = &nfc_int_act_cfg,
			[GPIOMUX_SUSPENDED] = &nfc_int_sus_cfg,
		},
	},
	{	/* NFC DOWNLOAD */
		.gpio = 24,
		.settings = {
//			[GPIOMUX_ACTIVE]    = &nfc_dlnd_act_cfg,
			[GPIOMUX_SUSPENDED] = &nfc_dlnd_sus_cfg,
		},
	},
};
#endif
#endif

static struct msm_gpiomux_config msm8960_atmel_configs[] __initdata = {
	{	/* TS INTERRUPT */
		.gpio = 11,
		.settings = {
			[GPIOMUX_ACTIVE]    = &atmel_int_act_cfg,
			[GPIOMUX_SUSPENDED] = &atmel_int_sus_cfg,
		},
	},
	{	/* TS LDO ENABLE */
		.gpio = 50,
		.settings = {
			[GPIOMUX_ACTIVE]    = &atmel_ldo_en_act_cfg,
			[GPIOMUX_SUSPENDED] = &atmel_ldo_en_sus_cfg,
		},
	},
	{	/* TS RESOUT */
		.gpio = 52,
		.settings = {
			[GPIOMUX_ACTIVE]    = &atmel_resout_act_cfg,
			[GPIOMUX_SUSPENDED] = &atmel_resout_sus_cfg,
		},
	},
};

/* Merge MI2S patch */
#if 0
static struct msm_gpiomux_config hap_lvl_shft_config[] __initdata = {
	{
		.gpio = 47,
		.settings = {
			[GPIOMUX_SUSPENDED] = &hap_lvl_shft_suspended_config,
			[GPIOMUX_ACTIVE] = &hap_lvl_shft_active_config,
		},
	},
};
#endif

static struct msm_gpiomux_config mdm_configs[] __initdata = {
	/* AP2MDM_STATUS */
	{
		.gpio = 94,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ap2mdm_cfg,
		}
	},
	/* MDM2AP_STATUS */
	{
		.gpio = 69,
		.settings = {
			[GPIOMUX_SUSPENDED] = &mdm2ap_status_cfg,
		}
	},
	/* MDM2AP_ERRFATAL */
	{
		.gpio = 70,
		.settings = {
			[GPIOMUX_SUSPENDED] = &mdm2ap_errfatal_cfg,
		}
	},
	/* AP2MDM_ERRFATAL */
	{
		.gpio = 95,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ap2mdm_cfg,
		}
	},
	/* AP2MDM_KPDPWR_N */
	{
		.gpio = 81,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ap2mdm_kpdpwr_n_cfg,
		}
	},
	/* AP2MDM_PMIC_RESET_N */
	{
		.gpio = 80,
		.settings = {
			[GPIOMUX_SUSPENDED] = &ap2mdm_kpdpwr_n_cfg,
		}
	}
};

static struct msm_gpiomux_config msm8960_mdp_vsync_configs[] __initdata = {
	{
		.gpio = 0,
		.settings = {
			[GPIOMUX_ACTIVE]    = &mdp_vsync_active_cfg,
			[GPIOMUX_SUSPENDED] = &mdp_vsync_suspend_cfg,
		},
	}
};

#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
static struct msm_gpiomux_config msm8960_hdmi_configs[] __initdata = {
	{
		.gpio = 99,
		.settings = {
			[GPIOMUX_ACTIVE]    = &hdmi_active_1_cfg,
			[GPIOMUX_SUSPENDED] = &hdmi_suspend_cfg,
		},
	},
	{
		.gpio = 100,
		.settings = {
			[GPIOMUX_ACTIVE]    = &hdmi_active_1_cfg,
			[GPIOMUX_SUSPENDED] = &hdmi_suspend_cfg,
		},
	},
	{
		.gpio = 101,
		.settings = {
			[GPIOMUX_ACTIVE]    = &hdmi_active_1_cfg,
			[GPIOMUX_SUSPENDED] = &hdmi_suspend_cfg,
		},
	},
	{
		.gpio = 102,
		.settings = {
			[GPIOMUX_ACTIVE]    = &hdmi_active_2_cfg,
			[GPIOMUX_SUSPENDED] = &hdmi_suspend_cfg,
		},
	},

};

static struct msm_gpiomux_config msm8930_mhl_configs[] __initdata = {
	{
		.gpio = 72,
		.settings = {
			[GPIOMUX_ACTIVE]    = &hdmi_active_3_cfg,
			[GPIOMUX_SUSPENDED] = &hdmi_suspend_cfg,
		},
	},
	{
		.gpio = 71,
		.settings = {
			[GPIOMUX_ACTIVE]    = &hdmi_active_4_cfg,
			[GPIOMUX_SUSPENDED] = &hdmi_suspend_cfg,
		},
	},
	{
		.gpio = 73,
		.settings = {
			[GPIOMUX_ACTIVE]    = &hdmi_active_5_cfg,
			[GPIOMUX_SUSPENDED] = &hdmi_suspend_cfg,
		},
	},

};
#endif

static struct gpiomux_setting haptics_active_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
};
static struct gpiomux_setting haptics_suspend_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_DOWN,
};

static struct msm_gpiomux_config msm8930_haptics_configs[] __initdata = {
	{
		.gpio = 77,
		.settings = {
			[GPIOMUX_ACTIVE] = &haptics_active_cfg,
			[GPIOMUX_SUSPENDED] = &haptics_suspend_cfg,
		},
	},
	{
		.gpio = 78,
		.settings = {
			[GPIOMUX_ACTIVE] = &haptics_active_cfg,
			[GPIOMUX_SUSPENDED] = &haptics_suspend_cfg,
		},
	},
};

static struct gpiomux_setting sd_det_line = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct msm_gpiomux_config msm8930_sd_det_config[] __initdata = {
	{
		.gpio = 94,	/* SD Card Detect Line */
		.settings = {
			[GPIOMUX_SUSPENDED] = &sd_det_line,
			[GPIOMUX_ACTIVE] = &sd_det_line,
		},
	},
};

#ifdef CONFIG_HUAWEI_KERNEL
static struct gpiomux_setting gpio_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
	.dir = GPIOMUX_IN,
};

//nfc_wake gpio suspend configure
static struct gpiomux_setting gpio_sus_cfg_nfc = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_DOWN,
	.dir = GPIOMUX_OUT_HIGH,
};

static struct gpiomux_setting gpio_int_sus_cfg = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN,
};

static struct msm_gpiomux_config msm8930_gpio_sus_configs[] = {
	{
		.gpio = 11,   
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_int_sus_cfg,
			[GPIOMUX_ACTIVE] = &gpio_int_sus_cfg,
	
		},
	},
	{
		.gpio = 24,   
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_sus_cfg_nfc,
			[GPIOMUX_ACTIVE] = &gpio_sus_cfg_nfc,
		},
	},
	{
		.gpio = 50,   
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_sus_cfg,
			[GPIOMUX_ACTIVE] = &gpio_sus_cfg,
		},
	},
	{
		.gpio = 51,   
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_int_sus_cfg,
			[GPIOMUX_ACTIVE] = &gpio_int_sus_cfg,
		},
	},
/* Avoid the issue BT SCO path cannot be created */
#if 0
	{
		.gpio = 63,   
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_sus_cfg,
			[GPIOMUX_ACTIVE] = &gpio_sus_cfg,
		},
	},
	{
		.gpio = 64,   
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_sus_cfg,
			[GPIOMUX_ACTIVE] = &gpio_sus_cfg,
		},
	},
	{
		.gpio = 65,   
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_sus_cfg,
			[GPIOMUX_ACTIVE] = &gpio_sus_cfg,
		},
	},
	{
		.gpio = 66,   
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_sus_cfg,
			[GPIOMUX_ACTIVE] = &gpio_sus_cfg,
		},
	},
#endif
	/*Add for touch power on*/
	{
		.gpio = 67,   
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_sus_cfg,
			[GPIOMUX_ACTIVE] = &gpio_sus_cfg,
		},
	},
	{
		.gpio = 73,   
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_sus_cfg,
			[GPIOMUX_ACTIVE] = &gpio_sus_cfg,
		},
	},
	{
		.gpio = 77,   
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_sus_cfg,
			[GPIOMUX_ACTIVE] = &gpio_sus_cfg,
		},
	},
	{
		.gpio = 78,   
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_sus_cfg,
			[GPIOMUX_ACTIVE] = &gpio_sus_cfg,
		},
	},
	{
		.gpio = 90,   
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_sus_cfg,
			[GPIOMUX_ACTIVE] = &gpio_sus_cfg,
		},
	},
	{
		.gpio = 106,   
		.settings = {
			[GPIOMUX_SUSPENDED] = &gpio_sus_cfg,
			[GPIOMUX_ACTIVE] = &gpio_sus_cfg,
		},
	},
};
#endif

#ifdef CONFIG_HUAWEI_GPIO_UNITE
int hw_gpio_init(void)
{
    int i;
    int config_index = 0;
    const struct gpio_config_type* pGpioCfgTbl = NULL;

    pGpioCfgTbl = get_gpio_config_table();

    if(NULL == pGpioCfgTbl)
    {
       pr_err(KERN_ERR "get gpio config table failed\n"); 
       return -EFAULT;
    }

    for(i = 0; i < NR_GPIO_IRQS; i++)
    {         
        /* get the gpio active config and suspend config from the config excel */
        if((pGpioCfgTbl + i)->init_func != NOSET   \
            || (pGpioCfgTbl + i)->sleep_func != NOSET)
        {
            msm8930_gpio_configs[config_index].gpio = (pGpioCfgTbl + i)->gpio_number;
    
            if((pGpioCfgTbl + i)->init_func != NOSET)
            {         
                msm8930_gpio_configs[config_index].settings[GPIOMUX_ACTIVE] = \
                    kmalloc(sizeof(*msm8930_gpio_configs[config_index].settings[GPIOMUX_ACTIVE]),\
                            GFP_KERNEL);

		   memset(msm8930_gpio_configs[config_index].settings[GPIOMUX_ACTIVE],
				0,sizeof(*msm8930_gpio_configs[config_index].settings[GPIOMUX_ACTIVE]));

                if (!(msm8930_gpio_configs[config_index].settings[GPIOMUX_ACTIVE]))
                {
                    pr_err(KERN_ERR "failed to malloc memory for gpio config.\n");
                    goto gpio_configs_free;
                }                                   

                msm8930_gpio_configs[config_index].settings[GPIOMUX_ACTIVE]->func = \
                    (pGpioCfgTbl + i)->init_func;

                if((pGpioCfgTbl + i)->init_dir!= NOSET)
                {
                    msm8930_gpio_configs[config_index].settings[GPIOMUX_ACTIVE]->dir = \
                        (pGpioCfgTbl + i)->init_dir;
                }

                if((pGpioCfgTbl + i)->init_drv != NOSET)
                {
                    msm8930_gpio_configs[config_index].settings[GPIOMUX_ACTIVE]->drv = \
                        (pGpioCfgTbl + i)->init_drv;
                }

                if((pGpioCfgTbl + i)->init_pull != NOSET)
                {
                    msm8930_gpio_configs[config_index].settings[GPIOMUX_ACTIVE]->pull = \
                        (pGpioCfgTbl + i)->init_pull;
                }
            }

            if((pGpioCfgTbl + i)->sleep_func != NOSET)
            { 
                msm8930_gpio_configs[config_index].settings[GPIOMUX_SUSPENDED] = \
                    kmalloc(sizeof(*msm8930_gpio_configs[config_index].settings[GPIOMUX_SUSPENDED]),\
                            GFP_KERNEL);
		   memset(msm8930_gpio_configs[config_index].settings[GPIOMUX_SUSPENDED],
				0,sizeof(*msm8930_gpio_configs[config_index].settings[GPIOMUX_SUSPENDED]));

                if (!msm8930_gpio_configs[config_index].settings[GPIOMUX_SUSPENDED])
                {
                    pr_err(KERN_ERR "failed to malloc memory for gpio config.\n");
                    goto gpio_configs_free;
                }

                msm8930_gpio_configs[config_index].settings[GPIOMUX_SUSPENDED]->func = \
                    (pGpioCfgTbl + i)->sleep_func;

                if((pGpioCfgTbl + i)->sleep_dir != NOSET)
                {
                    msm8930_gpio_configs[config_index].settings[GPIOMUX_SUSPENDED]->dir = \
                        (pGpioCfgTbl + i)->sleep_dir;
                }

                if((pGpioCfgTbl + i)->sleep_drv != NOSET)
                {
                    msm8930_gpio_configs[config_index].settings[GPIOMUX_SUSPENDED]->drv = \
                        (pGpioCfgTbl + i)->sleep_drv;
                }

                if((pGpioCfgTbl + i)->sleep_pull != NOSET)
                {
                    msm8930_gpio_configs[config_index].settings[GPIOMUX_SUSPENDED]->pull = \
                        (pGpioCfgTbl + i)->sleep_pull;
                }
            }

            config_index++;
        }    
    }

    /* gpio config for active status and suspend status. */
    msm_gpiomux_install(msm8930_gpio_configs,
			ARRAY_SIZE(msm8930_gpio_configs));

gpio_configs_free:
    /* free memory */
    for(i = 0; i < NR_GPIO_IRQS; i++)
    {
        if(msm8930_gpio_configs[i].settings[GPIOMUX_ACTIVE] != NULL)
        {
           kfree(msm8930_gpio_configs[i].settings[GPIOMUX_ACTIVE]); 
        }

        if(msm8930_gpio_configs[i].settings[GPIOMUX_SUSPENDED] != NULL)
        {
           kfree(msm8930_gpio_configs[i].settings[GPIOMUX_SUSPENDED]); 
        } 
    }

    return 0;
}
#endif

static struct gpiomux_setting gyro_int_line = {
	.func = GPIOMUX_FUNC_GPIO,
	.drv = GPIOMUX_DRV_2MA,
	.pull = GPIOMUX_PULL_NONE,
};

static struct msm_gpiomux_config msm8930_gyro_int_config[] __initdata = {
	{
		.gpio = 69,	/* Gyro Interrupt Line */
		.settings = {
			[GPIOMUX_SUSPENDED] = &gyro_int_line,
			[GPIOMUX_ACTIVE] = &gyro_int_line,
		},
	},
};

static struct msm_gpiomux_config msm_sitar_config[] __initdata = {
	{
		.gpio   = 42,           /* SYS_RST_N */
		.settings = {
			[GPIOMUX_SUSPENDED] = &sitar_reset,
		},
	}
};

int __init msm8930_init_gpiomux(void)
{
	int rc = msm_gpiomux_init(NR_GPIO_IRQS);
	hw_wifi_device_type  wifi_device_type = WIFI_TYPE_UNKNOWN;
	if (rc) {
		pr_err(KERN_ERR "msm_gpiomux_init failed %d\n", rc);
		return rc;
	}

#if defined(CONFIG_KS8851) || defined(CONFIG_KS8851_MODULE)
	msm_gpiomux_install(msm8960_ethernet_configs,
			ARRAY_SIZE(msm8960_ethernet_configs));
#endif

	msm_gpiomux_install(msm8960_gsbi_configs,
			ARRAY_SIZE(msm8960_gsbi_configs));

	/* Merge MI2S patch */
	msm_gpiomux_install(msm8930_mi2s_configs,
			ARRAY_SIZE(msm8930_mi2s_configs));

	msm_gpiomux_install(msm8960_atmel_configs,
			ARRAY_SIZE(msm8960_atmel_configs));

	msm_gpiomux_install(msm8960_slimbus_config,
			ARRAY_SIZE(msm8960_slimbus_config));

	msm_gpiomux_install(msm8960_audio_codec_configs,
			ARRAY_SIZE(msm8960_audio_codec_configs));

	msm_gpiomux_install(msm8960_audio_mbhc_configs,
			ARRAY_SIZE(msm8960_audio_mbhc_configs));

	msm_gpiomux_install(msm8960_audio_spkr_configs,
			ARRAY_SIZE(msm8960_audio_spkr_configs));

	msm_gpiomux_install(msm8960_audio_auxpcm_configs,
			ARRAY_SIZE(msm8960_audio_auxpcm_configs));
#ifndef CONFIG_HUAWEI_KERNEL
	msm_gpiomux_install(wcnss_5wire_interface,
			ARRAY_SIZE(wcnss_5wire_interface));

#else
	/*optimize the code*/
    wifi_device_type = get_hw_wifi_device_type();
    if((WIFI_BROADCOM_4330 == wifi_device_type)||(WIFI_BROADCOM_4330X == wifi_device_type))
    {
        msm_gpiomux_install(sdc4_interface,
                ARRAY_SIZE(sdc4_interface));
    }
    else if(WIFI_QUALCOMM_WCN3660 == wifi_device_type)
    {
	msm_gpiomux_install(wcnss_5wire_interface,
			ARRAY_SIZE(wcnss_5wire_interface));
    }
    else
    {
        printk("unkonw wifi chip type!\n");
    }
#endif

	if (machine_is_msm8930_mtp() || machine_is_msm8930_fluid() ||
		machine_is_msm8930_cdp()) {
/* Merge MI2S patch */
#if 0
		msm_gpiomux_install(hap_lvl_shft_config,
			ARRAY_SIZE(hap_lvl_shft_config));
#endif
#ifdef MSM8930_PHASE_2
		msm_gpiomux_install(msm8930_hsusb_configs,
			ARRAY_SIZE(msm8930_hsusb_configs));
#endif
	}

	if (PLATFORM_IS_CHARM25())
		msm_gpiomux_install(mdm_configs,
			ARRAY_SIZE(mdm_configs));

	if (machine_is_msm8930_cdp() || machine_is_msm8930_mtp()
		|| machine_is_msm8930_fluid())
		msm_gpiomux_install(msm8930_haptics_configs,
			ARRAY_SIZE(msm8930_haptics_configs));

#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
	msm_gpiomux_install(msm8960_hdmi_configs,
			ARRAY_SIZE(msm8960_hdmi_configs));
	if (msm8930_mhl_display_enabled())
		msm_gpiomux_install(msm8930_mhl_configs,
				ARRAY_SIZE(msm8930_mhl_configs));
#endif

	msm_gpiomux_install(msm8960_mdp_vsync_configs,
			ARRAY_SIZE(msm8960_mdp_vsync_configs));

	msm_gpiomux_install(msm8930_sd_det_config,
			ARRAY_SIZE(msm8930_sd_det_config));

	if (machine_is_msm8930_fluid() || machine_is_msm8930_mtp())
		msm_gpiomux_install(msm8930_gyro_int_config,
			ARRAY_SIZE(msm8930_gyro_int_config));

	msm_gpiomux_install(msm_sitar_config, ARRAY_SIZE(msm_sitar_config));

#ifdef CONFIG_HUAWEI_KERNEL
	msm_gpiomux_install(msm8930_gpio_sus_configs, ARRAY_SIZE(msm8930_gpio_sus_configs));
#endif
	return 0;
}
