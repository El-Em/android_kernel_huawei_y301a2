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

#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/bootmem.h>
#include <linux/gpio.h>
#include <asm/mach-types.h>
#include <asm/mach/mmc.h>
#include <mach/msm_bus_board.h>
#include <mach/board.h>
#include <mach/gpiomux.h>
#include <mach/socinfo.h>
#include <linux/wlan_plat.h>
#include "devices.h"

#include "board-8930.h"
#include "board-storage-common-a.h"

#ifdef CONFIG_HUAWEI_KERNEL
#include <hsad/config_interface.h>
#ifdef CONFIG_HUAWEI_WIFI_SDCC
#include <linux/skbuff.h>
#endif
#endif

/* MSM8960 has 5 SDCC controllers */
enum sdcc_controllers {
	SDCC1,
	SDCC2,
	SDCC3,
	SDCC4,
	SDCC5,
	MAX_SDCC_CONTROLLER
};

/* All SDCC controllers require VDD/VCC voltage */
static struct msm_mmc_reg_data mmc_vdd_reg_data[MAX_SDCC_CONTROLLER] = {
	/* SDCC1 : eMMC card connected */
	[SDCC1] = {
		.name = "sdc_vdd",
#ifdef CONFIG_HUAWEI_KERNEL
		/*set vdd level to 2.85V because LDO_5 supply power to LCD*/
		.high_vol_level = 2850000,
		.low_vol_level = 2850000,
#else
		.high_vol_level = 2950000,
		.low_vol_level = 2950000,
#endif
		.always_on = 1,
		.lpm_sup = 1,
		.lpm_uA = 9000,
		.hpm_uA = 200000, /* 200mA */
	},
	/* SDCC3 : External card slot connected */
	[SDCC3] = {
		.name = "sdc_vdd",
		.high_vol_level = 2950000,
		.low_vol_level = 2950000,
		/*
		 * Normally this is not an always ON regulator. On this
		 * platform, unfortunately the sd detect line is connected
		 * to this via esd circuit and so turn this off/on while card
		 * is not present causes the sd detect line to toggle
		 * continuously. This is expected to be fixed in the newer
		 * hardware revisions - maybe once that is done, this can be
		 * reverted.
		 */
		.always_on = 1,
		.lpm_sup = 1,
		.hpm_uA = 800000, /* 800mA */
		.lpm_uA = 9000,
		.reset_at_init = true,
	},
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
                [SDCC4] = {
                        .name = "sdc_vdd",
                        .high_vol_level = 1800000,
                        .low_vol_level = 1800000,
                        .hpm_uA = 600000, /* 600mA */
            },
#endif
};

/* All SDCC controllers may require voting for VDD PAD voltage */
static struct msm_mmc_reg_data mmc_vdd_io_reg_data[MAX_SDCC_CONTROLLER] = {
	/* SDCC1 : eMMC card connected */
	[SDCC1] = {
		.name = "sdc_vdd_io",
		.always_on = 1,
		.high_vol_level = 1800000,
		.low_vol_level = 1800000,
		.hpm_uA = 200000, /* 200mA */
	},
	/* SDCC3 : External card slot connected */
	[SDCC3] = {
		.name = "sdc_vdd_io",
		.high_vol_level = 2950000,
		.low_vol_level = 1850000,
		.always_on = 1,
		.lpm_sup = 1,
		/* Max. Active current required is 16 mA */
		.hpm_uA = 16000,
		/*
		 * Sleep current required is ~300 uA. But min. vote can be
		 * in terms of mA (min. 1 mA). So let's vote for 2 mA
		 * during sleep.
		 */
		.lpm_uA = 2000,
	}
};

static struct msm_mmc_slot_reg_data mmc_slot_vreg_data[MAX_SDCC_CONTROLLER] = {
	/* SDCC1 : eMMC card connected */
	[SDCC1] = {
		.vdd_data = &mmc_vdd_reg_data[SDCC1],
		.vdd_io_data = &mmc_vdd_io_reg_data[SDCC1],
	},
	/* SDCC3 : External card slot connected */
	[SDCC3] = {
		.vdd_data = &mmc_vdd_reg_data[SDCC3],
		.vdd_io_data = &mmc_vdd_io_reg_data[SDCC3],
                    
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
        },
        /* SDCC4 : External card slot connected */
        [SDCC4] = {
            .vdd_data = &mmc_vdd_reg_data[SDCC4],
#endif
	}
};

/* SDC1 pad data */
/*to reduce the CMD line overshoot,modify the CMD drive current to 6MA*/
static struct msm_mmc_pad_drv sdc1_pad_drv_on_cfg[] = {
	{TLMM_HDRV_SDC1_CLK, GPIO_CFG_16MA},
#ifdef CONFIG_HUAWEI_KERNEL
	{TLMM_HDRV_SDC1_CMD, GPIO_CFG_6MA},
#else
	{TLMM_HDRV_SDC1_CMD, GPIO_CFG_10MA},
#endif
	{TLMM_HDRV_SDC1_DATA, GPIO_CFG_10MA}
};

static struct msm_mmc_pad_drv sdc1_pad_drv_off_cfg[] = {
	{TLMM_HDRV_SDC1_CLK, GPIO_CFG_2MA},
	{TLMM_HDRV_SDC1_CMD, GPIO_CFG_2MA},
	{TLMM_HDRV_SDC1_DATA, GPIO_CFG_2MA}
};

static struct msm_mmc_pad_pull sdc1_pad_pull_on_cfg[] = {
	{TLMM_PULL_SDC1_CLK, GPIO_CFG_NO_PULL},
	{TLMM_PULL_SDC1_CMD, GPIO_CFG_PULL_UP},
	{TLMM_PULL_SDC1_DATA, GPIO_CFG_PULL_UP}
};

static struct msm_mmc_pad_pull sdc1_pad_pull_off_cfg[] = {
	{TLMM_PULL_SDC1_CLK, GPIO_CFG_NO_PULL},
	{TLMM_PULL_SDC1_CMD, GPIO_CFG_PULL_UP},
	{TLMM_PULL_SDC1_DATA, GPIO_CFG_PULL_UP}
};

/* SDC3 pad data */
static struct msm_mmc_pad_drv sdc3_pad_drv_on_cfg[] = {
#ifdef CONFIG_HUAWEI_KERNEL
	{TLMM_HDRV_SDC3_CLK, GPIO_CFG_12MA},
#else
	{TLMM_HDRV_SDC3_CLK, GPIO_CFG_8MA},
#endif
	{TLMM_HDRV_SDC3_CMD, GPIO_CFG_8MA},
	{TLMM_HDRV_SDC3_DATA, GPIO_CFG_8MA}
};

static struct msm_mmc_pad_drv sdc3_pad_drv_off_cfg[] = {
	{TLMM_HDRV_SDC3_CLK, GPIO_CFG_2MA},
	{TLMM_HDRV_SDC3_CMD, GPIO_CFG_2MA},
	{TLMM_HDRV_SDC3_DATA, GPIO_CFG_2MA}
};

static struct msm_mmc_pad_pull sdc3_pad_pull_on_cfg[] = {
	{TLMM_PULL_SDC3_CLK, GPIO_CFG_NO_PULL},
	{TLMM_PULL_SDC3_CMD, GPIO_CFG_PULL_UP},
	{TLMM_PULL_SDC3_DATA, GPIO_CFG_PULL_UP}
};

static struct msm_mmc_pad_pull sdc3_pad_pull_off_cfg[] = {
	{TLMM_PULL_SDC3_CLK, GPIO_CFG_NO_PULL},
	/*
	 * SDC3 CMD line should be PULLed UP otherwise fluid platform will
	 * see transitions (1 -> 0 and 0 -> 1) on card detection line,
	 * which would result in false card detection interrupts.
	 */
    /* pull down to remove the mid-level voltage on cmd */
#ifdef CONFIG_HUAWEI_KERNEL
	{TLMM_PULL_SDC3_CMD, GPIO_CFG_PULL_DOWN},
#else
	{TLMM_PULL_SDC3_CMD, GPIO_CFG_PULL_UP},
#endif
	/*
	 * Keeping DATA lines status to PULL UP will make sure that
	 * there is no current leak during sleep if external pull up
	 * is connected to DATA lines.
	 */
#ifdef CONFIG_HUAWEI_KERNEL
	{TLMM_PULL_SDC3_DATA, GPIO_CFG_PULL_DOWN}
#else
	{TLMM_PULL_SDC3_DATA, GPIO_CFG_PULL_UP}
#endif
};
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
/* SDC4 pad data */
static struct msm_mmc_pad_drv sdc4_pad_drv_on_cfg[] = {
	{TLMM_HDRV_SDC4_CLK, GPIO_CFG_16MA},
	{TLMM_HDRV_SDC4_CMD, GPIO_CFG_16MA},
	{TLMM_HDRV_SDC4_DATA, GPIO_CFG_16MA}
};

static struct msm_mmc_pad_drv sdc4_pad_drv_off_cfg[] = {
	{TLMM_HDRV_SDC4_CLK, GPIO_CFG_2MA},
	{TLMM_HDRV_SDC4_CMD, GPIO_CFG_2MA},
	{TLMM_HDRV_SDC4_DATA, GPIO_CFG_2MA}
};

static struct msm_mmc_pad_pull sdc4_pad_pull_on_cfg[] = {
	{TLMM_PULL_SDC4_CLK, GPIO_CFG_NO_PULL},
	{TLMM_PULL_SDC4_CMD, GPIO_CFG_PULL_UP},
	{TLMM_PULL_SDC4_DATA, GPIO_CFG_PULL_UP}
};

static struct msm_mmc_pad_pull sdc4_pad_pull_off_cfg[] = {
	{TLMM_PULL_SDC4_CLK, GPIO_CFG_NO_PULL},
	{TLMM_PULL_SDC4_CMD, GPIO_CFG_NO_PULL},
	{TLMM_PULL_SDC4_DATA, GPIO_CFG_NO_PULL}
};
#endif

static struct msm_mmc_pad_pull_data mmc_pad_pull_data[MAX_SDCC_CONTROLLER] = {
	[SDCC1] = {
		.on = sdc1_pad_pull_on_cfg,
		.off = sdc1_pad_pull_off_cfg,
		.size = ARRAY_SIZE(sdc1_pad_pull_on_cfg)
	},
	[SDCC3] = {
		.on = sdc3_pad_pull_on_cfg,
		.off = sdc3_pad_pull_off_cfg,
		.size = ARRAY_SIZE(sdc3_pad_pull_on_cfg)
	},
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
        [SDCC4] = {
            .on = sdc4_pad_pull_on_cfg,
            .off = sdc4_pad_pull_off_cfg,
            .size = ARRAY_SIZE(sdc4_pad_pull_on_cfg)
        },
#endif
};

static struct msm_mmc_pad_drv_data mmc_pad_drv_data[MAX_SDCC_CONTROLLER] = {
	[SDCC1] = {
		.on = sdc1_pad_drv_on_cfg,
		.off = sdc1_pad_drv_off_cfg,
		.size = ARRAY_SIZE(sdc1_pad_drv_on_cfg)
	},
	[SDCC3] = {
		.on = sdc3_pad_drv_on_cfg,
		.off = sdc3_pad_drv_off_cfg,
		.size = ARRAY_SIZE(sdc3_pad_drv_on_cfg)
	},
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
        [SDCC4] = {
            .on = sdc4_pad_drv_on_cfg,
            .off = sdc4_pad_drv_off_cfg,
            .size = ARRAY_SIZE(sdc4_pad_drv_on_cfg)
        },
#endif
};

static struct msm_mmc_pad_data mmc_pad_data[MAX_SDCC_CONTROLLER] = {
	[SDCC1] = {
		.pull = &mmc_pad_pull_data[SDCC1],
		.drv = &mmc_pad_drv_data[SDCC1]
	},
	[SDCC3] = {
		.pull = &mmc_pad_pull_data[SDCC3],
		.drv = &mmc_pad_drv_data[SDCC3]
	},
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
        [SDCC4] = {
            .pull = &mmc_pad_pull_data[SDCC4],
            .drv = &mmc_pad_drv_data[SDCC4]
        },
#endif
};

static struct msm_mmc_pin_data mmc_slot_pin_data[MAX_SDCC_CONTROLLER] = {
	[SDCC1] = {
		.pad_data = &mmc_pad_data[SDCC1],
	},
	[SDCC3] = {
		.pad_data = &mmc_pad_data[SDCC3],
	},
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
        [SDCC4] = {
            .pad_data = &mmc_pad_data[SDCC4],
        },
#endif
};

#define MSM_MPM_PIN_SDC1_DAT1	17
#define MSM_MPM_PIN_SDC3_DAT1	21

static unsigned int sdc1_sup_clk_rates[] = {
	400000, 24000000, 48000000, 96000000
};

#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
static unsigned int sdc3_sup_clk_rates[] = {
	400000, 24000000, 48000000, 96000000, 192000000,
};
#endif

/*set sdc4 clk 48M*/
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
static unsigned int sdc4_sup_clk_rates[] = {
	400000, 24000000, 48000000
};
#endif

#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
static struct mmc_platform_data msm8960_sdc1_data = {
	.ocr_mask       = MMC_VDD_27_28 | MMC_VDD_28_29,
#ifdef CONFIG_MMC_MSM_SDC1_8_BIT_SUPPORT
	.mmc_bus_width  = MMC_CAP_8_BIT_DATA,
#else
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
#endif
	.sup_clk_table	= sdc1_sup_clk_rates,
	.sup_clk_cnt	= ARRAY_SIZE(sdc1_sup_clk_rates),
	.nonremovable	= 1,
	.vreg_data	= &mmc_slot_vreg_data[SDCC1],
	.pin_data	= &mmc_slot_pin_data[SDCC1],
	.mpm_sdiowakeup_int = MSM_MPM_PIN_SDC1_DAT1,
	.msm_bus_voting_data = &sps_to_ddr_bus_voting_data,
	.uhs_caps2	= MMC_CAP2_HS200_1_8V_SDR,
};
#endif

#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
static struct mmc_platform_data msm8960_sdc3_data = {
	.ocr_mask       = MMC_VDD_27_28 | MMC_VDD_28_29,
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
	.sup_clk_table	= sdc3_sup_clk_rates,
	.sup_clk_cnt	= ARRAY_SIZE(sdc3_sup_clk_rates),
#ifdef CONFIG_MMC_MSM_SDC3_WP_SUPPORT
/*TODO: Insert right replacement for PM8038 */
#ifndef MSM8930_PHASE_2
	.wpswitch_gpio	= PM8921_GPIO_PM_TO_SYS(16),
#else
	.wpswitch_gpio	= 66,
	.is_wpswitch_active_low = true,
#endif
#endif
	.vreg_data	= &mmc_slot_vreg_data[SDCC3],
	.pin_data	= &mmc_slot_pin_data[SDCC3],
/*TODO: Insert right replacement for PM8038 */
#ifndef MSM8930_PHASE_2
	.status_gpio	= PM8921_GPIO_PM_TO_SYS(26),
	.status_irq	= PM8921_GPIO_IRQ(PM8921_IRQ_BASE, 26),
#else
	.status_gpio	= 94,
	.status_irq	= MSM_GPIO_TO_INT(94),
#endif
	.irq_flags	= IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
	.is_status_gpio_active_low = true,
	.xpc_cap	= 1,
	.uhs_caps	= (MMC_CAP_UHS_SDR12 | MMC_CAP_UHS_SDR25 |
			MMC_CAP_UHS_SDR50 | MMC_CAP_UHS_DDR50 |
			MMC_CAP_UHS_SDR104 | MMC_CAP_MAX_CURRENT_800),
	.mpm_sdiowakeup_int = MSM_MPM_PIN_SDC3_DAT1,
	.msm_bus_voting_data = &sps_to_ddr_bus_voting_data,
};
#endif

#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
static struct mmc_platform_data sdc4_plat_data = {
	.ocr_mask       = MMC_VDD_165_195 | MMC_VDD_27_28 | MMC_VDD_28_29, 
	.mmc_bus_width  = MMC_CAP_4_BIT_DATA,
	.sup_clk_table	= sdc4_sup_clk_rates,
	.sup_clk_cnt	= ARRAY_SIZE(sdc4_sup_clk_rates),
	.vreg_data	= &mmc_slot_vreg_data[SDCC4],
	.pin_data	= &mmc_slot_pin_data[SDCC4],
	.xpc_cap	= 1,
	.msm_bus_voting_data = &sps_to_ddr_bus_voting_data,
	.uhs_caps	= (MMC_CAP_UHS_SDR12 | MMC_CAP_UHS_SDR25 |
			MMC_CAP_UHS_SDR50 | MMC_CAP_UHS_DDR50 |
			MMC_CAP_MAX_CURRENT_600)
};
#endif

#ifdef CONFIG_HUAWEI_WIFI_SDCC
#define TAG_BCM			"BCM_4330"

#define PREALLOC_WLAN_NUMBER_OF_SECTIONS	4
#define PREALLOC_WLAN_NUMBER_OF_BUFFERS		160
#define PREALLOC_WLAN_SECTION_HEADER		24

#define WLAN_SECTION_SIZE_0	(PREALLOC_WLAN_NUMBER_OF_BUFFERS * 128)
#define WLAN_SECTION_SIZE_1	(PREALLOC_WLAN_NUMBER_OF_BUFFERS * 128)
#define WLAN_SECTION_SIZE_2	(PREALLOC_WLAN_NUMBER_OF_BUFFERS * 512)
#define WLAN_SECTION_SIZE_3	(PREALLOC_WLAN_NUMBER_OF_BUFFERS * 1024)

#define WLAN_SKB_BUF_NUM			16

/* for wifi awake */
#define WLAN_WAKES_MSM        		92	
/* for wifi power supply */
#define WLAN_REG 					1

#define WLAN_GPIO_FUNC_0         	0
#define WLAN_GPIO_FUNC_1         	1
#define WLAN_STAT_ON             	1
#define WLAN_STAT_OFF            	0
	
static unsigned wlan_wakes_msm[] = {
	GPIO_CFG( WLAN_WAKES_MSM, WLAN_GPIO_FUNC_0 , GPIO_CFG_INPUT, 
GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA ) 
};

static unsigned wifi_config_init[] = {
	GPIO_CFG( WLAN_REG, WLAN_GPIO_FUNC_0 , GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL , 
GPIO_CFG_2MA ) 
};

static struct sk_buff *wlan_static_skb[WLAN_SKB_BUF_NUM];

typedef struct wifi_mem_prealloc_struct {
	void *mem_ptr;
	unsigned long size;
} wifi_mem_prealloc_t;

static wifi_mem_prealloc_t wifi_mem_array[PREALLOC_WLAN_NUMBER_OF_SECTIONS] = {
	{ NULL, (WLAN_SECTION_SIZE_0 + PREALLOC_WLAN_SECTION_HEADER) },
	{ NULL, (WLAN_SECTION_SIZE_1 + PREALLOC_WLAN_SECTION_HEADER) },
	{ NULL, (WLAN_SECTION_SIZE_2 + PREALLOC_WLAN_SECTION_HEADER) },
	{ NULL, (WLAN_SECTION_SIZE_3 + PREALLOC_WLAN_SECTION_HEADER) }
};

/*wlan static memory alloc*/
static void *bcm_wifi_mem_prealloc(int section, unsigned long size)
{
	if (section == PREALLOC_WLAN_NUMBER_OF_SECTIONS)
		return wlan_static_skb;
	if ((section < 0) || (section > PREALLOC_WLAN_NUMBER_OF_SECTIONS))
		return NULL;
	if (wifi_mem_array[section].size < size)
		return NULL;
	return wifi_mem_array[section].mem_ptr;
}

/*wlan power control*/
static int bcm_wifi_set_power(int enable)
{
	int ret = 0;
	
   	if (enable)
	{
			/* turn on wifi_vreg */
            ret = gpio_direction_output(WLAN_REG, WLAN_STAT_ON);
            if (ret < 0) {
            	printk(KERN_ERR "%s: turn on wlan_reg failed (%d)\n" , __func__, ret);
            	return -EIO;
            }
            mdelay(150);
            printk(KERN_ERR "%s: wifi power successed to pull up\n" , __func__ );
		
	}
    else { 
        	/* turn off wifi_vreg */
            ret = gpio_direction_output(WLAN_REG, WLAN_STAT_OFF);
            if (ret < 0) {
            	printk(KERN_ERR "%s: turn off wlan_reg failed (%d)\n" , __func__, ret);
            	return -EIO;
            }
            mdelay(1);
            printk(KERN_ERR "%s: wifi power successed to pull down\n",__func__ );
	}

	return ret;
}

int __init bcm_wifi_init_gpio_mem(void)
{
	int i = 0;
	int rc = 0;


	/* config gpio WLAN_WAKES_MSM */
	rc = gpio_tlmm_config(wlan_wakes_msm[0], GPIO_CFG_ENABLE);	
	if( rc ) 
		printk(KERN_ERR "%s: %s gpio_tlmm_config(wlan_wakes_msm) failed rc = %d\n", __func__ , TAG_BCM , rc);
	else 
		printk(KERN_ERR "%s: %s gpio_tlmm_config(wlan_wakes_msm) successfully\n", __func__ , TAG_BCM);
	/* request gpio WLAN_WAKES_MSM */
	rc = gpio_request( WLAN_WAKES_MSM , "WLAN_WAKES_MSM" );
	if( rc ) 
		printk(KERN_ERR "%s: %s Failed to gpio_request(WLAN_WAKES_MSM) rc = %d\n" , __func__ , TAG_BCM , rc );	
	else 
		printk(KERN_ERR "%s: %s Success to gpio_request(WLAN_WAKES_MSM)\n" , __func__ , TAG_BCM );	

	/* config gpio WLAN_REG */
	rc = gpio_tlmm_config(wifi_config_init[0], GPIO_CFG_ENABLE);
	if( rc )
		printk(KERN_ERR "%s: %s gpio_tlmm_config(wifi_config_init) failed rc = %d\n", __func__ , TAG_BCM , rc);
	else
		printk(KERN_ERR "%s: %s gpio_tlmm_config(wifi_config_init) successfully\n", __func__ , TAG_BCM);
	/* request gpio WLAN_REG */
	rc = gpio_request( WLAN_REG , "WLAN_REG" );
	if( rc )
		printk(KERN_ERR "%s: %s Failed to gpio_request(WLAN_REG) rc = %d\n" , __func__ , TAG_BCM , rc);
	else
		printk(KERN_ERR "%s: %s Success to gpio_request(WLAN_REG)\n" , __func__ , TAG_BCM );
	
    mdelay(5);

    /* turn off wifi_vreg */
    rc = gpio_direction_output(WLAN_REG, 0);
    if (rc < 0) {
		printk(KERN_ERR "%s: %s turn off wlan_reg failed (%d)\n" , __func__, TAG_BCM,  rc);
		return -EIO;
    }
    else {
		printk(KERN_ERR "%s: %s turn off wlan_reg successfully (%d)\n" , __func__, TAG_BCM,  rc);
    }

    mdelay(5);
       
	for(i=0;( i < WLAN_SKB_BUF_NUM );i++) {
		if (i < (WLAN_SKB_BUF_NUM/2))
			wlan_static_skb[i] = dev_alloc_skb(4096); 	/* malloc skb 4k buffer */
		else
			wlan_static_skb[i] = dev_alloc_skb(32768); 	/* malloc skb 32k buffer */
	}
	for(i=0;( i < PREALLOC_WLAN_NUMBER_OF_SECTIONS );i++) {
		wifi_mem_array[i].mem_ptr = kmalloc(wifi_mem_array[i].size,
							GFP_KERNEL);
		if (wifi_mem_array[i].mem_ptr == NULL)
			return -ENOMEM;
	}
	
	printk("%s: %s bcm_wifi_init_gpio_mem successfully\n" , __func__ , TAG_BCM );
	return 0;
}

static struct wifi_platform_data bcm_wifi_control = {
	.mem_prealloc	= bcm_wifi_mem_prealloc,
	.set_power	=bcm_wifi_set_power,
};

static struct platform_device bcm_wifi_device = {
        .name           = "bcm4330_wlan",	/*bcm4330 wlan device*/
        .id             = 1,
        .num_resources  = 0,
        .resource       = NULL,
        .dev            = {
                .platform_data = &bcm_wifi_control,
        },
};
#endif

extern int sdcc_wifi_slot;

void __init msm8930_init_mmc(void)
{
	hw_wifi_device_type  wifi_device_type = WIFI_TYPE_UNKNOWN;
#ifdef CONFIG_HUAWEI_KERNEL
    hw_sd_trigger_type  trigger_type = FALL_TRIGGER;
#endif	
#ifdef CONFIG_MMC_MSM_SDC1_SUPPORT
	/*
	 * When eMMC runs in DDR mode on CDP platform, we have
	 * seen instability due to DATA CRC errors. These errors are
	 * attributed to long physical path between MSM and eMMC on CDP.
	 * So let's not enable the DDR mode on CDP platform but let other
	 * platforms take advantage of eMMC DDR mode.
	 */
	if (!machine_is_msm8930_cdp())
		msm8960_sdc1_data.uhs_caps |= (MMC_CAP_1_8V_DDR |
					       MMC_CAP_UHS_DDR50);
	/* SDC1 : eMMC card connected */
	msm_add_sdcc(1, &msm8960_sdc1_data);
#endif
#ifdef CONFIG_MMC_MSM_SDC3_SUPPORT
	/*
	 * All 8930 platform boards using the 1.2 SoC have been reworked so that
	 * the sd card detect line's esd circuit is no longer powered by the sd
	 * card's voltage regulator. So this means we can turn the regulator off
	 * to save power without affecting the sd card detect functionality.
	 * This change to the boards will be true for newer versions of the SoC
	 * as well.
	 */
	if ((SOCINFO_VERSION_MAJOR(socinfo_get_version()) >= 1 &&
			SOCINFO_VERSION_MINOR(socinfo_get_version()) >= 2) ||
			machine_is_msm8930_cdp()) {
		msm8960_sdc3_data.vreg_data->vdd_data->always_on = false;
		msm8960_sdc3_data.vreg_data->vdd_data->reset_at_init = false;
	}

	/* SDC3: External card slot */
	if (!machine_is_msm8930_cdp()) {
		msm8960_sdc3_data.wpswitch_gpio = 0;
		msm8960_sdc3_data.is_wpswitch_active_low = false;
	}
	
    /* The products may use different sd trigger type, depends on the sdcard slot.
     * Get the trigger type from the product configuration, and reassign the platform data.
     * If it is not set, use the platform data directly.
     */
#ifdef CONFIG_HUAWEI_KERNEL    
    trigger_type = get_hw_sd_trigger_type();
    /* FALL_TRIGGER means when the sdcard is inserted, the status gpio becomes low */
    if (FALL_TRIGGER == trigger_type) {
        msm8960_sdc3_data.is_status_gpio_active_low = true;
        printk("%s: sdcard is fall trigger \n", __func__);
    }        
    /* RAISE_TRIGGER means when the sdcard is inserted, the status gpio becomes high */
    else if(RAISE_TRIGGER == trigger_type) {        
        msm8960_sdc3_data.is_status_gpio_active_low = false;          
        printk("%s: sdcard is raise trigger \n", __func__);
    }
    else {
        printk("%s: sdcard trigger type is not set, use platform data directly \n", __func__);
    }
#endif	

	msm_add_sdcc(3, &msm8960_sdc3_data);
#endif
/* SDIO WLAN slot */
#ifdef CONFIG_MMC_MSM_SDC4_SUPPORT
	/*optimize the code*/
    wifi_device_type = get_hw_wifi_device_type();
    if((WIFI_BROADCOM_4330 == wifi_device_type)||(WIFI_BROADCOM_4330X == wifi_device_type))
    {
        sdcc_wifi_slot = 4;
        msm_add_sdcc(4, &sdc4_plat_data);
#ifdef CONFIG_HUAWEI_WIFI_SDCC
        bcm_wifi_init_gpio_mem();
        platform_device_register(&bcm_wifi_device);
#endif
    }
#endif

}
