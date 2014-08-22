/* Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
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
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/i2c.h>
#include <linux/i2c/sx150x.h>
#include <linux/i2c/isl9519.h>
#include <linux/gpio.h>
#include <linux/msm_ssbi.h>
#include <linux/regulator/msm-gpio-regulator.h>
#include <linux/mfd/pm8xxx/pm8921.h>
#include <linux/mfd/pm8xxx/pm8xxx-adc.h>
#include <linux/regulator/consumer.h>
#include <linux/spi/spi.h>
#include <linux/slimbus/slimbus.h>
#include <linux/bootmem.h>
#ifdef CONFIG_ANDROID_PMEM
#include <linux/android_pmem.h>
#endif
#include <linux/dma-mapping.h>
#include <linux/platform_data/qcom_crypto_device.h>
#include <linux/platform_data/qcom_wcnss_device.h>
#include <linux/leds.h>
#include <linux/leds-pm8xxx.h>
#include <linux/i2c/atmel_mxt_ts.h>
#include <linux/msm_tsens.h>
#include <linux/ks8851.h>
#include <linux/i2c/isa1200.h>
#include <linux/gpio_keys.h>
#include <linux/memory.h>
#include <linux/memblock.h>
#include <linux/msm_thermal.h>

#include <linux/slimbus/slimbus.h>
#include <linux/mfd/wcd9xxx/core.h>
#include <linux/mfd/wcd9xxx/pdata.h>

#ifdef CONFIG_STM_LIS3DH
#include <linux/input/lis3dh.h>
#endif

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/setup.h>
#include <asm/hardware/gic.h>
#include <asm/mach/mmc.h>

#include <mach/board.h>
#include <mach/msm_iomap.h>
#include <mach/msm_spi.h>
#ifdef CONFIG_USB_MSM_OTG_72K
#include <mach/msm_hsusb.h>
#else
#include <linux/usb/msm_hsusb.h>
#endif
#include <linux/usb/android.h>
#include <mach/usbdiag.h>
#include <mach/socinfo.h>
#include <mach/rpm.h>
#include <mach/gpiomux.h>
#include <mach/msm_bus_board.h>
#include <mach/msm_memtypes.h>
#include <mach/dma.h>
#include <mach/msm_xo.h>
#include <mach/restart.h>

#include <linux/msm_ion.h>
#include <mach/ion.h>
#include <mach/mdm2.h>
#include <mach/msm_rtb.h>
#include <linux/fmem.h>
#include <mach/msm_cache_dump.h>

#include <mach/kgsl.h>
#ifdef CONFIG_INPUT_MPU3050
#include <linux/input/mpu3050.h>
#endif

#include "timer.h"
#include "devices.h"
#include "devices-msm8x60.h"
#include "spm.h"
#include "pm.h"
#include <mach/cpuidle.h>
#include "rpm_resources.h"
#include <mach/mpm.h>
#include "smd_private.h"
#include "pm-boot.h"
#include "msm_watchdog.h"
#include "board-8930.h"
#include "acpuclock-krait.h"
#ifdef CONFIG_TOUCHSCREEN_CYPRESS_CYTTSP4
#include <linux/cyttsp4_bus.h>
#include <linux/cyttsp4_core.h>
#include <linux/cyttsp4_btn.h>
#include <linux/cyttsp4_mt.h>
#endif
#ifdef CONFIG_HUAWEI_KERNEL
#include "hw-board-8930.h"
#endif
#ifdef CONFIG_HW_POWER_TREE 
struct hw_config_power_tree* config_power_tree_pdata = NULL;
struct regulator_init_data* msm_saw_regulator_pdata__ = NULL;
struct gpio_regulator_platform_data* msm_gpio_regulator_pdata__ = NULL;
#endif

#ifdef CONFIG_HUAWEI_KERNEL
#include <asm-arm/huawei/smem_vendor_huawei.h>
#endif

static struct platform_device msm_fm_platform_init = {
	.name = "iris_fm",
	.id   = -1,
};
#ifdef CONFIG_TOUCHSCREEN_CYPRESS_CYTTSP4

#ifdef CONFIG_TOUCHSCREEN_CYPRESS_CYTTSP4_INCLUDE_FW
#include "linux/cyttsp4_img.h"
static struct cyttsp4_touch_firmware cyttsp4_firmware = {
	.img = cyttsp4_img,
	.size = ARRAY_SIZE(cyttsp4_img),
	.ver = cyttsp4_ver,
	.vsize = ARRAY_SIZE(cyttsp4_ver),
};
#else
static struct cyttsp4_touch_firmware cyttsp4_firmware = {
	.img = NULL,
	.size = 0,
	.ver = NULL,
	.vsize = 0,
};
#endif

static struct touch_settings cyttsp4_sett_param_regs = {
	.data = NULL,
	.size = 0,
	.tag = 0,
};

static struct touch_settings cyttsp4_sett_param_size = {
	.data = NULL,
	.size = 0,
	.tag = 0,
};

struct cyttsp4_sett_param_map cyttsp4_config_param_map[] = {
	[0] = {
			  .id = 0,
			  .param = NULL,
		  },
	
	[1] = {
			  .id = 2,
			  .param = NULL,
		  },
	
	[2] = {
			  .param = NULL,
		  },
};


static struct cyttsp4_loader_platform_data _cyttsp4_loader_platform_data = {
	.fw = &cyttsp4_firmware,
	.param_regs = &cyttsp4_sett_param_regs,
	.param_size = &cyttsp4_sett_param_size,
	.param_map = cyttsp4_config_param_map,
	.flags = 1,
};

#define CYTTSP4_USE_I2C

#ifdef CYTTSP4_USE_I2C
#define CYTTSP4_I2C_NAME "cyttsp4_i2c_adapter"
#define CYTTSP4_I2C_TCH_ADR 0x1A
#define CYTTSP4_LDR_TCH_ADR 0x1A
#define CYTTSP4_I2C_IRQ_GPIO  82 /* J6.9, C19, GPMC_AD14/GPIO_38 */
#define CYTTSP4_I2C_RST_GPIO  96 /* J6.10, D18, GPMC_AD13/GPIO_37 */
#endif
#define CY4_MAXX 720
#define CY4_MAXY 1280

#define CY4_MINX 0
#define CY4_MINY 0

#define CY4_ABS_MIN_X CY4_MINX
#define CY4_ABS_MIN_Y CY4_MINY
#define CY4_ABS_MAX_X CY4_MAXX
#define CY4_ABS_MAX_Y CY4_MAXY
#define CY4_ABS_MIN_P 0
#define CY4_ABS_MAX_P 255
#define CY4_ABS_MIN_W 0
#define CY4_ABS_MAX_W 255

#define CY4_ABS_MIN_T 0

#define CY4_ABS_MAX_T 15
#define CY_IGNORE_VALUE 0xFFFF
/* Cypress cyttsp4 end */
#endif
#define KS8851_RST_GPIO		89
#define KS8851_IRQ_GPIO		90
#define HAP_SHIFT_LVL_OE_GPIO	47

#define HDMI_MHL_MUX_GPIO       73
#define MHL_GPIO_INT            72
#define MHL_GPIO_RESET          71
#define MHL_GPIO_PWR_EN         5

#if defined(CONFIG_GPIO_SX150X) || defined(CONFIG_GPIO_SX150X_MODULE)

struct sx150x_platform_data msm8930_sx150x_data[] = {
	[SX150X_CAM] = {
		.gpio_base         = GPIO_CAM_EXPANDER_BASE,
		.oscio_is_gpo      = false,
		.io_pullup_ena     = 0x0,
		.io_pulldn_ena     = 0xc0,
		.io_open_drain_ena = 0x0,
		.irq_summary       = -1,
	},
};

#endif

#define MSM_PMEM_ADSP_SIZE         0x7800000
#define MSM_PMEM_AUDIO_SIZE        0x408000
#ifdef CONFIG_FB_MSM_HDMI_AS_PRIMARY
#define MSM_PMEM_SIZE 0x4000000 /* 64 Mbytes */
#else
#define MSM_PMEM_SIZE 0x2800000 /* 40 Mbytes */
#endif
#define MSM_LIQUID_PMEM_SIZE 0x4000000 /* 64 Mbytes */

#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
#define HOLE_SIZE	0x20000
#define MSM_CONTIG_MEM_SIZE  0x65000
#ifdef CONFIG_MSM_IOMMU
#define MSM_ION_MM_SIZE            0x3800000 /* Need to be multiple of 64K */
#define MSM_ION_SF_SIZE            0x0
#define MSM_ION_QSECOM_SIZE	0x780000 /* (7.5MB) */
#define MSM_ION_HEAP_NUM	7
#else
#define MSM_ION_SF_SIZE		MSM_PMEM_SIZE
#define MSM_ION_MM_SIZE		MSM_PMEM_ADSP_SIZE
#define MSM_ION_QSECOM_SIZE	0x600000 /* (6MB) */
#define MSM_ION_HEAP_NUM	8
#endif
#define MSM_ION_MM_FW_SIZE	(0x200000 - HOLE_SIZE) /* 2MB - 128Kb */
#define MSM_ION_MFC_SIZE	SZ_8K
#define MSM_ION_AUDIO_SIZE	MSM_PMEM_AUDIO_SIZE

#define MSM_LIQUID_ION_MM_SIZE (MSM_ION_MM_SIZE + 0x600000)
#define MSM_LIQUID_ION_SF_SIZE MSM_LIQUID_PMEM_SIZE
#define MSM_HDMI_PRIM_ION_SF_SIZE MSM_HDMI_PRIM_PMEM_SIZE

#define MSM_MM_FW_SIZE	(0x200000 - HOLE_SIZE) /*2MB -128Kb */
#define MSM8930_FIXED_AREA_START (0xa0000000 - (MSM_ION_MM_FW_SIZE + \
								HOLE_SIZE))
#define MAX_FIXED_AREA_SIZE	0x10000000
#define MSM8930_FW_START	MSM8930_FIXED_AREA_START

#else
#define MSM_CONTIG_MEM_SIZE  0x110C000
#define MSM_ION_HEAP_NUM	1
#endif

/*deleted unused code.*/
#ifdef CONFIG_KERNEL_MSM_CONTIG_MEM_REGION
static unsigned msm_contig_mem_size = MSM_CONTIG_MEM_SIZE;
static int __init msm_contig_mem_size_setup(char *p)
{
	msm_contig_mem_size = memparse(p, NULL);
	return 0;
}
early_param("msm_contig_mem_size", msm_contig_mem_size_setup);
#endif

#ifdef CONFIG_ANDROID_PMEM
static unsigned pmem_size = MSM_PMEM_SIZE;
static int __init pmem_size_setup(char *p)
{
	pmem_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_size", pmem_size_setup);

static unsigned pmem_adsp_size = MSM_PMEM_ADSP_SIZE;

static int __init pmem_adsp_size_setup(char *p)
{
	pmem_adsp_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_adsp_size", pmem_adsp_size_setup);

static unsigned pmem_audio_size = MSM_PMEM_AUDIO_SIZE;

static int __init pmem_audio_size_setup(char *p)
{
	pmem_audio_size = memparse(p, NULL);
	return 0;
}
early_param("pmem_audio_size", pmem_audio_size_setup);
#endif

#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
static struct android_pmem_platform_data android_pmem_pdata = {
	.name = "pmem",
	.allocator_type = PMEM_ALLOCATORTYPE_ALLORNOTHING,
	.cached = 1,
	.memory_type = MEMTYPE_EBI1,
};

static struct platform_device msm8930_android_pmem_device = {
	.name = "android_pmem",
	.id = 0,
	.dev = {.platform_data = &android_pmem_pdata},
};

static struct android_pmem_platform_data android_pmem_adsp_pdata = {
	.name = "pmem_adsp",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
	.memory_type = MEMTYPE_EBI1,
};
static struct platform_device msm8930_android_pmem_adsp_device = {
	.name = "android_pmem",
	.id = 2,
	.dev = { .platform_data = &android_pmem_adsp_pdata },
};

static struct android_pmem_platform_data android_pmem_audio_pdata = {
	.name = "pmem_audio",
	.allocator_type = PMEM_ALLOCATORTYPE_BITMAP,
	.cached = 0,
	.memory_type = MEMTYPE_EBI1,
};

static struct platform_device msm8930_android_pmem_audio_device = {
	.name = "android_pmem",
	.id = 4,
	.dev = { .platform_data = &android_pmem_audio_pdata },
};
#endif /* CONFIG_MSM_MULTIMEDIA_USE_ION */
#endif /* CONFIG_ANDROID_PMEM */

struct fmem_platform_data msm8930_fmem_pdata = {
};

#define DSP_RAM_BASE_8960 0x8da00000
#define DSP_RAM_SIZE_8960 0x1800000
static int dspcrashd_pdata_8960 = 0xDEADDEAD;

static struct resource resources_dspcrashd_8960[] = {
	{
		.name   = "msm_dspcrashd",
		.start  = DSP_RAM_BASE_8960,
		.end    = DSP_RAM_BASE_8960 + DSP_RAM_SIZE_8960,
		.flags  = IORESOURCE_DMA,
	},
};

static struct platform_device msm_device_dspcrashd_8960 = {
	.name           = "msm_dspcrashd",
	.num_resources  = ARRAY_SIZE(resources_dspcrashd_8960),
	.resource       = resources_dspcrashd_8960,
	.dev = { .platform_data = &dspcrashd_pdata_8960 },
};

static struct memtype_reserve msm8930_reserve_table[] __initdata = {
	[MEMTYPE_SMI] = {
	},
	[MEMTYPE_EBI0] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
	[MEMTYPE_EBI1] = {
		.flags	=	MEMTYPE_FLAGS_1M_ALIGN,
	},
};


static void __init reserve_rtb_memory(void)
{
#if defined(CONFIG_MSM_RTB)
	msm8930_reserve_table[MEMTYPE_EBI1].size += msm8930_rtb_pdata.size;
#endif
}

static void __init size_pmem_devices(void)
{
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	android_pmem_adsp_pdata.size = pmem_adsp_size;
	android_pmem_pdata.size = pmem_size;
	android_pmem_audio_pdata.size = MSM_PMEM_AUDIO_SIZE;
#endif /*CONFIG_MSM_MULTIMEDIA_USE_ION*/
#endif /*CONFIG_ANDROID_PMEM*/
}

#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
static void __init reserve_memory_for(struct android_pmem_platform_data *p)
{
	msm8930_reserve_table[p->memory_type].size += p->size;
}
#endif /*CONFIG_MSM_MULTIMEDIA_USE_ION*/
#endif /*CONFIG_ANDROID_PMEM*/

static void __init reserve_pmem_memory(void)
{
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	reserve_memory_for(&android_pmem_adsp_pdata);
	reserve_memory_for(&android_pmem_pdata);
	reserve_memory_for(&android_pmem_audio_pdata);
#endif /*CONFIG_MSM_MULTIMEDIA_USE_ION*/
	msm8930_reserve_table[MEMTYPE_EBI1].size += msm_contig_mem_size;
#endif /*CONFIG_ANDROID_PMEM*/
}

static int msm8930_paddr_to_memtype(unsigned int paddr)
{
	return MEMTYPE_EBI1;
}

#define FMEM_ENABLED 0
#ifdef CONFIG_ION_MSM
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
static struct ion_cp_heap_pdata cp_mm_msm8930_ion_pdata = {
	.permission_type = IPT_TYPE_MM_CARVEOUT,
	.align = PAGE_SIZE,
	.reusable = FMEM_ENABLED,
	.mem_is_fmem = FMEM_ENABLED,
	.fixed_position = FIXED_MIDDLE,
};

static struct ion_cp_heap_pdata cp_mfc_msm8930_ion_pdata = {
	.permission_type = IPT_TYPE_MFC_SHAREDMEM,
	.align = PAGE_SIZE,
	.reusable = 0,
	.mem_is_fmem = FMEM_ENABLED,
	.fixed_position = FIXED_HIGH,
};

static struct ion_co_heap_pdata co_msm8930_ion_pdata = {
	.adjacent_mem_id = INVALID_HEAP_ID,
	.align = PAGE_SIZE,
	.mem_is_fmem = 0,
};

static struct ion_co_heap_pdata fw_co_msm8930_ion_pdata = {
	.adjacent_mem_id = ION_CP_MM_HEAP_ID,
	.align = SZ_128K,
	.mem_is_fmem = FMEM_ENABLED,
	.fixed_position = FIXED_LOW,
};
#endif

/**
 * These heaps are listed in the order they will be allocated. Due to
 * video hardware restrictions and content protection the FW heap has to
 * be allocated adjacent (below) the MM heap and the MFC heap has to be
 * allocated after the MM heap to ensure MFC heap is not more than 256MB
 * away from the base address of the FW heap.
 * However, the order of FW heap and MM heap doesn't matter since these
 * two heaps are taken care of by separate code to ensure they are adjacent
 * to each other.
 * Don't swap the order unless you know what you are doing!
 */
static struct ion_platform_data msm8930_ion_pdata = {
	.nr = MSM_ION_HEAP_NUM,
	.heaps = {
		{
			.id	= ION_SYSTEM_HEAP_ID,
			.type	= ION_HEAP_TYPE_SYSTEM,
			.name	= ION_VMALLOC_HEAP_NAME,
		},
#ifdef CONFIG_MSM_MULTIMEDIA_USE_ION
		{
			.id	= ION_CP_MM_HEAP_ID,
			.type	= ION_HEAP_TYPE_CP,
			.name	= ION_MM_HEAP_NAME,
			.size	= MSM_ION_MM_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &cp_mm_msm8930_ion_pdata,
		},
		{
			.id	= ION_MM_FIRMWARE_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_MM_FIRMWARE_HEAP_NAME,
			.size	= MSM_ION_MM_FW_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &fw_co_msm8930_ion_pdata,
		},
		{
			.id	= ION_CP_MFC_HEAP_ID,
			.type	= ION_HEAP_TYPE_CP,
			.name	= ION_MFC_HEAP_NAME,
			.size	= MSM_ION_MFC_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &cp_mfc_msm8930_ion_pdata,
		},
#ifndef CONFIG_MSM_IOMMU
		{
			.id	= ION_SF_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_SF_HEAP_NAME,
			.size	= MSM_ION_SF_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &co_msm8930_ion_pdata,
		},
#endif
		{
			.id	= ION_IOMMU_HEAP_ID,
			.type	= ION_HEAP_TYPE_IOMMU,
			.name	= ION_IOMMU_HEAP_NAME,
		},
		{
			.id	= ION_QSECOM_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_QSECOM_HEAP_NAME,
			.size	= MSM_ION_QSECOM_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &co_msm8930_ion_pdata,
		},
		{
			.id	= ION_AUDIO_HEAP_ID,
			.type	= ION_HEAP_TYPE_CARVEOUT,
			.name	= ION_AUDIO_HEAP_NAME,
			.size	= MSM_ION_AUDIO_SIZE,
			.memory_type = ION_EBI_TYPE,
			.extra_data = (void *) &co_msm8930_ion_pdata,
		},
#endif
	}
};

static struct platform_device msm8930_ion_dev = {
	.name = "ion-msm",
	.id = 1,
	.dev = { .platform_data = &msm8930_ion_pdata },
};
#endif

#ifdef CONFIG_HUAWEI_KERNEL
static struct msm_xo_voter *bcm2079x_nfc_clock = NULL;

/* nfc clock control
 * vote:0 - close for clock pin request mode 
 *      1 - set for clock pin request mode   */
int bcm2079x_clock_output_ctrl(int vote)
{
    int rc = 0;
    const char * id = "bcm_nfc";

    if (bcm2079x_nfc_clock == NULL) 
	{
    	printk("%s in: vote is null!\n",__func__);
        bcm2079x_nfc_clock = msm_xo_get(MSM_XO_TCXO_D1, id);
        if (IS_ERR(bcm2079x_nfc_clock))
		{
            rc = PTR_ERR(bcm2079x_nfc_clock);
            pr_err("Failed to get TCXO_D1 voter (%ld)\n", PTR_ERR(bcm2079x_nfc_clock));
            goto fail_xo_get;
        }
    }

    if (vote == 0)
    {
        rc = msm_xo_mode_vote(bcm2079x_nfc_clock, MSM_XO_MODE_OFF);
        printk("%s: clock pin ctrl off! rc=%d!\n",__func__, rc);
        if (rc < 0)
        {
            pr_err("Configuring TCXO to MSM_XO_MODE_OFF failed  (%d)\n", rc);
			goto fail_xo_mode_vote;
        }        
        return rc;
    }
	
    rc = msm_xo_mode_vote(bcm2079x_nfc_clock, MSM_XO_MODE_PIN_CTRL);
    printk("%s: clock pin ctrl on! rc=%d!\n",__func__, rc);
    if (rc < 0) 
	{
        pr_err("Configuring TCXO to MSM_XO_MODE_PIN_CTRL failed  (%d)\n", rc);
        goto fail_xo_mode_vote;
    } 
    return rc;
 
    
fail_xo_mode_vote:
    msm_xo_put(bcm2079x_nfc_clock);
fail_xo_get:

    printk("%s: failed! rc=%d!\n",__func__, rc);
    return rc;
}

/* configure nfc ven pin to 1.8v */
static struct pm_gpio bcm_nfc_ven_cfg = {
    .direction        = PM_GPIO_DIR_OUT,
    .output_buffer    = PM_GPIO_OUT_BUF_CMOS,
    .output_value     = 0,
    .pull             = PM_GPIO_PULL_NO,
    .vin_sel          = PM8038_GPIO_VIN_L11,
    .out_strength     = PM_GPIO_STRENGTH_HIGH,
    .function         = PM_GPIO_FUNC_NORMAL,
    .inv_int_pol      = 0,
    .disable_pin      = 0,
};

/* this function is used to configure nfc ven pin to 1.8v */
int bcm2079x_ven_cfg(void)
{	
    int ret = 0;

    ret = pm8xxx_gpio_config(PM8038_GPIO_PM_TO_SYS(BCM_NFC_VEN), &bcm_nfc_ven_cfg);   	
    if (ret != 0) 
    {
        printk("%s: Nfc ven gpio config fail, ret = %d\n",__func__, ret);
        return -1;
    }

    return 0;
}

/*
1. NFC_VEN      PM(GPIO_2)
2. NFC_WAKE     MSM8930(GPIO_24)
3. CLK_REQ_NFC  PM£¨MPP_06£©
4. I2C5_DATA    MSM8930(GPIO_95)
5. I2C5_CLK     MSM8930(GPIO_96)
6. NFC_INT      MSM8930(GPIO_106)
7. TCXO_OUT_NFC PM(XO_OUT_D1)
*/
static struct bcm2079x_platform_data bcm2079x_pdata = {
	.irq_gpio = BCM_NFC_IRQ,//NFC_IRQ,
	.en_gpio = PM8038_GPIO_PM_TO_SYS(BCM_NFC_VEN),//NFC_EN,
	.wake_gpio = BCM_NFC_WAKE,//NFC_FIRM,
	.clock_output_ctrl = bcm2079x_clock_output_ctrl,  //NFC clock control
	.ven_cfg = bcm2079x_ven_cfg, //nfc ven cfg
};

static struct i2c_board_info nfc_bcm2079x[] __initdata = {
	{
		I2C_BOARD_INFO("bcm2079x-i2c", BCM_NFC_I2C_ADD),
		.irq = MSM_GPIO_TO_INT(BCM_NFC_IRQ),//IRQ_EINT(12),
		.platform_data = &bcm2079x_pdata,
	},
};
#endif

struct platform_device msm8930_fmem_device = {
	.name = "fmem",
	.id = 1,
	.dev = { .platform_data = &msm8930_fmem_pdata },
};

static void __init reserve_mem_for_ion(enum ion_memory_types mem_type,
				      unsigned long size)
{
	msm8930_reserve_table[mem_type].size += size;
}

static void __init msm8930_reserve_fixed_area(unsigned long fixed_area_size)
{
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
	int ret;

	if (fixed_area_size > MAX_FIXED_AREA_SIZE)
		panic("fixed area size is larger than %dM\n",
			MAX_FIXED_AREA_SIZE >> 20);

	reserve_info->fixed_area_size = fixed_area_size;
	reserve_info->fixed_area_start = MSM8930_FW_START;

	ret = memblock_remove(reserve_info->fixed_area_start,
		reserve_info->fixed_area_size);
	BUG_ON(ret);
#endif
}

/**
 * Reserve memory for ION and calculate amount of reusable memory for fmem.
 * We only reserve memory for heaps that are not reusable. However, we only
 * support one reusable heap at the moment so we ignore the reusable flag for
 * other than the first heap with reusable flag set. Also handle special case
 * for video heaps (MM,FW, and MFC). Video requires heaps MM and MFC to be
 * at a higher address than FW in addition to not more than 256MB away from the
 * base address of the firmware. This means that if MM is reusable the other
 * two heaps must be allocated in the same region as FW. This is handled by the
 * mem_is_fmem flag in the platform data. In addition the MM heap must be
 * adjacent to the FW heap for content protection purposes.
 */
static void __init reserve_ion_memory(void)
{
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
	unsigned int i;
	unsigned int reusable_count = 0;
	unsigned int fixed_size = 0;
	unsigned int fixed_low_size, fixed_middle_size, fixed_high_size;
	unsigned long fixed_low_start, fixed_middle_start, fixed_high_start;

	msm8930_fmem_pdata.size = 0;
	msm8930_fmem_pdata.reserved_size_low = 0;
	msm8930_fmem_pdata.reserved_size_high = 0;
	msm8930_fmem_pdata.align = PAGE_SIZE;
	fixed_low_size = 0;
	fixed_middle_size = 0;
	fixed_high_size = 0;

	/* We only support 1 reusable heap. Check if more than one heap
	 * is specified as reusable and set as non-reusable if found.
	 */
	for (i = 0; i < msm8930_ion_pdata.nr; ++i) {
		const struct ion_platform_heap *heap =
						&(msm8930_ion_pdata.heaps[i]);

		if (heap->type == ION_HEAP_TYPE_CP && heap->extra_data) {
			struct ion_cp_heap_pdata *data = heap->extra_data;

			reusable_count += (data->reusable) ? 1 : 0;

			if (data->reusable && reusable_count > 1) {
				pr_err("%s: Too many heaps specified as "
					"reusable. Heap %s was not configured "
					"as reusable.\n", __func__, heap->name);
				data->reusable = 0;
			}
		}
	}

	for (i = 0; i < msm8930_ion_pdata.nr; ++i) {
		const struct ion_platform_heap *heap =
						&(msm8930_ion_pdata.heaps[i]);

		if (heap->extra_data) {
			int fixed_position = NOT_FIXED;
			int mem_is_fmem = 0;

			switch (heap->type) {
			case ION_HEAP_TYPE_CP:
				mem_is_fmem = ((struct ion_cp_heap_pdata *)
					heap->extra_data)->mem_is_fmem;
				fixed_position = ((struct ion_cp_heap_pdata *)
					heap->extra_data)->fixed_position;
				break;
			case ION_HEAP_TYPE_CARVEOUT:
				mem_is_fmem = ((struct ion_co_heap_pdata *)
					heap->extra_data)->mem_is_fmem;
				fixed_position = ((struct ion_co_heap_pdata *)
					heap->extra_data)->fixed_position;
				break;
			default:
				break;
			}

			if (fixed_position != NOT_FIXED)
				fixed_size += heap->size;
			else
				reserve_mem_for_ion(MEMTYPE_EBI1, heap->size);

			if (fixed_position == FIXED_LOW)
				fixed_low_size += heap->size;
			else if (fixed_position == FIXED_MIDDLE)
				fixed_middle_size += heap->size;
			else if (fixed_position == FIXED_HIGH)
				fixed_high_size += heap->size;

			if (mem_is_fmem)
				msm8930_fmem_pdata.size += heap->size;
		}
	}

	if (!fixed_size)
		return;

	if (msm8930_fmem_pdata.size) {
		msm8930_fmem_pdata.reserved_size_low = fixed_low_size +
							HOLE_SIZE;
		msm8930_fmem_pdata.reserved_size_high = fixed_high_size;
	}

	/* Since the fixed area may be carved out of lowmem,
	 * make sure the length is a multiple of 1M.
	 */
	fixed_size = (fixed_size + HOLE_SIZE + SECTION_SIZE - 1)
		& SECTION_MASK;
	msm8930_reserve_fixed_area(fixed_size);

	fixed_low_start = MSM8930_FIXED_AREA_START;
	fixed_middle_start = fixed_low_start + fixed_low_size + HOLE_SIZE;
	fixed_high_start = fixed_middle_start + fixed_middle_size;

	for (i = 0; i < msm8930_ion_pdata.nr; ++i) {
		struct ion_platform_heap *heap = &(msm8930_ion_pdata.heaps[i]);

		if (heap->extra_data) {
			int fixed_position = NOT_FIXED;
			struct ion_cp_heap_pdata *pdata = NULL;

			switch (heap->type) {
			case ION_HEAP_TYPE_CP:
				pdata =
				(struct ion_cp_heap_pdata *)heap->extra_data;
				fixed_position = pdata->fixed_position;
				break;
			case ION_HEAP_TYPE_CARVEOUT:
				fixed_position = ((struct ion_co_heap_pdata *)
					heap->extra_data)->fixed_position;
				break;
			default:
				break;
			}

			switch (fixed_position) {
			case FIXED_LOW:
				heap->base = fixed_low_start;
				break;
			case FIXED_MIDDLE:
				heap->base = fixed_middle_start;
				pdata->secure_base = fixed_middle_start
							- HOLE_SIZE;
				pdata->secure_size = HOLE_SIZE + heap->size;
				break;
			case FIXED_HIGH:
				heap->base = fixed_high_start;
				break;
			default:
				break;
			}
		}
	}
#endif
}

static void __init reserve_mdp_memory(void)
{
	msm8930_mdp_writeback(msm8930_reserve_table);
}

#ifdef CONFIG_MSM_CACHE_DUMP
static void __init reserve_cache_dump_memory(void)
{
	unsigned int total;

	total = msm8930_cache_dump_pdata.l1_size +
		msm8930_cache_dump_pdata.l2_size;
	msm8930_reserve_table[MEMTYPE_EBI1].size += total;
}
#else
static void __init reserve_cache_dump_memory(void) { }
#endif

static void __init msm8930_calculate_reserve_sizes(void)
{
	size_pmem_devices();
	reserve_pmem_memory();
	reserve_ion_memory();
	reserve_mdp_memory();
	reserve_rtb_memory();
	reserve_cache_dump_memory();
}

static struct reserve_info msm8930_reserve_info __initdata = {
	.memtype_reserve_table = msm8930_reserve_table,
	.calculate_reserve_sizes = msm8930_calculate_reserve_sizes,
	.reserve_fixed_area = msm8930_reserve_fixed_area,
	.paddr_to_memtype = msm8930_paddr_to_memtype,
};

static int msm8930_memory_bank_size(void)
{
	return 1<<29;
}

static void __init locate_unstable_memory(void)
{
	struct membank *mb = &meminfo.bank[meminfo.nr_banks - 1];
	unsigned long bank_size;
	unsigned long low, high;

	bank_size = msm8930_memory_bank_size();
	low = meminfo.bank[0].start;
	high = mb->start + mb->size;

	/* Check if 32 bit overflow occured */
	if (high < mb->start)
		high -= PAGE_SIZE;

	if (high < MAX_FIXED_AREA_SIZE + MSM8930_FIXED_AREA_START)
		panic("fixed area extends beyond end of memory\n");

	low &= ~(bank_size - 1);

	if (high - low <= bank_size)
		goto no_dmm;

	msm8930_reserve_info.bank_size = bank_size;
#ifdef CONFIG_ENABLE_DMM
	msm8930_reserve_info.low_unstable_address = mb->start -
					MIN_MEMORY_BLOCK_SIZE + mb->size;
	msm8930_reserve_info.max_unstable_size = MIN_MEMORY_BLOCK_SIZE;
	pr_info("low unstable address %lx max size %lx bank size %lx\n",
		msm8930_reserve_info.low_unstable_address,
		msm8930_reserve_info.max_unstable_size,
		msm8930_reserve_info.bank_size);
	return;
#endif
no_dmm:
	msm8930_reserve_info.low_unstable_address = high;
	msm8930_reserve_info.max_unstable_size = 0;
}

static void __init place_movable_zone(void)
{
#ifdef CONFIG_ENABLE_DMM
	movable_reserved_start = msm8930_reserve_info.low_unstable_address;
	movable_reserved_size = msm8930_reserve_info.max_unstable_size;
	pr_info("movable zone start %lx size %lx\n",
		movable_reserved_start, movable_reserved_size);
#endif
}

static void __init msm8930_early_memory(void)
{
	reserve_info = &msm8930_reserve_info;
	locate_unstable_memory();
	place_movable_zone();
}

static char prim_panel_name[PANEL_NAME_MAX_LEN];
static char ext_panel_name[PANEL_NAME_MAX_LEN];

static int __init prim_display_setup(char *param)
{
	if (strnlen(param, PANEL_NAME_MAX_LEN))
		strlcpy(prim_panel_name, param, PANEL_NAME_MAX_LEN);
	return 0;
}
early_param("prim_display", prim_display_setup);

static int __init ext_display_setup(char *param)
{
	if (strnlen(param, PANEL_NAME_MAX_LEN))
		strlcpy(ext_panel_name, param, PANEL_NAME_MAX_LEN);
	return 0;
}
early_param("ext_display", ext_display_setup);

/* define size of reserve memory is 1M */
#ifdef CONFIG_SRECORDER_MSM
#define SRECORDER_RESERVED_MEM_SIZE (SZ_1M)

/* define start address of reserve memory */
static unsigned long s_srecorder_reserved_mem_phys_start_addr = 0x0;

/*
 * Function:       unsigned long get_srecorder_reserved_mem_size(void)
 * Description:    get size of reserve memory
 * Return:         SRECORDER_RESERVED_MEM_SIZE: size of reserve memory
 */
unsigned long get_srecorder_reserved_mem_size(void)
{
    return SRECORDER_RESERVED_MEM_SIZE;
}

/*
 * Function:       unsigned long get_mempools_pstart_addr(void)
 * Description:    get start address of reserve memory
 * Return:         s_srecorder_reserved_mem_phys_start_addr:start address of reserve memory
 */
unsigned long get_srecorder_reserved_mem_phys_start_addr(void)
{
    return s_srecorder_reserved_mem_phys_start_addr;
}

extern unsigned long get_mempools_pstart_addr(void);
#endif /* CONFIG_SRECORDER_MSM */

static void __init msm8930_reserve(void)
{
	msm8930_set_display_params(prim_panel_name, ext_panel_name);
	msm_reserve();

#ifdef CONFIG_SRECORDER_MSM
    if (0x0 != get_mempools_pstart_addr())
    {
        s_srecorder_reserved_mem_phys_start_addr = get_mempools_pstart_addr() - 10 * SRECORDER_RESERVED_MEM_SIZE;
        if (0 == memblock_remove(s_srecorder_reserved_mem_phys_start_addr, SRECORDER_RESERVED_MEM_SIZE))
        {
            printk("<<<< Allocate 0x%08lx bytes starting form 0x%08lx for S-Recorder\n", (unsigned long)SRECORDER_RESERVED_MEM_SIZE, 
                s_srecorder_reserved_mem_phys_start_addr);
        }
        else
        {
            s_srecorder_reserved_mem_phys_start_addr = 0x0;
            printk(">>>> No sufficient mem for S-Recorder!\n");
        }
    }
    else
    {
        printk(">>>> Can't know the start address for S-Recorder's reserved memory!\n");
    }
#endif /* CONFIG_SRECORDER_MSM */

	if (msm8930_fmem_pdata.size) {
#if defined(CONFIG_ION_MSM) && defined(CONFIG_MSM_MULTIMEDIA_USE_ION)
		if (reserve_info->fixed_area_size) {
			msm8930_fmem_pdata.phys =
				reserve_info->fixed_area_start + MSM_MM_FW_SIZE;
		pr_info("mm fw at %lx (fixed) size %x\n",
			reserve_info->fixed_area_start, MSM_MM_FW_SIZE);
		pr_info("fmem start %lx (fixed) size %lx\n",
			msm8930_fmem_pdata.phys, msm8930_fmem_pdata.size);
		}
#endif
	}
}

static int msm8930_change_memory_power(u64 start, u64 size,
	int change_type)
{
	return soc_change_memory_power(start, size, change_type);
}

static void __init msm8930_allocate_memory_regions(void)
{
	msm8930_allocate_fb_region();
}

#ifdef CONFIG_WCD9304_CODEC

#define SITAR_INTERRUPT_BASE (NR_MSM_IRQS + NR_GPIO_IRQS + NR_PM8921_IRQS)

/* Micbias setting is based on 8660 CDP/MTP/FLUID requirement
 * 4 micbiases are used to power various analog and digital
 * microphones operating at 1800 mV. Technically, all micbiases
 * can source from single cfilter since all microphones operate
 * at the same voltage level. The arrangement below is to make
 * sure all cfilters are exercised. LDO_H regulator ouput level
 * does not need to be as high as 2.85V. It is choosen for
 * microphone sensitivity purpose.
 */
static struct wcd9xxx_pdata sitar_platform_data = {
		.slimbus_slave_device = {
		.name = "sitar-slave",
		.e_addr = {0, 0, 0x00, 0, 0x17, 2},
	},
	.irq = MSM_GPIO_TO_INT(62),
	.irq_base = SITAR_INTERRUPT_BASE,
	.num_irqs = NR_WCD9XXX_IRQS,
	.reset_gpio = 42,
	.micbias = {
		.ldoh_v = SITAR_LDOH_2P85_V,
		.cfilt1_mv = 1800,
		.cfilt2_mv = 1800,
		.bias1_cfilt_sel = SITAR_CFILT1_SEL,
		.bias2_cfilt_sel = SITAR_CFILT2_SEL,
		.bias1_cap_mode = MICBIAS_EXT_BYP_CAP,
		.bias2_cap_mode = MICBIAS_NO_EXT_BYP_CAP,
	},
	.regulator = {
	{
		.name = "CDC_VDD_CP",
		.min_uV = 2200000,
		.max_uV = 2200000,
		.optimum_uA = WCD9XXX_CDC_VDDA_CP_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_RX",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_RX_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_TX",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_TX_CUR_MAX,
	},
	{
		.name = "VDDIO_CDC",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_VDDIO_CDC_CUR_MAX,
	},
	{
		.name = "VDDD_CDC_D",
		.min_uV = 1200000,
		.max_uV = 1250000,
		.optimum_uA = WCD9XXX_VDDD_CDC_D_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_A_1P2V",
		.min_uV = 1200000,
		.max_uV = 1250000,
		.optimum_uA = WCD9XXX_VDDD_CDC_A_CUR_MAX,
	},
	},
};

static struct slim_device msm_slim_sitar = {
	.name = "sitar-slim",
	.e_addr = {0, 1, 0x00, 0, 0x17, 2},
	.dev = {
	.platform_data = &sitar_platform_data,
	},
};

static struct wcd9xxx_pdata sitar1p1_platform_data = {
		.slimbus_slave_device = {
		.name = "sitar-slave",
		.e_addr = {0, 0, 0x70, 0, 0x17, 2},
	},
	.irq = MSM_GPIO_TO_INT(62),
	.irq_base = SITAR_INTERRUPT_BASE,
	.num_irqs = NR_WCD9XXX_IRQS,
	.reset_gpio = 42,
	.micbias = {
		.ldoh_v = SITAR_LDOH_2P85_V,
		.cfilt1_mv = 1800,
		.cfilt2_mv = 1800,
		.bias1_cfilt_sel = SITAR_CFILT1_SEL,
		.bias2_cfilt_sel = SITAR_CFILT2_SEL,
		.bias1_cap_mode = MICBIAS_NO_EXT_BYP_CAP,
		.bias2_cap_mode = MICBIAS_NO_EXT_BYP_CAP,
	},
	.regulator = {
	{
		.name = "CDC_VDD_CP",
		.min_uV = 2200000,
		.max_uV = 2200000,
		.optimum_uA = WCD9XXX_CDC_VDDA_CP_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_RX",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_RX_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_TX",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_CDC_VDDA_TX_CUR_MAX,
	},
	{
		.name = "VDDIO_CDC",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.optimum_uA = WCD9XXX_VDDIO_CDC_CUR_MAX,
	},
	{
		.name = "VDDD_CDC_D",
		.min_uV = 1200000,
		.max_uV = 1250000,
		.optimum_uA = WCD9XXX_VDDD_CDC_D_CUR_MAX,
	},
	{
		.name = "CDC_VDDA_A_1P2V",
		.min_uV = 1200000,
		.max_uV = 1250000,
		.optimum_uA = WCD9XXX_VDDD_CDC_A_CUR_MAX,
	},
	},
};

static struct slim_device msm_slim_sitar1p1 = {
	.name = "sitar1p1-slim",
	.e_addr = {0, 1, 0x70, 0, 0x17, 2},
	.dev = {
	.platform_data = &sitar1p1_platform_data,
	},
};
#endif


static struct slim_boardinfo msm_slim_devices[] = {
#ifdef CONFIG_WCD9304_CODEC
	{
		.bus_num = 1,
		.slim_slave = &msm_slim_sitar,
	},
	{
		.bus_num = 1,
		.slim_slave = &msm_slim_sitar1p1,
	},
#endif
	/* add more slimbus slaves as needed */
};

#define MSM_WCNSS_PHYS	0x03000000
#define MSM_WCNSS_SIZE	0x280000

static struct resource resources_wcnss_wlan[] = {
	{
		.start	= RIVA_APPS_WLAN_RX_DATA_AVAIL_IRQ,
		.end	= RIVA_APPS_WLAN_RX_DATA_AVAIL_IRQ,
		.name	= "wcnss_wlanrx_irq",
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= RIVA_APPS_WLAN_DATA_XFER_DONE_IRQ,
		.end	= RIVA_APPS_WLAN_DATA_XFER_DONE_IRQ,
		.name	= "wcnss_wlantx_irq",
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= MSM_WCNSS_PHYS,
		.end	= MSM_WCNSS_PHYS + MSM_WCNSS_SIZE - 1,
		.name	= "wcnss_mmio",
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= 84,
		.end	= 88,
		.name	= "wcnss_gpios_5wire",
		.flags	= IORESOURCE_IO,
	},
};

static struct qcom_wcnss_opts qcom_wcnss_pdata = {
	.has_48mhz_xo	= 1,
};

static struct platform_device msm_device_wcnss_wlan = {
	.name		= "wcnss_wlan",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(resources_wcnss_wlan),
	.resource	= resources_wcnss_wlan,
	.dev		= {.platform_data = &qcom_wcnss_pdata},
};

#ifdef CONFIG_QSEECOM
/* qseecom bus scaling */
static struct msm_bus_vectors qseecom_clks_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ib = 0,
		.ab = 0,
	},
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_SPS,
		.ib = 0,
		.ab = 0,
	},
	{
		.src = MSM_BUS_MASTER_SPDM,
		.dst = MSM_BUS_SLAVE_SPDM,
		.ib = 0,
		.ab = 0,
	},
};

static struct msm_bus_vectors qseecom_enable_dfab_vectors[] = {
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ib = (492 * 8) * 1000000UL,
		.ab = (492 * 8) *  100000UL,
	},
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_SPS,
		.ib = (492 * 8) * 1000000UL,
		.ab = (492 * 8) *  100000UL,
	},
	{
		.src = MSM_BUS_MASTER_SPDM,
		.dst = MSM_BUS_SLAVE_SPDM,
		.ib = 0,
		.ab = 0,
	},
};

static struct msm_bus_vectors qseecom_enable_sfpb_vectors[] = {
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ib = 0,
		.ab = 0,
	},
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_SPS,
		.ib = 0,
		.ab = 0,
	},
	{
		.src = MSM_BUS_MASTER_SPDM,
		.dst = MSM_BUS_SLAVE_SPDM,
		.ib = (64 * 8) * 1000000UL,
		.ab = (64 * 8) *  100000UL,
	},
};

static struct msm_bus_vectors qseecom_enable_dfab_sfpb_vectors[] = {
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ib = (492 * 8) * 1000000UL,
		.ab = (492 * 8) *  100000UL,
	},
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_SPS,
		.ib = (492 * 8) * 1000000UL,
		.ab = (492 * 8) *  100000UL,
	},
	{
		.src = MSM_BUS_MASTER_SPDM,
		.dst = MSM_BUS_SLAVE_SPDM,
		.ib = (64 * 8) * 1000000UL,
		.ab = (64 * 8) *  100000UL,
	},
};

static struct msm_bus_paths qseecom_hw_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(qseecom_clks_init_vectors),
		qseecom_clks_init_vectors,
	},
	{
		ARRAY_SIZE(qseecom_enable_dfab_vectors),
		qseecom_enable_sfpb_vectors,
	},
	{
		ARRAY_SIZE(qseecom_enable_sfpb_vectors),
		qseecom_enable_sfpb_vectors,
	},
	{
		ARRAY_SIZE(qseecom_enable_dfab_sfpb_vectors),
		qseecom_enable_dfab_sfpb_vectors,
	},
};

static struct msm_bus_scale_pdata qseecom_bus_pdata = {
	qseecom_hw_bus_scale_usecases,
	ARRAY_SIZE(qseecom_hw_bus_scale_usecases),
	.name = "qsee",
};

static struct platform_device qseecom_device = {
	.name		= "qseecom",
	.id		= 0,
	.dev		= {
		.platform_data = &qseecom_bus_pdata,
	},
};
#endif

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)

#define QCE_SIZE		0x10000
#define QCE_0_BASE		0x18500000

#define QCE_HW_KEY_SUPPORT	0
#define QCE_SHA_HMAC_SUPPORT	1
#define QCE_SHARE_CE_RESOURCE	1
#define QCE_CE_SHARED		0

/* Begin Bus scaling definitions */
static struct msm_bus_vectors crypto_hw_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_ADM_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
	{
		.src = MSM_BUS_MASTER_ADM_PORT1,
		.dst = MSM_BUS_SLAVE_GSBI1_UART,
		.ab = 0,
		.ib = 0,
	},
};

static struct msm_bus_vectors crypto_hw_active_vectors[] = {
	{
		.src = MSM_BUS_MASTER_ADM_PORT0,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 70000000UL,
		.ib = 70000000UL,
	},
	{
		.src = MSM_BUS_MASTER_ADM_PORT1,
		.dst = MSM_BUS_SLAVE_GSBI1_UART,
		.ab = 2480000000UL,
		.ib = 2480000000UL,
	},
};

static struct msm_bus_paths crypto_hw_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(crypto_hw_init_vectors),
		crypto_hw_init_vectors,
	},
	{
		ARRAY_SIZE(crypto_hw_active_vectors),
		crypto_hw_active_vectors,
	},
};

static struct msm_bus_scale_pdata crypto_hw_bus_scale_pdata = {
		crypto_hw_bus_scale_usecases,
		ARRAY_SIZE(crypto_hw_bus_scale_usecases),
		.name = "cryptohw",
};
/* End Bus Scaling Definitions*/

static struct resource qcrypto_resources[] = {
	[0] = {
		.start = QCE_0_BASE,
		.end = QCE_0_BASE + QCE_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.name = "crypto_channels",
		.start = DMOV_CE_IN_CHAN,
		.end = DMOV_CE_OUT_CHAN,
		.flags = IORESOURCE_DMA,
	},
	[2] = {
		.name = "crypto_crci_in",
		.start = DMOV_CE_IN_CRCI,
		.end = DMOV_CE_IN_CRCI,
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.name = "crypto_crci_out",
		.start = DMOV_CE_OUT_CRCI,
		.end = DMOV_CE_OUT_CRCI,
		.flags = IORESOURCE_DMA,
	},
};

static struct resource qcedev_resources[] = {
	[0] = {
		.start = QCE_0_BASE,
		.end = QCE_0_BASE + QCE_SIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.name = "crypto_channels",
		.start = DMOV_CE_IN_CHAN,
		.end = DMOV_CE_OUT_CHAN,
		.flags = IORESOURCE_DMA,
	},
	[2] = {
		.name = "crypto_crci_in",
		.start = DMOV_CE_IN_CRCI,
		.end = DMOV_CE_IN_CRCI,
		.flags = IORESOURCE_DMA,
	},
	[3] = {
		.name = "crypto_crci_out",
		.start = DMOV_CE_OUT_CRCI,
		.end = DMOV_CE_OUT_CRCI,
		.flags = IORESOURCE_DMA,
	},
};

#endif

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE)

static struct msm_ce_hw_support qcrypto_ce_hw_suppport = {
	.ce_shared = QCE_CE_SHARED,
	.shared_ce_resource = QCE_SHARE_CE_RESOURCE,
	.hw_key_support = QCE_HW_KEY_SUPPORT,
	.sha_hmac = QCE_SHA_HMAC_SUPPORT,
	.bus_scale_table = &crypto_hw_bus_scale_pdata,
};

static struct platform_device qcrypto_device = {
	.name		= "qcrypto",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(qcrypto_resources),
	.resource	= qcrypto_resources,
	.dev		= {
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = &qcrypto_ce_hw_suppport,
	},
};
#endif

#if defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)

static struct msm_ce_hw_support qcedev_ce_hw_suppport = {
	.ce_shared = QCE_CE_SHARED,
	.shared_ce_resource = QCE_SHARE_CE_RESOURCE,
	.hw_key_support = QCE_HW_KEY_SUPPORT,
	.sha_hmac = QCE_SHA_HMAC_SUPPORT,
	.bus_scale_table = &crypto_hw_bus_scale_pdata,
};

static struct platform_device qcedev_device = {
	.name		= "qce",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(qcedev_resources),
	.resource	= qcedev_resources,
	.dev		= {
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = &qcedev_ce_hw_suppport,
	},
};
#endif

#define MDM2AP_ERRFATAL			70
#define AP2MDM_ERRFATAL			95
#define MDM2AP_STATUS			69
#define AP2MDM_STATUS			94
#define AP2MDM_PMIC_RESET_N		80
#define AP2MDM_KPDPWR_N			81

static struct resource mdm_resources[] = {
	{
		.start	= MDM2AP_ERRFATAL,
		.end	= MDM2AP_ERRFATAL,
		.name	= "MDM2AP_ERRFATAL",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= AP2MDM_ERRFATAL,
		.end	= AP2MDM_ERRFATAL,
		.name	= "AP2MDM_ERRFATAL",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= MDM2AP_STATUS,
		.end	= MDM2AP_STATUS,
		.name	= "MDM2AP_STATUS",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= AP2MDM_STATUS,
		.end	= AP2MDM_STATUS,
		.name	= "AP2MDM_STATUS",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= AP2MDM_PMIC_RESET_N,
		.end	= AP2MDM_PMIC_RESET_N,
		.name	= "AP2MDM_PMIC_RESET_N",
		.flags	= IORESOURCE_IO,
	},
	{
		.start	= AP2MDM_KPDPWR_N,
		.end	= AP2MDM_KPDPWR_N,
		.name	= "AP2MDM_KPDPWR_N",
		.flags	= IORESOURCE_IO,
	},
};

static struct mdm_platform_data mdm_platform_data = {
	.mdm_version = "2.5",
};

static struct platform_device mdm_device = {
	.name		= "mdm2_modem",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(mdm_resources),
	.resource	= mdm_resources,
	.dev		= {
		.platform_data = &mdm_platform_data,
	},
};

static struct platform_device *mdm_devices[] __initdata = {
	&mdm_device,
};

#ifdef CONFIG_MSM_MPM
static uint16_t msm_mpm_irqs_m2a[MSM_MPM_NR_MPM_IRQS] __initdata = {
	[1] = MSM_GPIO_TO_INT(46),
	[2] = MSM_GPIO_TO_INT(150),
	[4] = MSM_GPIO_TO_INT(103),
	[5] = MSM_GPIO_TO_INT(104),
	[6] = MSM_GPIO_TO_INT(105),
	[7] = MSM_GPIO_TO_INT(106),
	[8] = MSM_GPIO_TO_INT(107),
	[9] = MSM_GPIO_TO_INT(7),
	[10] = MSM_GPIO_TO_INT(11),
	[11] = MSM_GPIO_TO_INT(15),
	[12] = MSM_GPIO_TO_INT(19),
	[13] = MSM_GPIO_TO_INT(23),
	[14] = MSM_GPIO_TO_INT(27),
	[15] = MSM_GPIO_TO_INT(31),
	[16] = MSM_GPIO_TO_INT(35),
	[19] = MSM_GPIO_TO_INT(90),
	[20] = MSM_GPIO_TO_INT(92),
	[23] = MSM_GPIO_TO_INT(85),
	[24] = MSM_GPIO_TO_INT(83),
	[25] = USB1_HS_IRQ,
	[26] = MSM_GPIO_TO_INT(6),
	[27] = HDMI_IRQ,
	[29] = MSM_GPIO_TO_INT(10),
	[30] = MSM_GPIO_TO_INT(102),
	[31] = MSM_GPIO_TO_INT(81),
	[32] = MSM_GPIO_TO_INT(78),
	[33] = MSM_GPIO_TO_INT(94),
	[34] = MSM_GPIO_TO_INT(72),
	[35] = MSM_GPIO_TO_INT(39),
	[36] = MSM_GPIO_TO_INT(43),
	[37] = MSM_GPIO_TO_INT(61),
	[38] = MSM_GPIO_TO_INT(50),
	[39] = MSM_GPIO_TO_INT(42),
	[41] = MSM_GPIO_TO_INT(62),
	[42] = MSM_GPIO_TO_INT(8),
	[43] = MSM_GPIO_TO_INT(33),
	[44] = MSM_GPIO_TO_INT(70),
	[45] = MSM_GPIO_TO_INT(69),
	[46] = MSM_GPIO_TO_INT(67),
	[47] = MSM_GPIO_TO_INT(65),
	[48] = MSM_GPIO_TO_INT(55),
	[49] = MSM_GPIO_TO_INT(74),
	[50] = MSM_GPIO_TO_INT(98),
	[51] = MSM_GPIO_TO_INT(49),
	[52] = MSM_GPIO_TO_INT(40),
	[53] = MSM_GPIO_TO_INT(37),
	[54] = MSM_GPIO_TO_INT(24),
	[55] = MSM_GPIO_TO_INT(14),
};

static uint16_t msm_mpm_bypassed_apps_irqs[] __initdata = {
	TLMM_MSM_SUMMARY_IRQ,
	RPM_APCC_CPU0_GP_HIGH_IRQ,
	RPM_APCC_CPU0_GP_MEDIUM_IRQ,
	RPM_APCC_CPU0_GP_LOW_IRQ,
	RPM_APCC_CPU0_WAKE_UP_IRQ,
	RPM_APCC_CPU1_GP_HIGH_IRQ,
	RPM_APCC_CPU1_GP_MEDIUM_IRQ,
	RPM_APCC_CPU1_GP_LOW_IRQ,
	RPM_APCC_CPU1_WAKE_UP_IRQ,
	MSS_TO_APPS_IRQ_0,
	MSS_TO_APPS_IRQ_1,
	MSS_TO_APPS_IRQ_2,
	MSS_TO_APPS_IRQ_3,
	MSS_TO_APPS_IRQ_4,
	MSS_TO_APPS_IRQ_5,
	MSS_TO_APPS_IRQ_6,
	MSS_TO_APPS_IRQ_7,
	MSS_TO_APPS_IRQ_8,
	MSS_TO_APPS_IRQ_9,
	LPASS_SCSS_GP_LOW_IRQ,
	LPASS_SCSS_GP_MEDIUM_IRQ,
	LPASS_SCSS_GP_HIGH_IRQ,
	SPS_MTI_30,
	SPS_MTI_31,
	RIVA_APSS_SPARE_IRQ,
	RIVA_APPS_WLAN_SMSM_IRQ,
	RIVA_APPS_WLAN_RX_DATA_AVAIL_IRQ,
	RIVA_APPS_WLAN_DATA_XFER_DONE_IRQ,
};

struct msm_mpm_device_data msm8930_mpm_dev_data __initdata = {
	.irqs_m2a = msm_mpm_irqs_m2a,
	.irqs_m2a_size = ARRAY_SIZE(msm_mpm_irqs_m2a),
	.bypassed_apps_irqs = msm_mpm_bypassed_apps_irqs,
	.bypassed_apps_irqs_size = ARRAY_SIZE(msm_mpm_bypassed_apps_irqs),
	.mpm_request_reg_base = MSM_RPM_BASE + 0x9d8,
	.mpm_status_reg_base = MSM_RPM_BASE + 0xdf8,
	.mpm_apps_ipc_reg = MSM_APCS_GCC_BASE + 0x008,
	.mpm_apps_ipc_val =  BIT(1),
	.mpm_ipc_irq = RPM_APCC_CPU0_GP_MEDIUM_IRQ,

};
#endif

#define MSM_SHARED_RAM_PHYS 0x80000000

static void __init msm8930_map_io(void)
{
	msm_shared_ram_phys = MSM_SHARED_RAM_PHYS;
	msm_map_msm8930_io();

	if (socinfo_init() < 0)
		pr_err("socinfo_init() failed!\n");
}

static void __init msm8930_init_irq(void)
{
	struct msm_mpm_device_data *data = NULL;
#ifdef CONFIG_MSM_MPM
	data = &msm8930_mpm_dev_data;
#endif

	msm_mpm_irq_extn_init(data);
	gic_init(0, GIC_PPI_START, MSM_QGIC_DIST_BASE,
						(void *)MSM_QGIC_CPU_BASE);
}

static void __init msm8930_init_buses(void)
{
#ifdef CONFIG_MSM_BUS_SCALING
	msm_bus_rpm_set_mt_mask();
	msm_bus_8930_apps_fabric_pdata.rpm_enabled = 1;
	msm_bus_8930_sys_fabric_pdata.rpm_enabled = 1;
	msm_bus_8930_mm_fabric_pdata.rpm_enabled = 1;
	msm_bus_8930_apps_fabric.dev.platform_data =
		&msm_bus_8930_apps_fabric_pdata;
	msm_bus_8930_sys_fabric.dev.platform_data =
		&msm_bus_8930_sys_fabric_pdata;
	msm_bus_8930_mm_fabric.dev.platform_data =
		&msm_bus_8930_mm_fabric_pdata;
	msm_bus_8930_sys_fpb.dev.platform_data = &msm_bus_8930_sys_fpb_pdata;
	msm_bus_8930_cpss_fpb.dev.platform_data = &msm_bus_8930_cpss_fpb_pdata;
#endif
}

static struct msm_spi_platform_data msm8960_qup_spi_gsbi1_pdata = {
	.max_clock_speed = 15060000,
};

#ifdef CONFIG_USB_MSM_OTG_72K
static struct msm_otg_platform_data msm_otg_pdata;
#else
static int enable_usb_host_mode;
static int __init usb_host_mode_with_pm8917(char *param)
{
	int ret;

	ret = kstrtoint(param, 10, &enable_usb_host_mode);
	return ret;
}
early_param("usb_host_mode_pm8917", usb_host_mode_with_pm8917);

#ifdef CONFIG_MSM_BUS_SCALING
/* Bandwidth requests (zero) if no vote placed */
static struct msm_bus_vectors usb_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 0,
		.ib = 0,
	},
};

/* Bus bandwidth requests in Bytes/sec */
static struct msm_bus_vectors usb_max_vectors[] = {
	{
		.src = MSM_BUS_MASTER_SPS,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab = 60000000,		/* At least 480Mbps on bus. */
		.ib = 960000000,	/* MAX bursts rate */
	},
};

static struct msm_bus_paths usb_bus_scale_usecases[] = {
	{
		ARRAY_SIZE(usb_init_vectors),
		usb_init_vectors,
	},
	{
		ARRAY_SIZE(usb_max_vectors),
		usb_max_vectors,
	},
};

static struct msm_bus_scale_pdata usb_bus_scale_pdata = {
	usb_bus_scale_usecases,
	ARRAY_SIZE(usb_bus_scale_usecases),
	.name = "usb",
};
#endif

static int hsusb_phy_init_seq[] = {
	0x44, 0x80, /* set VBUS valid threshold
			and disconnect valid threshold */
	0x68, 0x81, /* update DC voltage level */
	0x24, 0x82, /* set preemphasis and rise/fall time */
	0x13, 0x83, /* set source impedance adjusment */
	-1};

#define MSM_MPM_PIN_USB1_OTGSESSVLD	40

static struct msm_otg_platform_data msm_otg_pdata = {
	.mode			= USB_OTG,
	.otg_control		= OTG_PMIC_CONTROL,
	.phy_type		= SNPS_28NM_INTEGRATED_PHY,
	.power_budget		= 750,
#ifdef CONFIG_MSM_BUS_SCALING
	.bus_scale_table	= &usb_bus_scale_pdata,
#endif
#ifdef CONFIG_FB_MSM_HDMI_MHL_8334
	.mhl_dev_name		= "sii8334",
#endif
	.mpm_otgsessvld_int	= MSM_MPM_PIN_USB1_OTGSESSVLD,
};
#endif

#define PID_MAGIC_ID		0x71432909
#define SERIAL_NUM_MAGIC_ID	0x61945374
#define SERIAL_NUMBER_LENGTH	127
#define DLOAD_USB_BASE_ADD	0x2A03F0C8

struct magic_num_struct {
	uint32_t pid;
	uint32_t serial_num;
};

struct dload_struct {
	uint32_t	reserved1;
	uint32_t	reserved2;
	uint32_t	reserved3;
	uint16_t	reserved4;
	uint16_t	pid;
	char		serial_number[SERIAL_NUMBER_LENGTH];
	uint16_t	reserved5;
	struct magic_num_struct magic_struct;
};

static int usb_diag_update_pid_and_serial_num(uint32_t pid, const char *snum)
{
	struct dload_struct __iomem *dload = 0;

	dload = ioremap(DLOAD_USB_BASE_ADD, sizeof(*dload));
	if (!dload) {
		pr_err("%s: cannot remap I/O memory region: %08x\n",
					__func__, DLOAD_USB_BASE_ADD);
		return -ENXIO;
	}

	pr_debug("%s: dload:%p pid:%x serial_num:%s\n",
				__func__, dload, pid, snum);
	/* update pid */
	dload->magic_struct.pid = PID_MAGIC_ID;
	dload->pid = pid;

	/* update serial number */
	dload->magic_struct.serial_num = 0;
	if (!snum) {
		memset(dload->serial_number, 0, SERIAL_NUMBER_LENGTH);
		goto out;
	}

	dload->magic_struct.serial_num = SERIAL_NUM_MAGIC_ID;
	strlcpy(dload->serial_number, snum, SERIAL_NUMBER_LENGTH);
out:
	iounmap(dload);
	return 0;
}

static struct android_usb_platform_data android_usb_pdata = {
	.update_pid_and_serial_num = usb_diag_update_pid_and_serial_num,
};

static struct platform_device android_usb_device = {
	.name	= "android_usb",
	.id	= -1,
	.dev	= {
		.platform_data = &android_usb_pdata,
	},
};

static uint8_t spm_wfi_cmd_sequence[] __initdata = {
	0x03, 0x0f,
};

static uint8_t spm_retention_cmd_sequence[] __initdata = {
	0x00, 0x05, 0x03, 0x0D,
	0x0B, 0x00, 0x0f,
};

static uint8_t spm_power_collapse_without_rpm[] __initdata = {
	0x00, 0x24, 0x54, 0x10,
	0x09, 0x03, 0x01,
	0x10, 0x54, 0x30, 0x0C,
	0x24, 0x30, 0x0f,
};

static uint8_t spm_power_collapse_with_rpm[] __initdata = {
	0x00, 0x24, 0x54, 0x10,
	0x09, 0x07, 0x01, 0x0B,
	0x10, 0x54, 0x30, 0x0C,
	0x24, 0x30, 0x0f,
};

static struct msm_spm_seq_entry msm_spm_boot_cpu_seq_list[] __initdata = {
	[0] = {
		.mode = MSM_SPM_MODE_CLOCK_GATING,
		.notify_rpm = false,
		.cmd = spm_wfi_cmd_sequence,
	},
	[1] = {
		.mode = MSM_SPM_MODE_POWER_RETENTION,
		.notify_rpm = false,
		.cmd = spm_retention_cmd_sequence,
	},
	[2] = {
		.mode = MSM_SPM_MODE_POWER_COLLAPSE,
		.notify_rpm = false,
		.cmd = spm_power_collapse_without_rpm,
	},
	[3] = {
		.mode = MSM_SPM_MODE_POWER_COLLAPSE,
		.notify_rpm = true,
		.cmd = spm_power_collapse_with_rpm,
	},
};

static struct msm_spm_seq_entry msm_spm_nonboot_cpu_seq_list[] __initdata = {
	[0] = {
		.mode = MSM_SPM_MODE_CLOCK_GATING,
		.notify_rpm = false,
		.cmd = spm_wfi_cmd_sequence,
	},
	[1] = {
		.mode = MSM_SPM_MODE_POWER_COLLAPSE,
		.notify_rpm = false,
		.cmd = spm_power_collapse_without_rpm,
	},
	[2] = {
		.mode = MSM_SPM_MODE_POWER_COLLAPSE,
		.notify_rpm = true,
		.cmd = spm_power_collapse_with_rpm,
	},
};

static struct msm_spm_platform_data msm_spm_data[] __initdata = {
	[0] = {
		.reg_base_addr = MSM_SAW0_BASE,
		.reg_init_values[MSM_SPM_REG_SAW2_CFG] = 0x1F,
#if defined(CONFIG_MSM_AVS_HW)
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_CTL] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_HYSTERESIS] = 0x00,
#endif
		.reg_init_values[MSM_SPM_REG_SAW2_SPM_CTL] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DLY] = 0x03020004,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_0] = 0x0084009C,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_1] = 0x00A4001C,
		.vctl_timeout_us = 50,
		.num_modes = ARRAY_SIZE(msm_spm_boot_cpu_seq_list),
		.modes = msm_spm_boot_cpu_seq_list,
	},
	[1] = {
		.reg_base_addr = MSM_SAW1_BASE,
		.reg_init_values[MSM_SPM_REG_SAW2_CFG] = 0x1F,
#if defined(CONFIG_MSM_AVS_HW)
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_CTL] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW2_AVS_HYSTERESIS] = 0x00,
#endif
		.reg_init_values[MSM_SPM_REG_SAW2_SPM_CTL] = 0x01,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DLY] = 0x02020204,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_0] = 0x0060009C,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_1] = 0x0000001C,
		.vctl_timeout_us = 50,
		.num_modes = ARRAY_SIZE(msm_spm_nonboot_cpu_seq_list),
		.modes = msm_spm_nonboot_cpu_seq_list,
	},
};

static uint8_t l2_spm_wfi_cmd_sequence[] __initdata = {
	0x00, 0x20, 0x03, 0x20,
	0x00, 0x0f,
};

static uint8_t l2_spm_gdhs_cmd_sequence[] __initdata = {
	0x00, 0x20, 0x34, 0x64,
	0x48, 0x07, 0x48, 0x20,
	0x50, 0x64, 0x04, 0x34,
	0x50, 0x0f,
};
static uint8_t l2_spm_power_off_cmd_sequence[] __initdata = {
	0x00, 0x10, 0x34, 0x64,
	0x48, 0x07, 0x48, 0x10,
	0x50, 0x64, 0x04, 0x34,
	0x50, 0x0F,
};

static struct msm_spm_seq_entry msm_spm_l2_seq_list[] __initdata = {
	[0] = {
		.mode = MSM_SPM_L2_MODE_RETENTION,
		.notify_rpm = false,
		.cmd = l2_spm_wfi_cmd_sequence,
	},
	[1] = {
		.mode = MSM_SPM_L2_MODE_GDHS,
		.notify_rpm = true,
		.cmd = l2_spm_gdhs_cmd_sequence,
	},
	[2] = {
		.mode = MSM_SPM_L2_MODE_POWER_COLLAPSE,
		.notify_rpm = true,
		.cmd = l2_spm_power_off_cmd_sequence,
	},
};

static struct msm_spm_platform_data msm_spm_l2_data[] __initdata = {
	[0] = {
		.reg_base_addr = MSM_SAW_L2_BASE,
		.reg_init_values[MSM_SPM_REG_SAW2_SPM_CTL] = 0x00,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DLY] = 0x02020204,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_0] = 0x00A000AE,
		.reg_init_values[MSM_SPM_REG_SAW2_PMIC_DATA_1] = 0x00A00020,
		.modes = msm_spm_l2_seq_list,
		.num_modes = ARRAY_SIZE(msm_spm_l2_seq_list),
	},
};

#define ISA1200_HAP_EN_GPIO	77
#define ISA1200_HAP_LEN_GPIO	78
#define ISA1200_HAP_CLK_PM8038	PM8038_GPIO_PM_TO_SYS(7)
#define ISA1200_HAP_CLK_PM8917	PM8917_GPIO_PM_TO_SYS(38)

static int isa1200_power(int on)
{
	unsigned int gpio = ISA1200_HAP_CLK_PM8038;
	enum pm8xxx_aux_clk_id clk_id = CLK_MP3_1;
	int rc = 0;

	if (socinfo_get_pmic_model() == PMIC_MODEL_PM8917) {
		gpio = ISA1200_HAP_CLK_PM8917;
		clk_id = CLK_MP3_2;
	}

	gpio_set_value_cansleep(gpio, !!on);

	if (on)
		rc = pm8xxx_aux_clk_control(clk_id, XO_DIV_1, true);
	else
		rc = pm8xxx_aux_clk_control(clk_id, XO_DIV_NONE, true);

	if (rc) {
		pr_err("%s: unable to write aux clock register(%d)\n",
			__func__, rc);
	}

	return rc;
}

static int isa1200_dev_setup(bool enable)
{
	unsigned int gpio = ISA1200_HAP_CLK_PM8038;
	int rc = 0;

	if (socinfo_get_pmic_model() == PMIC_MODEL_PM8917)
		gpio = ISA1200_HAP_CLK_PM8917;

	if (!enable)
		goto fail_gpio_dir;

	rc = gpio_request(gpio, "haptics_clk");
	if (rc) {
		pr_err("%s: gpio_request for %d gpio failed rc(%d)\n",
					__func__, gpio, rc);
		goto fail_gpio_req;
	}

	rc = gpio_direction_output(gpio, 0);
	if (rc) {
		pr_err("%s: gpio_direction_output failed for %d gpio rc(%d)\n",
						__func__, gpio, rc);
		goto fail_gpio_dir;
	}

	return 0;

fail_gpio_dir:
	gpio_free(gpio);
fail_gpio_req:
	return rc;

}

static struct isa1200_regulator isa1200_reg_data[] = {
	{
		.name = "vddp",
		.min_uV = ISA_I2C_VTG_MIN_UV,
		.max_uV = ISA_I2C_VTG_MAX_UV,
		.load_uA = ISA_I2C_CURR_UA,
	},
	{
		.name = "vcc_i2c",
		.min_uV = ISA_I2C_VTG_MIN_UV,
		.max_uV = ISA_I2C_VTG_MAX_UV,
		.load_uA = ISA_I2C_CURR_UA,
	},
};

static struct isa1200_platform_data isa1200_1_pdata = {
	.name = "vibrator",
	.dev_setup = isa1200_dev_setup,
	.power_on = isa1200_power,
	.hap_en_gpio = ISA1200_HAP_EN_GPIO,
	.hap_len_gpio = ISA1200_HAP_LEN_GPIO,
	.max_timeout = 15000,
	.mode_ctrl = PWM_GEN_MODE,
	.pwm_fd = {
		.pwm_div = 256,
	},
	.is_erm = false,
	.smart_en = true,
	.ext_clk_en = true,
	.chip_en = 1,
	.regulator_info = isa1200_reg_data,
	.num_regulators = ARRAY_SIZE(isa1200_reg_data),
};

static struct i2c_board_info msm_isa1200_board_info[] __initdata = {
	{
		I2C_BOARD_INFO("isa1200_1", 0x90>>1),
		.platform_data = &isa1200_1_pdata,
	},
};

#define MXT_TS_GPIO_IRQ			11
#define MXT_TS_RESET_GPIO		52

static const u8 mxt_config_data_8930_v1[] = {
	/* T6 Object */
	 0, 0, 0, 0, 0, 0,
	/* T38 Object */
	 15, 3, 0, 15, 12, 11, 0, 0,
	/* T7 Object */
	32, 16, 50,
	/* T8 Object */
	 30, 0, 5, 1, 0, 0, 8, 8, 0, 0,
	/* T9 Object */
	 131, 0, 0, 19, 11, 0, 16, 43, 2, 3,
	 10, 7, 2, 0, 4, 5, 35, 10, 43, 4,
	 54, 2, 15, 32, 38, 38, 143, 40, 143, 80,
	 7, 9, 50, 50, 2,
	/* T15 Object */
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0,
	/* T18 Object */
	 0, 0,
	/* T19 Object */
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0,
	/* T23 Object */
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0,
	/* T25 Object */
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0,
	/* T40 Object */
	 0, 0, 0, 0, 0,
	/* T42 Object */
	 0, 0, 0, 0, 0, 0, 0, 0,
	/* T46 Object */
	 0, 3, 8, 16, 0, 0, 1, 0, 0,
	/* T47 Object */
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* T48 Object */
	 0, 0, 8, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 100, 4, 64,
	 0, 0, 5, 42, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0,
};

static const u8 mxt_config_data_8930_v2[] = {
	/* T6 Object */
	 0, 0, 0, 0, 0, 0,
	/* T38 Object */
	 15, 4, 0, 9, 7, 12, 0, 0,
	/* T7 Object */
	32, 16, 50,
	/* T8 Object */
	 30, 0, 5, 10, 0, 0, 10, 10, 0, 0,
	/* T9 Object */
	 131, 0, 0, 19, 11, 0, 16, 50, 1, 3,
	 12, 7, 2, 0, 4, 5, 2, 10, 43, 4,
	 54, 2, -25, 29, 38, 18, 143, 40, 207, 80,
	 17, 5, 50, 50, 0,
	/* T18 Object */
	 0, 0,
	/* T19 Object */
	 0, 0, 0, 0, 0, 0,
	/* T25 Object */
	 0, 0, 0, 0, 0, 0,
	/* T42 Object */
	 3, 60, 20, 20, 150, 0, 0, 0,
	/* T46 Object */
	 0, 3, 28, 28, 0, 0, 1, 0, 0,
	/* T47 Object */
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* T48 Object */
	 1, 3, 82, 0, 0, 0, 0, 0, 0, 0,
	 16, 30, 0, 6, 6, 0, 0, 124, 4, 100,
	 0, 0, 0, 5, 0, 42, 0, 1, 0, 40,
	 52, 20, 0, 0, 0, 50, 1, 5, 2, 1,
	 4, 5, 3, -25, 29, 38, 18, 143, 40, 207,
	 80, 10, 5, 2,
	/* T55 Object */
	0, 0, 0, 0,
};

#ifndef CONFIG_HUAWEI_KERNEL
static ssize_t mxt224e_vkeys_show(struct kobject *kobj,
			struct kobj_attribute *attr, char *buf)
{
	return snprintf(buf, 200,
	__stringify(EV_KEY) ":" __stringify(KEY_BACK) ":57:1030:90:90"
	":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":206:1030:90:90"
	":" __stringify(EV_KEY) ":" __stringify(KEY_HOME) ":366:1030:90:90"
	":" __stringify(EV_KEY) ":" __stringify(KEY_SEARCH) ":503:1030:90:90"
	"\n");
}

static struct kobj_attribute mxt224e_vkeys_attr = {
	.attr = {
		.mode = S_IRUGO,
	},
	.show = &mxt224e_vkeys_show,
};

static struct attribute *mxt224e_properties_attrs[] = {
	&mxt224e_vkeys_attr.attr,
	NULL
};

static struct attribute_group mxt224e_properties_attr_group = {
	.attrs = mxt224e_properties_attrs,
};

static void mxt_init_vkeys_8930(void)
{
	int rc = 0;
	static struct kobject *mxt224e_properties_kobj;

	mxt224e_vkeys_attr.attr.name = "virtualkeys.atmel_mxt_ts";
	mxt224e_properties_kobj = kobject_create_and_add("board_properties",
								NULL);
	if (mxt224e_properties_kobj)
		rc = sysfs_create_group(mxt224e_properties_kobj,
					&mxt224e_properties_attr_group);
	if (!mxt224e_properties_kobj || rc)
		pr_err("%s: failed to create board_properties\n",
				__func__);

	return;
}
#endif /*CONFIG_HUAWEI_KERNEL*/

static struct mxt_config_info mxt_config_array[] = {
	{
		.config			= mxt_config_data_8930_v1,
		.config_length		= ARRAY_SIZE(mxt_config_data_8930_v1),
		.family_id		= 0x81,
		.variant_id		= 0x01,
		.version		= 0x10,
		.build			= 0xAA,
		.bootldr_id		= MXT_BOOTLOADER_ID_224E,
		.fw_name		= "atmel_8930_fluid_v2_0_AB.hex",
	},
	{
		.config			= mxt_config_data_8930_v2,
		.config_length		= ARRAY_SIZE(mxt_config_data_8930_v2),
		.family_id		= 0x81,
		.variant_id		= 0x15,
		.version		= 0x11,
		.build			= 0xAA,
		.bootldr_id		= MXT_BOOTLOADER_ID_224E,
		.fw_name		= "atmel_8930_fluid_v2_0_AB.hex",
	},
	{
		.config			= mxt_config_data_8930_v2,
		.config_length		= ARRAY_SIZE(mxt_config_data_8930_v2),
		.family_id		= 0x81,
		.variant_id		= 0x01,
		.version		= 0x20,
		.build			= 0xAB,
		.bootldr_id		= MXT_BOOTLOADER_ID_224E,
	},
};

static struct mxt_platform_data mxt_platform_data_8930 = {
	.config_array		= mxt_config_array,
	.config_array_size	= ARRAY_SIZE(mxt_config_array),
	.panel_minx		= 0,
	.panel_maxx		= 566,
	.panel_miny		= 0,
	.panel_maxy		= 1067,
	.disp_minx		= 0,
	.disp_maxx		= 540,
	.disp_miny		= 0,
	.disp_maxy		= 960,
	.irqflags		= IRQF_TRIGGER_FALLING,
#ifdef MSM8930_PHASE_2
	.digital_pwr_regulator	= true,
#endif
	.i2c_pull_up		= true,
	.reset_gpio		= MXT_TS_RESET_GPIO,
	.irq_gpio		= MXT_TS_GPIO_IRQ,
};

#ifdef CONFIG_HUAWEI_NFC_PN544
static struct pn544_nfc_platform_data pn544_hw_data = 
{
	.pn544_ven_reset = pn544_ven_reset,
	.pn544_interrupt_gpio_config = pn544_interrupt_gpio_config,
	.pn544_fw_download_pull_down = pn544_fw_download_pull_down,
	.pn544_fw_download_pull_high = pn544_fw_download_pull_high,
	.pn544_clock_output_ctrl = pn544_clock_output_ctrl,
	.pn544_clock_output_mode_ctrl = pn544_clock_output_mode_ctrl,
};
#endif

#ifdef CONFIG_HUAWEI_KERNEL
static struct touch_hw_platform_data touch_hw_data = 
{
	.touch_power = power_switch,
	.set_touch_interrupt_gpio = set_touch_interrupt_gpio,
	.set_touch_probe_flag = set_touch_probe_flag,
	.read_touch_probe_flag = read_touch_probe_flag,
	.touch_reset = touch_reset,
	.get_touch_reset_gpio = get_touch_reset_gpio,
	.get_touch_resolution = get_touch_resolution,
	.read_button_flag = read_button_flag,
	.get_touch_button_map = get_touch_button_map,
};
#ifdef CONFIG_HUAWEI_CYPRESS_TOUCHSCREEN
/* do not arbitrarity set the feature register which includes the CA bit
 * at startup, the feature bits tell what is available
 * to enable a feature; write a 1 to the available bit.
static const uint8_t cyttsp_op_regs[] = {0, 0, 0, 0x08, 0x01};
 */
/*Change TP report distence from 9 to 1 pixel*/
static const uint8_t cyttsp_op_regs[] = {0, 0, 0, 0x00};
static const uint8_t cyttsp_si_regs[] = {0, 0, 0, 0, 0, 0, 0x00, 0xFF, 0x0A};
static const uint8_t cyttsp_bl_keys[] = {0, 1, 2, 3, 4, 5, 6, 7};
static const char cyttsp_use_name[] = CY_I2C_NAME;

static struct cyttsp_touch_settings cyttsp_sett_op_regs = {
	.data = (uint8_t *)&cyttsp_op_regs[0],
	.size = sizeof(cyttsp_op_regs),
	.tag = 3,
};

static struct cyttsp_touch_settings cyttsp_sett_si_regs = {
	.data = (uint8_t *)&cyttsp_si_regs[0],
	.size = sizeof(cyttsp_si_regs),
	.tag = 6,
};

static struct cyttsp_touch_settings cyttsp_sett_bl_keys = {
	.data = (uint8_t *)&cyttsp_bl_keys[0],
	.size = sizeof(cyttsp_bl_keys),
	.tag = 0,
};

#ifdef CY_USE_AUTOLOAD_FW
#include <linux/cyttsp3_img.h>
static struct cyttsp_touch_firmware cyttsp3_firmware = {
	.img = cyttsp3_img,
	.size = sizeof(cyttsp3_img),
	.ver = cyttsp3_ver,
	.vsize = sizeof(cyttsp3_ver),
};
#else
static struct cyttsp_touch_firmware cyttsp3_firmware = {
	.img = NULL,
	.size = 0,
	.ver = NULL,
	.vsize = 0,
};
#endif

#if 0 /* Use this block for pre-Gingerbread integrations */
static const uint16_t cyttsp3_abs[] = {
	ABS_MT_POSITION_X, CY_ABS_MIN_X, CY_ABS_MAX_X, 0, 0,
	ABS_MT_POSITION_Y, CY_ABS_MIN_Y, CY_ABS_MAX_Y, 0, 0,
	ABS_MT_TOUCH_MAJOR, CY_ABS_MIN_P, CY_ABS_MAX_P, 0, 0,
	CY_IGNORE_VALUE, CY_ABS_MIN_W, CY_ABS_MAX_W, 0, 0,
	ABS_MT_TRACKING_ID, CY_ABS_MIN_T, CY_ABS_MAX_T, 0, 0,
};
#else /* Use this block for Gingerbread and later integrations */
uint16_t cyttsp3_abs[] = {
	ABS_MT_POSITION_X, CY_ABS_MIN_X, CY_ABS_MAX_X, 0, 0,
	ABS_MT_POSITION_Y, CY_ABS_MIN_Y, CY_ABS_MAX_Y, 0, 0,
	ABS_MT_PRESSURE, CY_ABS_MIN_P, CY_ABS_MAX_P, 0, 0,
	ABS_MT_TOUCH_MAJOR, CY_ABS_MIN_W, CY_ABS_MAX_W, 0, 0,
	ABS_MT_TRACKING_ID, CY_ABS_MIN_T, CY_ABS_MAX_T, 0, 0,
};
#endif

struct cyttsp_touch_framework cyttsp3_framework = {
	.abs = (uint16_t *)&cyttsp3_abs[0], 
	.size = ARRAY_SIZE(cyttsp3_abs), 
	.enable_vkeys = 1,
};
struct cyttsp_touch_platform_data cyttsp3_i2c_touch_platform_data = 
{
	.sett = 
	{
		NULL,
		&cyttsp_sett_op_regs,
		&cyttsp_sett_si_regs,
		&cyttsp_sett_bl_keys,
	},
	.fw = &cyttsp3_firmware,
	.frmwrk = &cyttsp3_framework,
	.addr = {CY_I2C_TCH_ADR, CY_I2C_LDR_ADR},
	.flags = 0x00,
	.hw_reset = cyttsp3_hw_reset,
	.hw_recov = cyttsp3_hw_recov,
	.irq_stat = cyttsp3_irq_stat,
	.set_touch_probe_flag = set_touch_probe_flag,/*add by huawei*/
	.read_touch_probe_flag = read_touch_probe_flag,/*add by huawei*/
	.get_touch_resolution = get_touch_resolution, /*add by huawei*/
	.set_touch_interrupt_gpio = set_touch_interrupt_gpio,/*add by huawei*/
	.get_touch_reset_gpio = get_touch_reset_gpio,/*add this function for ESD restart IC*/
};	
#endif

#ifdef CONFIG_TOUCHSCREEN_CYPRESS_CYTTSP4
/* Cypress cyttsp4 start */
int cyttsp4_power_switch(int pm)
{
	int ret = 0;
	gpio_request(67,"TouchScreen_1.8_Enable");
	gpio_direction_output(67,1);
	mdelay(1);
	ret = gpio_get_value(67);
	printk("%s: gpio67=%d\n", __func__,ret);
	power_switch(pm);
	return ret;
}
static int cyttsp4_xres(struct cyttsp4_core_platform_data *pdata,
		struct device *dev)
{
	int rst_gpio = pdata->rst_gpio;
	int rc = 0;

	gpio_set_value(rst_gpio, 1);
	msleep(20);
	gpio_set_value(rst_gpio, 0);
	msleep(40);
	gpio_set_value(rst_gpio, 1);
	msleep(20);
	dev_info(dev,
		"%s: RESET CYTTSP gpio=%d r=%d\n", __func__,
		pdata->rst_gpio, rc);
	return rc;
}

static int cyttsp4_init(struct cyttsp4_core_platform_data *pdata,
		int on, struct device *dev)
{
	int rst_gpio = pdata->rst_gpio;
	int irq_gpio = pdata->irq_gpio;
	int rc = 0;

	if (gpio_tlmm_config(GPIO_CFG(irq_gpio, 0,GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),GPIO_CFG_ENABLE))
	{
		pr_err("%s:touch int gpio config failed\n", __func__);
		rc = -ENODEV;
	}

	if (gpio_tlmm_config(GPIO_CFG(rst_gpio, 0,GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),GPIO_CFG_ENABLE))
	{
		pr_err("%s:touch rst gpio config failed\n", __func__);
		rc = -ENODEV;
	}
	if (on) {
		rc = gpio_request(rst_gpio, NULL);
		if (rc < 0) {
			gpio_free(rst_gpio);
			rc = gpio_request(rst_gpio, NULL);
		}
		if (rc < 0) {
			dev_err(dev,
				"%s: Fail request gpio=%d\n", __func__,
				rst_gpio);
		} else {
			rc = gpio_direction_output(rst_gpio, 1);
			if (rc < 0) {
				pr_err("%s: Fail set output gpio=%d\n",
					__func__, rst_gpio);
				gpio_free(rst_gpio);
			} else {
				rc = gpio_request(irq_gpio, NULL);
				if (rc < 0) {
					gpio_free(irq_gpio);
					rc = gpio_request(irq_gpio,
						NULL);
				}
				if (rc < 0) {
					dev_err(dev,
						"%s: Fail request gpio=%d\n",
						__func__, irq_gpio);
					gpio_free(rst_gpio);
				} else {
					gpio_direction_input(irq_gpio);
				}
			}
		}
	} else {
		gpio_free(rst_gpio);
		gpio_free(irq_gpio);
	}

	dev_info(dev,
		"%s: INIT CYTTSP RST gpio=%d and IRQ gpio=%d r=%d\n",
		__func__, rst_gpio, irq_gpio, rc);
	return rc;
}

static int cyttsp4_wakeup(struct cyttsp4_core_platform_data *pdata,
		struct device *dev, atomic_t *ignore_irq)
{
	int irq_gpio = pdata->irq_gpio;
	int rc = 0;

	if (ignore_irq)
		atomic_set(ignore_irq, 1);
	rc = gpio_direction_output(irq_gpio, 0);
	if (rc < 0) {
		if (ignore_irq)
			atomic_set(ignore_irq, 0);
		dev_err(dev,
			"%s: Fail set output gpio=%d\n",
			__func__, irq_gpio);
	} else {
		udelay(2000);
		rc = gpio_direction_input(irq_gpio);
		if (ignore_irq)
			atomic_set(ignore_irq, 0);
		if (rc < 0) {
			dev_err(dev,
				"%s: Fail set input gpio=%d\n",
				__func__, irq_gpio);
		}
	}

	dev_info(dev,
		"%s: WAKEUP CYTTSP gpio=%d r=%d\n", __func__,
		irq_gpio, rc);
	return rc;
}

static int cyttsp4_sleep(struct cyttsp4_core_platform_data *pdata,
		struct device *dev, atomic_t *ignore_irq)
{
	return 0;
}

static int cyttsp4_power(struct cyttsp4_core_platform_data *pdata,
		int on, struct device *dev, atomic_t *ignore_irq)
{
	if (on)
		return cyttsp4_wakeup(pdata, dev, ignore_irq);

	return cyttsp4_sleep(pdata, dev, ignore_irq);
}

static int cyttsp4_irq_stat(struct cyttsp4_core_platform_data *pdata,
		struct device *dev)
{
	return gpio_get_value(pdata->irq_gpio);
}

/* Button to keycode conversion */
static u16 cyttsp4_btn_keys[] = {
	/* use this table to map buttons to keycodes (see input.h) */
	KEY_BACK,		/* 102 */
	KEY_HOME,		/* 139 */
	KEY_MENU,		/* 158 */
	KEY_SEARCH,		/* 217 */
	KEY_VOLUMEDOWN,		/* 114 */
	KEY_VOLUMEUP,		/* 115 */
	KEY_CAMERA,		/* 212 */
	KEY_POWER		/* 116 */
};

static struct touch_settings cyttsp4_sett_btn_keys = {
	.data = (uint8_t *)&cyttsp4_btn_keys[0],
	.size = ARRAY_SIZE(cyttsp4_btn_keys),
	.tag = 0,
};

static struct cyttsp4_core_platform_data _cyttsp4_core_platform_data = {
	.irq_gpio = MSM_8930_TOUCH_INT_PIN,
	.rst_gpio = MSM_8930_TOUCH_RESET_PIN,
	.xres = cyttsp4_xres,
	.init = cyttsp4_init,
	.touch_power = cyttsp4_power_switch,
	.power = cyttsp4_power,
	.irq_stat = cyttsp4_irq_stat,
	.set_touch_probe_flag = set_touch_probe_flag,
	.read_touch_probe_flag = read_touch_probe_flag,
	.sett = {
		NULL,	/* Reserved */
		NULL,	/* Command Registers */
		NULL,	/* Touch Report */
		NULL,	/* Cypress Data Record */
		NULL,	/* Test Record */
		NULL,	/* Panel Configuration Record */
		NULL, /* &cyttsp4_sett_param_regs, */
		NULL, /* &cyttsp4_sett_param_size, */
		NULL,	/* Reserved */
		NULL,	/* Reserved */
		NULL,	/* Operational Configuration Record */
		NULL, /* &cyttsp4_sett_ddata, *//* Design Data Record */
		NULL, /* &cyttsp4_sett_mdata, *//* Manufacturing Data Record */
		NULL,	/* Config and Test Registers */
		&cyttsp4_sett_btn_keys,	/* button-to-keycode table */
	},
	.loader_pdata = &_cyttsp4_loader_platform_data,
};


static struct cyttsp4_core_info cyttsp4_core_info __initdata = {
	.name = CYTTSP4_CORE_NAME,
	.id = "main_ttsp_core",
	.adap_id = CYTTSP4_I2C_NAME,
	.platform_data = &_cyttsp4_core_platform_data,
};

static const uint16_t cyttsp4_abs[] = {
	ABS_MT_POSITION_X, CY4_ABS_MIN_X, CY4_ABS_MAX_X, 0, 0,
	ABS_MT_POSITION_Y, CY4_ABS_MIN_Y, CY4_ABS_MAX_Y, 0, 0,
	ABS_MT_PRESSURE, CY4_ABS_MIN_P, CY4_ABS_MAX_P, 0, 0,
	CY_IGNORE_VALUE, CY4_ABS_MIN_W, CY4_ABS_MAX_W, 0, 0,
	ABS_MT_TRACKING_ID, CY4_ABS_MIN_T, CY4_ABS_MAX_T, 0, 0,
	ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0,
	ABS_MT_TOUCH_MINOR, 0, 255, 0, 0,
	ABS_MT_ORIENTATION, -128, 127, 0, 0,
};
struct touch_framework cyttsp4_framework = {
	.abs = (uint16_t *)&cyttsp4_abs[0],
	.size = ARRAY_SIZE(cyttsp4_abs),
	.enable_vkeys = 0,
};

static struct cyttsp4_mt_platform_data _cyttsp4_mt_platform_data = {
	.frmwrk = &cyttsp4_framework,
	.flags = 0x40,
	.inp_dev_name = CYTTSP4_MT_NAME,
};

struct cyttsp4_device_info cyttsp4_mt_info __initdata = {
	.name = CYTTSP4_MT_NAME,
	.core_id = "main_ttsp_core",
	.platform_data = &_cyttsp4_mt_platform_data,
};

static struct cyttsp4_btn_platform_data _cyttsp4_btn_platform_data = {
	.inp_dev_name = CYTTSP4_BTN_NAME,
};

struct cyttsp4_device_info cyttsp4_btn_info __initdata = {
	.name = CYTTSP4_BTN_NAME,
	.core_id = "main_ttsp_core",
	.platform_data = &_cyttsp4_btn_platform_data,
};

static void  huawei_cyttsp4_init(void)
{
	/* Register core and devices */
	cyttsp4_register_core_device(&cyttsp4_core_info);
	cyttsp4_register_device(&cyttsp4_mt_info);
	cyttsp4_register_device(&cyttsp4_btn_info);
}


/* Cypress cyttsp4 end */
#endif

static struct i2c_board_info touch_device_info_8930[] __initdata = {
#ifdef CONFIG_HUAWEI_FEATURE_RMI_TOUCH
	{
		I2C_BOARD_INFO("Synaptics_rmi", 0x70),
		.platform_data = &touch_hw_data,
		.irq = MSM_GPIO_TO_INT(MSM_8930_TOUCH_INT_PIN),
		.flags = true,
	},
#endif

#ifdef CONFIG_HUAWEI_CYPRESS_TOUCHSCREEN
	{
		I2C_BOARD_INFO(CY_I2C_NAME, CY_I2C_TCH_ADR),
		.irq = MSM_GPIO_TO_INT(MSM_8930_TOUCH_INT_PIN),
		.platform_data = &cyttsp3_i2c_touch_platform_data,
        .flags = true,
	},
#endif

#ifdef CONFIG_TOUCHSCREEN_CYPRESS_CYTTSP4
	{
		I2C_BOARD_INFO(CYTTSP4_I2C_NAME, CYTTSP4_I2C_TCH_ADR),
		.irq = MSM_GPIO_TO_INT(MSM_8930_TOUCH_INT_PIN),
		.platform_data = CYTTSP4_I2C_NAME,
		.flags = true,
	},
#endif


    {
		I2C_BOARD_INFO("Synaptics_rmi", 0x20),
		.platform_data = &touch_hw_data,
		.irq = MSM_GPIO_TO_INT(MSM_8930_TOUCH_INT_PIN),
        .flags = true,
	},
};
#endif
static struct i2c_board_info mxt_device_info_8930[] __initdata = {
	{
		I2C_BOARD_INFO("atmel_mxt_ts", 0x4a),
		.platform_data = &mxt_platform_data_8930,
		.irq = MSM_GPIO_TO_INT(MXT_TS_GPIO_IRQ),
	},
};
/* -------------------- huawei sensors -------------------- */
static int gs_init_flag = 0;   /*gsensor is not initialized*/
/*modify sensors platform_data */
#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_ACCELEROMETER_ADI_ADXL346
static struct gs_platform_data gs_adi346_platform_data = {
	.adapt_fn = gsensor_support_dummyaddr_adi346,
	.slave_addr = (0xA6 >> 1),  /*i2c slave address*/
	.dev_id = 0x00,    /*WHO AM I*/
	.init_flag = &gs_init_flag,
	.get_compass_gs_position=get_compass_gs_position,
	.gs_power = power_switch,
};
#endif

#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_ACCELEROMETER_KXTIK1004
static struct gs_platform_data gs_kxtik_platform_data = {
	.adapt_fn = gsensor_support_dummyaddr_kxtik,
	.slave_addr = (0x1E >> 1),  /*i2c slave address*/
	.dev_id = 0x05,    /*WHO AM I*/
	.init_flag = &gs_init_flag,
	.get_compass_gs_position=get_compass_gs_position,
	.gs_power = power_switch,
};
#endif

#ifdef CONFIG_HUAWEI_FEATURE_GYROSCOPE_L3G4200DH
static struct gyro_platform_data gy_l3g4200d_platform_data = {
	.gyro_power = power_switch,
	.axis_map_x = 0,     /*x map read data[axis_map_x] from i2c*/
	.axis_map_y = 1,
	.axis_map_z = 2,	
	.negate_x = 0,       /*negative x,y or z*/
	.negate_y =0,
	.negate_z = 0,
	.slave_addr = 0x68,  	/*i2c slave address*/
	.dev_id = 0x0F,             /*WHO AM I*/
};
#endif

#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_ACCELEROMETER_MMA8452
static struct gs_platform_data gs_mma8452_platform_data = {
	.adapt_fn = NULL,
	.slave_addr = (0x38 >> 1),  /*i2c slave address*/
	.dev_id = 0x2A,    /*WHO AM I*/
	.init_flag = &gs_init_flag,
	.get_compass_gs_position=get_compass_gs_position,
	.gs_power = power_switch,
};
#endif

#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_ACCELEROMETER_ST_LIS3XH
static struct gs_platform_data gs_st_lis3xh_platform_data = {
	.adapt_fn = NULL,
	.slave_addr = (0x30 >> 1),  /*i2c slave address*/
	.dev_id = 0x00,    /*WHO AM I*/
	.init_flag = &gs_init_flag,
	.get_compass_gs_position=get_compass_gs_position,
	.gs_power = power_switch,
};
#endif

#ifdef CONFIG_HUAWEI_FEATURE_PROXIMITY_EVERLIGHT_APS_9900
static struct aps9900_hw_platform_data aps9900_hw_data = {
	.aps9900_gpio_config_interrupt = aps9900_gpio_config_interrupt,
	.aps9900_power = power_switch,
};
#endif

#ifdef CONFIG_HUAWEI_FEATURE_PROXIMITY_EVERLIGHT_APS_12D
static struct aps12d_hw_platform_data aps12d_hw_data = {
	.aps12d_power = power_switch,
};
#endif

#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_AK8975
static struct compass_platform_data akm8975_hw_data = {
	.compass_power = power_switch,
};
#endif

#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_AK8963
static struct compass_platform_data akm8963_hw_data = {
	.compass_power = power_switch,
};
#endif

static struct i2c_board_info flash_led_device_info_8930[] __initdata = {
#ifdef CONFIG_HUAWEI_FEATURE_LM3642
	{
	I2C_BOARD_INFO("lm3642" , 0x63),
	},
#endif

#ifdef CONFIG_HUAWEI_FEATURE_TPS61310
	{
	I2C_BOARD_INFO("tps61310", 0x33),		
	},
#endif
};
static struct i2c_board_info sensor_device_info_8930[] __initdata = {
#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_ACCELEROMETER_ADI_ADXL346
	{
		I2C_BOARD_INFO("gs_adi346", 0xA8>>1),  /* actual address 0xA6, fake address 0xA8*/
		.platform_data = &gs_adi346_platform_data,
		.irq = MSM_GPIO_TO_INT(46)    //MEMS_INT1
	},
#endif 

#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_ACCELEROMETER_KXTIK1004
	{ 
		I2C_BOARD_INFO("gs_kxtik", 0x1E >> 1),  /* actual address 0x0F*/
		.platform_data = &gs_kxtik_platform_data,
		.irq = MSM_GPIO_TO_INT(46)     //MEMS_INT1
	},
#endif

#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_ACCELEROMETER_MMA8452
	{
		I2C_BOARD_INFO("gs_mma8452", 0x38 >> 1),
		.platform_data = &gs_mma8452_platform_data,
		.irq = MSM_GPIO_TO_INT(46)    //MEMS_INT1
	},
#endif	

#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_ACCELEROMETER_ST_LIS3XH
	{
		I2C_BOARD_INFO("gs_st_lis3xh", 0x30 >> 1),
		.platform_data = &gs_st_lis3xh_platform_data,
		.irq = MSM_GPIO_TO_INT(46)    //MEMS_INT1
	},
#endif

#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_AK8975
	{
		I2C_BOARD_INFO("akm8975", 0x0C),//7 bit addr, no write bit
		.platform_data = &akm8975_hw_data,
		.irq = MSM_GPIO_TO_INT(MSM_8930_COMPASS_INT_PIN)
	},
#endif 

#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_AK8963
	{
		I2C_BOARD_INFO("akm8963", 0x0E),//7 bit addr, no write bit
		.platform_data = &akm8963_hw_data,
		.irq = MSM_GPIO_TO_INT(MSM_8930_COMPASS_INT_PIN)
	},
#endif 

#ifdef CONFIG_HUAWEI_FEATURE_GYROSCOPE_L3G4200DH
	{   
		I2C_BOARD_INFO("l3g4200d", 0x68),  
		.platform_data = &gy_l3g4200d_platform_data,
	},
	#endif

#ifdef CONFIG_HUAWEI_FEATURE_PROXIMITY_EVERLIGHT_APS_12D
	{   
		I2C_BOARD_INFO("aps-12d", 0x88 >> 1),  
		.platform_data = &aps12d_hw_data,
	},
#endif

#ifdef CONFIG_HUAWEI_FEATURE_PROXIMITY_EVERLIGHT_APS_9900
	{   
		I2C_BOARD_INFO("aps-9900", 0x39),
		.irq = MSM_GPIO_TO_INT(MSM_8930_APS_INT_PIN),
		.platform_data = &aps9900_hw_data,
	},
#endif
};
#ifdef CONFIG_QWERTY_KEYPAD_ADP5587
static struct i2c_board_info qwerty_keypad_device_info[] __initdata = {
	{
		I2C_BOARD_INFO("adp5587", 0x34),
		.irq = MSM_GPIO_TO_INT(19)
	},
};
#endif

#define MHL_POWER_GPIO_PM8038	PM8038_GPIO_PM_TO_SYS(MHL_GPIO_PWR_EN)
#define MHL_POWER_GPIO_PM8917	PM8917_GPIO_PM_TO_SYS(25)
static struct msm_mhl_platform_data mhl_platform_data = {
	.irq = MSM_GPIO_TO_INT(MHL_GPIO_INT),
	.gpio_mhl_int = MHL_GPIO_INT,
	.gpio_mhl_reset = MHL_GPIO_RESET,
	.gpio_mhl_power = MHL_POWER_GPIO_PM8038,
	.gpio_hdmi_mhl_mux = HDMI_MHL_MUX_GPIO,
};

static struct i2c_board_info sii_device_info[] __initdata = {
	{
		/*
		 * keeps SI 8334 as the default
		 * MHL TX
		 */
		I2C_BOARD_INFO("sii8334", 0x39),
		.platform_data = &mhl_platform_data,
		.flags = I2C_CLIENT_WAKE,
	},
};

#ifdef CONFIG_HUAWEI_NFC_PN544
static struct i2c_board_info nfc_device_info_8930[] __initdata = {
	{
		I2C_BOARD_INFO(PN544_DRIVER_NAME, PN544_I2C_ADDR),
		.irq = MSM_GPIO_TO_INT(GPIO_NFC_INT),
		.platform_data = &pn544_hw_data,
	},
};
#endif

#ifdef MSM8930_PHASE_2

#define GPIO_VOLUME_UP_PM8038		PM8038_GPIO_PM_TO_SYS(3)
#ifdef CONFIG_HUAWEI_KERNEL
#define GPIO_PTT_KEY_PM8038		   PM8038_GPIO_PM_TO_SYS(6)
/* use gpio7 work as volume- */
#define GPIO_VOLUME_DOWN_PM8038		PM8038_GPIO_PM_TO_SYS(7)
#else
#define GPIO_VOLUME_DOWN_PM8038		PM8038_GPIO_PM_TO_SYS(8)
#endif
/* use gpio50 work as slide. use mpp10 work as snapshot */
#ifdef CONFIG_HUAWEI_KERNEL
#define GPIO_SLIDE_DETECT		50
#endif 

#define GPIO_CAMERA_SNAPSHOT_PM8038	PM8038_GPIO_PM_TO_SYS(10)
#define GPIO_CAMERA_FOCUS_PM8038	PM8038_GPIO_PM_TO_SYS(11)

#define GPIO_VOLUME_UP_PM8917		PM8917_GPIO_PM_TO_SYS(27)
#define GPIO_VOLUME_DOWN_PM8917		PM8917_GPIO_PM_TO_SYS(28)
#define GPIO_CAMERA_SNAPSHOT_PM8917	PM8917_GPIO_PM_TO_SYS(36)
#define GPIO_CAMERA_FOCUS_PM8917	PM8917_GPIO_PM_TO_SYS(37)

static struct gpio_keys_button keys_8930_pm8038[] = {
	{
		.code = KEY_VOLUMEUP,
		.type = EV_KEY,
		.desc = "volume_up",
		.gpio = GPIO_VOLUME_UP_PM8038,
         //delete	6 lines	
		.wakeup = 1,    
		.active_low = 1,
		.debounce_interval = 15,
	},
	{
		.code = KEY_VOLUMEDOWN,
		.type = EV_KEY,
		.desc = "volume_down",
		.gpio = GPIO_VOLUME_DOWN_PM8038,
         //delete	6 lines	
		.wakeup = 1,   
		.active_low = 1,
		.debounce_interval = 15,
	},
	{
		.code = KEY_CAMERA_FOCUS,
		.type = EV_KEY,
		.desc = "camera_focus",
		.gpio = GPIO_CAMERA_FOCUS_PM8038,
		.wakeup = 1,
		.active_low = 1,
		.debounce_interval = 15,
	},
	{
		.code = KEY_CAMERA,
		.type = EV_KEY,
		.desc = "camera_snapshot",
		.gpio = GPIO_CAMERA_SNAPSHOT_PM8038,
		.wakeup = 0,
		.active_low = 1,
		.debounce_interval = 15,
	},
#ifdef CONFIG_HUAWEI_KERNEL
	{
		.code = SW_LID,
		.type = EV_SW,
		.desc = "SLIDE_DETECT",
		.gpio = GPIO_SLIDE_DETECT,
		.wakeup = 1,
		.active_low = 0,
		.debounce_interval = 15,
	},
#endif
};

static struct gpio_keys_button keys_8930_pm8917[] = {
	{
		.code = KEY_VOLUMEUP,
		.type = EV_KEY,
		.desc = "volume_up",
		.gpio = GPIO_VOLUME_UP_PM8917,
		.wakeup = 1,
		.active_low = 1,
		.debounce_interval = 15,
	},
	{
		.code = KEY_VOLUMEDOWN,
		.type = EV_KEY,
		.desc = "volume_down",
		.gpio = GPIO_VOLUME_DOWN_PM8917,
		.wakeup = 1,
		.active_low = 1,
		.debounce_interval = 15,
	},
	{
		.code = KEY_CAMERA_FOCUS,
		.type = EV_KEY,
		.desc = "camera_focus",
		.gpio = GPIO_CAMERA_FOCUS_PM8917,
		.wakeup = 1,
		.active_low = 1,
		.debounce_interval = 15,
	},
	{
		.code = KEY_CAMERA_SNAPSHOT,
		.type = EV_KEY,
		.desc = "camera_snapshot",
		.gpio = GPIO_CAMERA_SNAPSHOT_PM8917,
		.wakeup = 1,
		.active_low = 1,
		.debounce_interval = 15,
	},
};

static struct gpio_keys_button keys_8930_pm8038_custom[] = {
	{
		.code = KEY_CHAT,
		.type = EV_KEY,
		.desc = KEYNAME_PTT,
		.gpio = GPIO_PTT_KEY_PM8038,
		.wakeup = 1,
		.active_low = 1,
		.debounce_interval = 15,
	},
};	

static struct gpio_keys_platform_data gpio_keys_8930_custom_pdata = {
	.buttons = keys_8930_pm8038_custom,
	.nbuttons = ARRAY_SIZE(keys_8930_pm8038_custom),
};

static struct platform_device gpio_keys_8930_custom = {
	.name		= "gpio-keys",
	.id		= 0,
	.dev		= {
		.platform_data  = &gpio_keys_8930_custom_pdata,
	},
};

/* Add GPIO keys for 8930 */
static struct gpio_keys_platform_data gpio_keys_8930_pdata = {
	.buttons = keys_8930_pm8038,
	.nbuttons = ARRAY_SIZE(keys_8930_pm8038),
};

static struct platform_device gpio_keys_8930 = {
	.name		= "gpio-keys",
	.id		= -1,
	.dev		= {
		.platform_data  = &gpio_keys_8930_pdata,
	},
};
#endif /* MSM8930_PHASE_2 */

static struct msm_i2c_platform_data msm8960_i2c_qup_gsbi4_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
};
/*set i2c frequency to 400K to accelerate transmit speed*/
static struct msm_i2c_platform_data msm8960_i2c_qup_gsbi3_pdata = {
	.clk_freq = 400000,
	.src_clk_rate = 24000000,
};
static struct msm_i2c_platform_data msm8960_i2c_qup_gsbi9_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
};

static struct msm_i2c_platform_data msm8960_i2c_qup_gsbi10_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
};

static struct msm_i2c_platform_data msm8960_i2c_qup_gsbi12_pdata = {
	.clk_freq = 100000,
	.src_clk_rate = 24000000,
};


static struct ks8851_pdata spi_eth_pdata = {
	.irq_gpio = KS8851_IRQ_GPIO,
	.rst_gpio = KS8851_RST_GPIO,
};

static struct spi_board_info spi_board_info[] __initdata = {
	{
		.modalias               = "ks8851",
		.irq                    = MSM_GPIO_TO_INT(KS8851_IRQ_GPIO),
		.max_speed_hz           = 19200000,
		.bus_num                = 0,
		.chip_select            = 0,
		.mode                   = SPI_MODE_0,
		.platform_data		= &spi_eth_pdata
	},
	{
		.modalias               = "dsi_novatek_3d_panel_spi",
		.max_speed_hz           = 10800000,
		.bus_num                = 0,
		.chip_select            = 1,
		.mode                   = SPI_MODE_0,
	},
};

static struct platform_device msm_device_saw_core0 = {
	.name	= "saw-regulator",
	.id	= 0,
	.dev	= {
		.platform_data = &msm8930_pm8038_saw_regulator_core0_pdata,
	},
};

static struct platform_device msm_device_saw_core1 = {
	.name	= "saw-regulator",
	.id	= 1,
	.dev	= {
		.platform_data = &msm8930_pm8038_saw_regulator_core1_pdata,
	},
};

static struct tsens_platform_data msm_tsens_pdata  = {
	.tsens_factor		= 1000,
	.hw_type		= APQ_8064,
	.tsens_num_sensor	= 10,
	.slope = {1132, 1135, 1137, 1135, 1157,
			1142, 1124, 1153, 1175, 1166},
};

static struct platform_device msm_tsens_device = {
	.name   = "tsens8960-tm",
	.id = -1,
};

static struct msm_thermal_data msm_thermal_pdata = {
	.sensor_id = 9,
	.poll_ms = 250,
	.limit_temp_degC = 60,
	.temp_hysteresis_degC = 10,
	.freq_step = 2,
};

#ifdef CONFIG_MSM_FAKE_BATTERY
static struct platform_device fish_battery_device = {
	.name = "fish_battery",
};
#endif

#ifndef MSM8930_PHASE_2

/* 8930 Phase 1 */
static struct platform_device msm8930_device_ext_5v_vreg __devinitdata = {
	.name	= GPIO_REGULATOR_DEV_NAME,
	.id	= PM8921_MPP_PM_TO_SYS(7),
	.dev	= {
		.platform_data = &msm_gpio_regulator_pdata[GPIO_VREG_ID_EXT_5V],
	},
};

static struct platform_device msm8930_device_ext_l2_vreg __devinitdata = {
	.name	= GPIO_REGULATOR_DEV_NAME,
	.id	= 91,
	.dev	= {
		.platform_data = &msm_gpio_regulator_pdata[GPIO_VREG_ID_EXT_L2],
	},
};

#else

/* 8930 Phase 2 */
static struct platform_device msm8930_device_ext_5v_vreg __devinitdata = {
	.name	= GPIO_REGULATOR_DEV_NAME,
	.id	= 63,
	.dev	= {
		.platform_data = &msm8930_pm8038_gpio_regulator_pdata[
					MSM8930_GPIO_VREG_ID_EXT_5V],
	},
};

static struct platform_device msm8930_device_ext_otg_sw_vreg __devinitdata = {
	.name	= GPIO_REGULATOR_DEV_NAME,
	.id	= 97,
	.dev	= {
		.platform_data = &msm8930_pm8038_gpio_regulator_pdata[
					MSM8930_GPIO_VREG_ID_EXT_OTG_SW],
	},
};

#endif

static struct platform_device msm8930_device_rpm_regulator __devinitdata = {
	.name	= "rpm-regulator",
	.id	= -1,
	.dev	= {
#ifndef MSM8930_PHASE_2
		.platform_data = &msm_rpm_regulator_pdata,
#else
		.platform_data = &msm8930_pm8038_rpm_regulator_pdata,
#endif
	},
};

#ifdef CONFIG_HUAWEI_HW_DEV_DCT
static struct platform_device huawei_device_detect = {
	.name = "hw-dev-detect",
	.id   =-1,
};
#endif

static struct platform_device *early_common_devices[] __initdata = {
	&msm8960_device_dmov,
	&msm_device_smd,
	&msm8960_device_uart_gsbi5,
	&msm_device_uart_dm6,
	&msm_device_saw_core0,
	&msm_device_saw_core1,
};

/* ext_5v and ext_otg_sw are present when using PM8038 */
static struct platform_device *pmic_pm8038_devices[] __initdata = {
	&msm8930_device_ext_5v_vreg,
#ifndef MSM8930_PHASE_2
	&msm8930_device_ext_l2_vreg,
#endif
	&msm8960_device_ssbi_pmic,
#ifdef MSM8930_PHASE_2
	&msm8930_device_ext_otg_sw_vreg,
#endif
};

/* ext_5v and ext_otg_sw are not present when using PM8917 */
static struct platform_device *pmic_pm8917_devices[] __initdata = {
	&msm8960_device_ssbi_pmic,
};

#ifdef CONFIG_HUAWEI_KERNEL
static struct platform_device *wcnss_devices[] __initdata = {
        &msm_device_wcnss_wlan,
};
#endif

static struct platform_device *common_devices[] __initdata = {
	&msm_8960_q6_lpass,
	&msm_8960_riva,
	&msm_pil_tzapps,
	&msm_pil_vidc,
	&msm8960_device_qup_spi_gsbi1,
	&msm8960_device_qup_i2c_gsbi3,
	&msm8960_device_qup_i2c_gsbi4,
	&msm8960_device_qup_i2c_gsbi9,
	&msm8960_device_qup_i2c_gsbi10,
	&msm8960_device_qup_i2c_gsbi12,
	&msm_slim_ctrl,
#ifndef CONFIG_HUAWEI_KERNEL	
	&msm_device_wcnss_wlan,
#endif	
#if defined(CONFIG_QSEECOM)
		&qseecom_device,
#endif

#if defined(CONFIG_CRYPTO_DEV_QCRYPTO) || \
		defined(CONFIG_CRYPTO_DEV_QCRYPTO_MODULE)
	&qcrypto_device,
#endif

#if defined(CONFIG_CRYPTO_DEV_QCEDEV) || \
		defined(CONFIG_CRYPTO_DEV_QCEDEV_MODULE)
	&qcedev_device,
#endif
#ifdef CONFIG_MSM_ROTATOR
	&msm_rotator_device,
#endif
	&msm_device_sps,
#ifdef CONFIG_MSM_FAKE_BATTERY
	&fish_battery_device,
#endif
#ifdef CONFIG_ANDROID_PMEM
#ifndef CONFIG_MSM_MULTIMEDIA_USE_ION
	&msm8930_android_pmem_device,
	&msm8930_android_pmem_adsp_device,
	&msm8930_android_pmem_audio_device,
#endif /*CONFIG_MSM_MULTIMEDIA_USE_ION*/
#endif /*CONFIG_ANDROID_PMEM*/
	&msm8930_fmem_device,
	&msm_device_bam_dmux,
	&msm_fm_platform_init,

#ifdef CONFIG_HW_RANDOM_MSM
	&msm_device_rng,
#endif
	&msm8930_rpm_device,
	&msm8930_rpm_log_device,
	&msm8930_rpm_rbcpr_device,
	&msm8930_rpm_stat_device,
	&msm8930_rpm_master_stat_device,
#ifdef CONFIG_ION_MSM
	&msm8930_ion_dev,
#endif
	&msm_device_tz_log,
	&coresight_tpiu_device,
	&coresight_etb_device,
	&coresight_funnel_device,
	&coresight_etm0_device,
	&coresight_etm1_device,
	&msm_device_dspcrashd_8960,
	&msm8960_device_watchdog,
#ifdef MSM8930_PHASE_2
	&gpio_keys_8930,
#endif
	&msm8930_rtb_device,
	&msm8930_cpu_idle_device,
	&msm8930_msm_gov_device,
	&msm_bus_8930_apps_fabric,
	&msm_bus_8930_sys_fabric,
	&msm_bus_8930_mm_fabric,
	&msm_bus_8930_sys_fpb,
	&msm_bus_8930_cpss_fpb,
	&msm8960_device_cache_erp,
	&msm8930_iommu_domain_device,
	&msm_tsens_device,
	&msm8930_cache_dump_device,
#ifdef CONFIG_HUAWEI_HW_DEV_DCT
	&huawei_device_detect,
#endif
	&msm8930_pc_cntr,
};

static struct platform_device *cdp_devices[] __initdata = {
	&msm8960_device_otg,
	&msm8960_device_gadget_peripheral,
	&msm_device_hsusb_host,
	&android_usb_device,
	&msm_pcm,
	&msm_pcm_routing,
	&msm_cpudai0,
	&msm_cpudai1,
	/* Merge MI2S patch */
	&msm_cpudai_mi2s,
	&msm_cpudai_hdmi_rx,
	&msm_cpudai_bt_rx,
	&msm_cpudai_bt_tx,
	&msm_cpudai_fm_rx,
	&msm_cpudai_fm_tx,
	&msm_cpudai_auxpcm_rx,
	&msm_cpudai_auxpcm_tx,
	&msm_cpu_fe,
	&msm_stub_codec,
#ifdef CONFIG_MSM_GEMINI
	&msm8960_gemini_device,
#endif
	&msm_voice,
	&msm_voip,
	&msm_lpa_pcm,
	&msm_cpudai_afe_01_rx,
	&msm_cpudai_afe_01_tx,
	&msm_cpudai_afe_02_rx,
	&msm_cpudai_afe_02_tx,
	&msm_pcm_afe,
	&msm_compr_dsp,
	&msm_cpudai_incall_music_rx,
	&msm_cpudai_incall_record_rx,
	&msm_cpudai_incall_record_tx,
	&msm_pcm_hostless,
	&msm_multi_ch_pcm,
	&msm_lowlatency_pcm,
};

static void __init msm8930_i2c_init(void)
{
	msm8960_device_qup_i2c_gsbi4.dev.platform_data =
					&msm8960_i2c_qup_gsbi4_pdata;

	msm8960_device_qup_i2c_gsbi3.dev.platform_data =
					&msm8960_i2c_qup_gsbi3_pdata;

	msm8960_device_qup_i2c_gsbi9.dev.platform_data =
					&msm8960_i2c_qup_gsbi9_pdata;

	msm8960_device_qup_i2c_gsbi10.dev.platform_data =
					&msm8960_i2c_qup_gsbi10_pdata;

	msm8960_device_qup_i2c_gsbi12.dev.platform_data =
					&msm8960_i2c_qup_gsbi12_pdata;
}

static struct msm_rpmrs_level msm_rpmrs_levels[] __initdata = {
	{
		MSM_PM_SLEEP_MODE_WAIT_FOR_INTERRUPT,
		MSM_RPMRS_LIMITS(ON, ACTIVE, MAX, ACTIVE),
		true,
		1, 784, 180000, 100,
	},

	{
		MSM_PM_SLEEP_MODE_RETENTION,
		MSM_RPMRS_LIMITS(ON, ACTIVE, MAX, ACTIVE),
		true,
		415, 715, 340827, 475,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE_STANDALONE,
		MSM_RPMRS_LIMITS(ON, ACTIVE, MAX, ACTIVE),
		true,
		1300, 228, 1200000, 2000,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(ON, GDHS, MAX, ACTIVE),
		false,
		2000, 138, 1208400, 3200,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(ON, HSFS_OPEN, ACTIVE, RET_HIGH),
		false,
		6000, 119, 1850300, 9000,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, GDHS, MAX, ACTIVE),
		false,
		9200, 68, 2839200, 16400,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, MAX, ACTIVE),
		false,
		10300, 63, 3128000, 18200,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, ACTIVE, RET_HIGH),
		false,
		18000, 10, 4602600, 27000,
	},

	{
		MSM_PM_SLEEP_MODE_POWER_COLLAPSE,
		MSM_RPMRS_LIMITS(OFF, HSFS_OPEN, RET_HIGH, RET_LOW),
		false,
		20000, 2, 5752000, 32000,
	},
};

static struct msm_rpmrs_platform_data msm_rpmrs_data __initdata = {
	.levels = &msm_rpmrs_levels[0],
	.num_levels = ARRAY_SIZE(msm_rpmrs_levels),
	.vdd_mem_levels  = {
		[MSM_RPMRS_VDD_MEM_RET_LOW]	= 750000,
		[MSM_RPMRS_VDD_MEM_RET_HIGH]	= 750000,
		[MSM_RPMRS_VDD_MEM_ACTIVE]	= 1050000,
		[MSM_RPMRS_VDD_MEM_MAX]		= 1150000,
	},
	.vdd_dig_levels = {
		[MSM_RPMRS_VDD_DIG_RET_LOW]	= 0,
		[MSM_RPMRS_VDD_DIG_RET_HIGH]	= 0,
		[MSM_RPMRS_VDD_DIG_ACTIVE]	= 1,
		[MSM_RPMRS_VDD_DIG_MAX]		= 3,
	},
	.vdd_mask = 0x7FFFFF,
	.rpmrs_target_id = {
		[MSM_RPMRS_ID_PXO_CLK]		= MSM_RPM_ID_PXO_CLK,
		[MSM_RPMRS_ID_L2_CACHE_CTL]	= MSM_RPM_ID_LAST,
		[MSM_RPMRS_ID_VDD_DIG_0]	= MSM_RPM_ID_VOLTAGE_CORNER,
		[MSM_RPMRS_ID_VDD_DIG_1]	= MSM_RPM_ID_LAST,
		[MSM_RPMRS_ID_VDD_MEM_0]	= MSM_RPM_ID_PM8038_L24_0,
		[MSM_RPMRS_ID_VDD_MEM_1]	= MSM_RPM_ID_PM8038_L24_1,
		[MSM_RPMRS_ID_RPM_CTL]		= MSM_RPM_ID_RPM_CTL,
	},
};

static struct msm_rpmrs_platform_data msm_rpmrs_data_pm8917 __initdata = {
	.levels = &msm_rpmrs_levels[0],
	.num_levels = ARRAY_SIZE(msm_rpmrs_levels),
	.vdd_mem_levels  = {
		[MSM_RPMRS_VDD_MEM_RET_LOW]	= 750000,
		[MSM_RPMRS_VDD_MEM_RET_HIGH]	= 750000,
		[MSM_RPMRS_VDD_MEM_ACTIVE]	= 1050000,
		[MSM_RPMRS_VDD_MEM_MAX]		= 1150000,
	},
	.vdd_dig_levels = {
		[MSM_RPMRS_VDD_DIG_RET_LOW]	= 0,
		[MSM_RPMRS_VDD_DIG_RET_HIGH]	= 0,
		[MSM_RPMRS_VDD_DIG_ACTIVE]	= 1,
		[MSM_RPMRS_VDD_DIG_MAX]		= 3,
	},
	.vdd_mask = 0x7FFFFF,
	.rpmrs_target_id = {
		[MSM_RPMRS_ID_PXO_CLK]		= MSM_RPM_ID_PXO_CLK,
		[MSM_RPMRS_ID_L2_CACHE_CTL]	= MSM_RPM_ID_LAST,
		[MSM_RPMRS_ID_VDD_DIG_0]	= MSM_RPM_ID_VOLTAGE_CORNER,
		[MSM_RPMRS_ID_VDD_DIG_1]	= MSM_RPM_ID_LAST,
		[MSM_RPMRS_ID_VDD_MEM_0]	= MSM_RPM_ID_PM8917_L24_0,
		[MSM_RPMRS_ID_VDD_MEM_1]	= MSM_RPM_ID_PM8917_L24_1,
		[MSM_RPMRS_ID_RPM_CTL]		= MSM_RPM_ID_RPM_CTL,
	},
};

static struct msm_pm_boot_platform_data msm_pm_boot_pdata __initdata = {
	.mode = MSM_PM_BOOT_CONFIG_TZ,
};

#ifdef CONFIG_I2C
#define I2C_SURF 1
#define I2C_FFA  (1 << 1)
#define I2C_RUMI (1 << 2)
#define I2C_SIM  (1 << 3)
#define I2C_FLUID (1 << 4)
#define I2C_LIQUID (1 << 5)

struct i2c_registry {
	u8                     machs;
	int                    bus;
	struct i2c_board_info *info;
	int                    len;
};

#ifdef CONFIG_INPUT_MPU3050
#define MPU3050_INT_GPIO		69

static struct mpu3050_gyro_platform_data mpu3050_gyro = {
	.gpio_int = MPU3050_INT_GPIO,
};

static struct i2c_board_info __initdata mpu3050_i2c_boardinfo[] = {
	{
		I2C_BOARD_INFO("mpu3050", 0x68),
		.irq = MSM_GPIO_TO_INT(MPU3050_INT_GPIO),
		.platform_data = &mpu3050_gyro,
	},
};
#endif

#ifdef CONFIG_ISL9519_CHARGER
static struct isl_platform_data isl_data __initdata = {
	.valid_n_gpio		= 0,	/* Not required when notify-by-pmic */
	.chg_detection_config	= NULL,	/* Not required when notify-by-pmic */
	.max_system_voltage	= 4200,
	.min_system_voltage	= 3200,
	.chgcurrent		= 1000, /* 1900, */
	.term_current		= 400,	/* Need fine tuning */
	.input_current		= 2048,
};

static struct i2c_board_info isl_charger_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("isl9519q", 0x9),
		.irq		= 0,	/* Not required when notify-by-pmic */
		.platform_data	= &isl_data,
	},
};
#endif /* CONFIG_ISL9519_CHARGER */

#ifdef CONFIG_STM_LIS3DH
static struct lis3dh_acc_platform_data lis3dh_accel = {
	.poll_interval = 200,
	.min_interval = 10,
	.g_range = LIS3DH_ACC_G_2G,
	.axis_map_x = 1,
	.axis_map_y = 0,
	.axis_map_z = 2,
	.negate_x = 0,
	.negate_y = 0,
	.negate_z = 1,
	.init = NULL,
	.exit = NULL,
	.gpio_int1 = -EINVAL,
	.gpio_int2 = -EINVAL,
};

static struct i2c_board_info __initdata lis3dh_i2c_boardinfo[] = {
	{
		I2C_BOARD_INFO(LIS3DH_ACC_DEV_NAME, 0x18),
		.platform_data = &lis3dh_accel,
	},
};
#endif /* CONFIG_STM_LIS3DH */

static struct i2c_registry msm8960_i2c_devices[] __initdata = {
#ifdef CONFIG_ISL9519_CHARGER
	{
		I2C_LIQUID,
		MSM_8930_GSBI10_QUP_I2C_BUS_ID,
		isl_charger_i2c_info,
		ARRAY_SIZE(isl_charger_i2c_info),
	},
#endif /* CONFIG_ISL9519_CHARGER */
#ifdef CONFIG_INPUT_MPU3050
	{
		I2C_FFA | I2C_FLUID,
		MSM_8930_GSBI12_QUP_I2C_BUS_ID,
		mpu3050_i2c_boardinfo,
		ARRAY_SIZE(mpu3050_i2c_boardinfo),
	},
#endif
#ifdef CONFIG_HUAWEI_KERNEL
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
        MSM_8930_GSBI9_QUP_I2C_BUS_ID,
		nfc_bcm2079x,
		ARRAY_SIZE(nfc_bcm2079x),
	},
#endif
#ifdef CONFIG_HUAWEI_NFC_PN544
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_8930_GSBI9_QUP_I2C_BUS_ID,
		nfc_device_info_8930,
		ARRAY_SIZE(nfc_device_info_8930),
	},
#endif
#ifdef CONFIG_QWERTY_KEYPAD_ADP5587
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_8930_GSBI9_QUP_I2C_BUS_ID,
		qwerty_keypad_device_info,
		ARRAY_SIZE(qwerty_keypad_device_info),
	},
#endif
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_8930_GSBI9_QUP_I2C_BUS_ID,
		msm_isa1200_board_info,
		ARRAY_SIZE(msm_isa1200_board_info),
	},
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_8930_GSBI3_QUP_I2C_BUS_ID,
		mxt_device_info_8930,
		ARRAY_SIZE(mxt_device_info_8930),
	},
#ifdef CONFIG_HUAWEI_KERNEL
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_8930_GSBI3_QUP_I2C_BUS_ID,
		touch_device_info_8930,
		ARRAY_SIZE(touch_device_info_8930),
	},
#endif
	{
		I2C_SURF | I2C_FFA | I2C_LIQUID | I2C_FLUID,
		MSM_8930_GSBI9_QUP_I2C_BUS_ID,
		sii_device_info,
		ARRAY_SIZE(sii_device_info),
	},
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_8930_GSBI12_QUP_I2C_BUS_ID,
		sensor_device_info_8930,
		ARRAY_SIZE(sensor_device_info_8930),
	},
#ifdef CONFIG_STM_LIS3DH
	{
		I2C_FFA | I2C_FLUID,
		MSM_8930_GSBI12_QUP_I2C_BUS_ID,
		lis3dh_i2c_boardinfo,
		ARRAY_SIZE(lis3dh_i2c_boardinfo),
	},
#endif
	{
		I2C_SURF | I2C_FFA | I2C_FLUID,
		MSM_8930_GSBI12_QUP_I2C_BUS_ID,
		flash_led_device_info_8930,
		ARRAY_SIZE(flash_led_device_info_8930),
	},
};
#endif /* CONFIG_I2C */

static void __init register_i2c_devices(void)
{
#ifdef CONFIG_I2C
	u8 mach_mask = 0;
	int i;
#ifdef CONFIG_MSM_CAMERA
	struct i2c_registry msm8930_camera_i2c_devices = {
		I2C_SURF | I2C_FFA | I2C_FLUID | I2C_LIQUID | I2C_RUMI,
		MSM_8930_GSBI4_QUP_I2C_BUS_ID,
		msm8930_camera_board_info.board_info,
		msm8930_camera_board_info.num_i2c_board_info,
	};
#endif

	/* Build the matching 'supported_machs' bitmask */
	if (machine_is_msm8930_cdp() || machine_is_msm8627_cdp())
		mach_mask = I2C_SURF;
	else if (machine_is_msm8930_fluid())
		mach_mask = I2C_FLUID;
	else if (machine_is_msm8930_mtp() || machine_is_msm8627_mtp())
		mach_mask = I2C_FFA;
	else
		pr_err("unmatched machine ID in register_i2c_devices\n");

	/* Run the array and install devices as appropriate */
	for (i = 0; i < ARRAY_SIZE(msm8960_i2c_devices); ++i) {
		if (msm8960_i2c_devices[i].machs & mach_mask)
			i2c_register_board_info(msm8960_i2c_devices[i].bus,
						msm8960_i2c_devices[i].info,
						msm8960_i2c_devices[i].len);
	}
#ifdef CONFIG_MSM_CAMERA
	if (msm8930_camera_i2c_devices.machs & mach_mask)
		i2c_register_board_info(msm8930_camera_i2c_devices.bus,
			msm8930_camera_i2c_devices.info,
			msm8930_camera_i2c_devices.len);
#endif
#endif
}

/*Modify the WCD9xxx platform data to support supplies from PM8917 */
static void __init msm8930_pm8917_wcd9xxx_pdata_fixup(
		struct wcd9xxx_pdata *cdc_pdata)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(cdc_pdata->regulator); i++) {

		if (cdc_pdata->regulator[i].name != NULL
			&& strncmp(cdc_pdata->regulator[i].name,
				"CDC_VDD_CP", 10) == 0) {
			cdc_pdata->regulator[i].min_uV =
				cdc_pdata->regulator[i].max_uV = 1800000;
			pr_info("%s: CDC_VDD_CP forced to 1.8 volts for PM8917\n",
				__func__);
			return;
		}
	}
}

/* Modify platform data values to match requirements for PM8917. */
static void __init msm8930_pm8917_pdata_fixup(void)
{
	struct acpuclk_platform_data *pdata;

	msm8930_pm8917_wcd9xxx_pdata_fixup(&sitar_platform_data);
	msm8930_pm8917_wcd9xxx_pdata_fixup(&sitar1p1_platform_data);

	mhl_platform_data.gpio_mhl_power = MHL_POWER_GPIO_PM8917;

	gpio_keys_8930_pdata.buttons = keys_8930_pm8917;
	gpio_keys_8930_pdata.nbuttons = ARRAY_SIZE(keys_8930_pm8917);

	msm_device_saw_core0.dev.platform_data
		= &msm8930_pm8038_saw_regulator_core0_pdata;
	msm_device_saw_core1.dev.platform_data
		= &msm8930_pm8038_saw_regulator_core1_pdata;

	msm8930_device_rpm_regulator.dev.platform_data
		= &msm8930_pm8917_rpm_regulator_pdata;

	pdata = msm8930_device_acpuclk.dev.platform_data;
	pdata->uses_pm8917 = true;
}

#ifdef CONFIG_HW_POWER_TREE
void __init HuaWei_Regulator_Config(void)
{
	static int i;
  config_power_tree_pdata = get_power_config_table();
  //rpm
  msm8930_rpm_regulator_pdata.init_data = config_power_tree_pdata->rpm_power_pdata;
  msm8930_rpm_regulator_pdata.num_regulators = config_power_tree_pdata->rpm_reg_num;
    
  //saw
  msm_saw_regulator_pdata__ = config_power_tree_pdata->saw_power_pdata;
  for(i=0;i<config_power_tree_pdata->saw_reg_num;i++)
  {
  	if(0==strcmp(msm_saw_regulator_pdata__[i].constraints.name,"8038_s5"))
  		msm_device_saw_core0.dev.platform_data = &msm_saw_regulator_pdata__[i];
  	else if(0==strcmp(msm_saw_regulator_pdata__[i].constraints.name,"8038_s6"))
  		msm_device_saw_core1.dev.platform_data = &msm_saw_regulator_pdata__[i];
  	else
  		printk("Some msm saw regulator config error!\n");
  }
  
  //gpio
  msm_gpio_regulator_pdata__ = config_power_tree_pdata->gpio_power_pdata;
  for(i=0;i<config_power_tree_pdata->gpio_reg_num;i++)
  {
  	if(0==strcmp(msm_gpio_regulator_pdata__[i].regulator_name,"ext_5v"))
  		msm8930_device_ext_5v_vreg.dev.platform_data = &msm_gpio_regulator_pdata__[i];
  	else if(0==strcmp(msm_gpio_regulator_pdata__[i].regulator_name,"ext_otg_sw")) //add new gpio power in 104050,20120303
  		msm8930_device_ext_otg_sw_vreg.dev.platform_data = &msm_gpio_regulator_pdata__[i];
  	else
  		printk("Some msm gpio regulator config error!\n");
  }
}
#endif

static void __init msm8930_cdp_init(void)
{
#ifdef CONFIG_HUAWEI_KERNEL
	int i = 0, ret= 0;
#endif
	if (socinfo_get_pmic_model() == PMIC_MODEL_PM8917)
		msm8930_pm8917_pdata_fixup();
	if (meminfo_init(SYS_MEMORY, SZ_256M) < 0)
		pr_err("meminfo_init() failed!\n");

	platform_device_register(&msm_gpio_device);
	msm_tsens_early_init(&msm_tsens_pdata);
	msm_thermal_init(&msm_thermal_pdata);
	if (socinfo_get_pmic_model() != PMIC_MODEL_PM8917) {
		BUG_ON(msm_rpm_init(&msm8930_rpm_data));
		BUG_ON(msm_rpmrs_levels_init(&msm_rpmrs_data));
	} else {
		BUG_ON(msm_rpm_init(&msm8930_rpm_data_pm8917));
		BUG_ON(msm_rpmrs_levels_init(&msm_rpmrs_data_pm8917));
	}

#ifdef CONFIG_HW_POWER_TREE
	HuaWei_Regulator_Config();
#endif

    /* transplant the usb solution from 8x25ICS to 8930JB */
#ifdef CONFIG_HUAWEI_KERNEL
    import_kernel_cmdline();
#endif

	regulator_suppress_info_printing();
	if (msm_xo_init())
		pr_err("Failed to initialize XO votes\n");
	platform_device_register(&msm8930_device_rpm_regulator);
	if (socinfo_get_pmic_model() == PMIC_MODEL_PM8917)
		msm_clock_init(&msm8930_pm8917_clock_init_data);
	else
		msm_clock_init(&msm8930_clock_init_data);

	if (socinfo_get_pmic_model() == PMIC_MODEL_PM8917) {
		/*
		 * By default, set USB mode as USB Peripheral only due to
		 * hardware rework requirement for USB Host Mode.
		 * Provide pmic_id_irq number only if host mode is enable
		 * by user assuming that hardware rework is available.
		 */
		if (enable_usb_host_mode) {
			/* MPP01 IRQ number */
			msm_otg_pdata.pmic_id_irq =
				PM8921_MPP_IRQ(PM8917_IRQ_BASE, 1);
		} else {
			pr_err("Enabling USB Peripheral Only mode.\n");
			msm_otg_pdata.mode = USB_PERIPHERAL;
		}
	} else {
		msm_otg_pdata.pmic_id_irq =
				PM8038_USB_ID_IN_IRQ(PM8038_IRQ_BASE);
	}

	msm_otg_pdata.phy_init_seq = hsusb_phy_init_seq;
	if (msm8930_mhl_display_enabled()) {
		mhl_platform_data.mhl_enabled = true;
		msm_otg_pdata.mhl_enable = true;
	}
	msm8960_device_otg.dev.platform_data = &msm_otg_pdata;
	android_usb_pdata.swfi_latency =
			msm_rpmrs_levels[0].latency_us;
	msm8930_init_gpiomux();
	msm8960_device_qup_spi_gsbi1.dev.platform_data =
				&msm8960_qup_spi_gsbi1_pdata;
	spi_register_board_info(spi_board_info, ARRAY_SIZE(spi_board_info));

	/*
	 * TODO: When physical 8930/PM8038 hardware becomes
	 * available, remove this block or add the config
	 * option.
	 */
#ifndef MSM8930_PHASE_2
	msm8960_init_pmic();
#else
	msm8930_init_pmic();
#endif
	msm8930_i2c_init();
	msm8930_init_gpu();
	msm_spm_init(msm_spm_data, ARRAY_SIZE(msm_spm_data));
	msm_spm_l2_init(msm_spm_l2_data);
	msm8930_init_buses();
	if (cpu_is_msm8627()) {
		platform_add_devices(msm8627_footswitch,
				msm8627_num_footswitch);
	} else {
		if (socinfo_get_pmic_model() == PMIC_MODEL_PM8917)
			platform_add_devices(msm8930_pm8917_footswitch,
					msm8930_pm8917_num_footswitch);
		else
			platform_add_devices(msm8930_footswitch,
					msm8930_num_footswitch);
	}
	if (cpu_is_msm8627())
		platform_device_register(&msm8627_device_acpuclk);
	else if (cpu_is_msm8930())
		platform_device_register(&msm8930_device_acpuclk);
	else if (cpu_is_msm8930aa())
		platform_device_register(&msm8930aa_device_acpuclk);
	platform_add_devices(early_common_devices,
				ARRAY_SIZE(early_common_devices));
	if (socinfo_get_pmic_model() != PMIC_MODEL_PM8917)
		platform_add_devices(pmic_pm8038_devices,
					ARRAY_SIZE(pmic_pm8038_devices));
	else
		platform_add_devices(pmic_pm8917_devices,
					ARRAY_SIZE(pmic_pm8917_devices));
	platform_add_devices(common_devices, ARRAY_SIZE(common_devices));

#ifdef CONFIG_HUAWEI_KERNEL
	/*If the product supports custom keys, then add it*/
	if (0 < get_key_custom_num(KEYNAME_SUPPORT)){
		/*this will make sure not to conflict gpio number on different product/board*/
		for (i = 0; i < gpio_keys_8930_custom_pdata.nbuttons; ++i){
			ret = get_key_custom_num(keys_8930_pm8038_custom[i].desc);
			if (ret == -1)
				continue;
			pr_info("Key %s gpio is configured as:%d\n", keys_8930_pm8038_custom[i].desc, ret);
			keys_8930_pm8038_custom[i].gpio = PM8038_GPIO_PM_TO_SYS(ret);
		}
		platform_device_register(&gpio_keys_8930_custom);
	}
#endif

#ifdef CONFIG_HUAWEI_KERNEL
        if (WIFI_QUALCOMM_WCN3660 == get_hw_wifi_device_type())
        {
            platform_add_devices(wcnss_devices, ARRAY_SIZE(wcnss_devices));
        }
#endif       

	msm8930_add_vidc_device();
	/*
	 * TODO: When physical 8930/PM8038 hardware becomes
	 * available, remove this block or add the config
	 * option.
	 */
#ifndef MSM8930_PHASE_2
	msm8960_pm8921_gpio_mpp_init();
#else
	if (socinfo_get_pmic_model() != PMIC_MODEL_PM8917)
		msm8930_pm8038_gpio_mpp_init();
	else
		msm8930_pm8917_gpio_mpp_init();
#endif
	/* Don't add modem devices on APQ targets */
	if (socinfo_get_id() != 119 && socinfo_get_id() != 157
	    && socinfo_get_id() != 160){
		platform_device_register(&msm_8960_q6_mss_fw);
		platform_device_register(&msm_8960_q6_mss_sw);
	}
/* bluetooth power init */
#ifdef CONFIG_HUAWEI_KERNEL
	bluetooth_power_init();
#endif
	platform_add_devices(cdp_devices, ARRAY_SIZE(cdp_devices));
#ifdef CONFIG_MSM_CAMERA
	msm8930_init_cam();
#endif
	msm8930_init_mmc();
	msm8930_init_audio_acdb();  
#ifdef CONFIG_HUAWEI_KERNEL
    virtualkeys_init();
#else
	mxt_init_vkeys_8930();
#endif
	register_i2c_devices();
#ifdef CONFIG_HUAWEI_KERNEL
	config_debugfs_init();
#endif
	msm8930_init_fb();
#ifdef CONFIG_HUAWEI_GPIO_UNITE 
	hw_gpio_init();
#endif

	slim_register_board_info(msm_slim_devices,
		ARRAY_SIZE(msm_slim_devices));
	change_memory_power = &msm8930_change_memory_power;
	BUG_ON(msm_pm_boot_init(&msm_pm_boot_pdata));
	msm_pm_set_tz_retention_flag(1);

	if (PLATFORM_IS_CHARM25())
		platform_add_devices(mdm_devices, ARRAY_SIZE(mdm_devices));
#ifdef CONFIG_HUAWEI_KERNEL
    hw_extern_sdcard_add_device();
#endif
#ifdef CONFIG_TOUCHSCREEN_CYPRESS_CYTTSP4
    huawei_cyttsp4_init();
#endif
}
#ifdef CONFIG_HUAWEI_KERNEL
static struct resource hw_extern_sdcard_resources[] = {
    {
        .flags  = IORESOURCE_MEM,
    },
};

/* 
 * Define the 'hw_extern_sdcard' device node for MMI sdcard test to  
 * judge if the sd card inserted.
 */
static struct platform_device hw_extern_sdcard_device = {
    .name           = "hw_extern_sdcard",
    .id             = -1,
    .num_resources  = ARRAY_SIZE(hw_extern_sdcard_resources),
    .resource       = hw_extern_sdcard_resources,
};

static struct resource hw_extern_sdcardMounted_resources[] = {
    {
        .flags  = IORESOURCE_MEM,
    },
};

/* 
 * Define the 'hw_extern_sdcardMounted' device node for MMI sdcard test to  
 * judge if the sd card mounted.
 */
static struct platform_device hw_extern_sdcardMounted_device = {
    .name           = "hw_extern_sdcardMounted",
    .id             = -1,
    .num_resources  = ARRAY_SIZE(hw_extern_sdcardMounted_resources),
    .resource       = hw_extern_sdcardMounted_resources,
};

/* 
 * Add the device nodes 'hw_extern_sdcard' and 'hw_extern_sdcardMounted' in /dev. 
 * It is used by MMI sdcard test.
 */
int __init hw_extern_sdcard_add_device(void)
{
    platform_device_register(&hw_extern_sdcard_device);
    platform_device_register(&hw_extern_sdcardMounted_device);
    return 0;
}
#endif

MACHINE_START(MSM8930_CDP, "QCT MSM8930 CDP")
	.map_io = msm8930_map_io,
	.reserve = msm8930_reserve,
	.init_irq = msm8930_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8930_cdp_init,
	.init_early = msm8930_allocate_memory_regions,
	.init_very_early = msm8930_early_memory,
	.restart = msm_restart,
MACHINE_END

MACHINE_START(MSM8930_MTP, "QCT MSM8930 MTP")
	.map_io = msm8930_map_io,
	.reserve = msm8930_reserve,
	.init_irq = msm8930_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8930_cdp_init,
	.init_early = msm8930_allocate_memory_regions,
	.init_very_early = msm8930_early_memory,
	.restart = msm_restart,
MACHINE_END

MACHINE_START(MSM8930_FLUID, "QCT MSM8930 FLUID")
	.map_io = msm8930_map_io,
	.reserve = msm8930_reserve,
	.init_irq = msm8930_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8930_cdp_init,
	.init_early = msm8930_allocate_memory_regions,
	.init_very_early = msm8930_early_memory,
	.restart = msm_restart,
MACHINE_END

MACHINE_START(MSM8627_CDP, "QCT MSM8627 CDP")
	.map_io = msm8930_map_io,
	.reserve = msm8930_reserve,
	.init_irq = msm8930_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8930_cdp_init,
	.init_early = msm8930_allocate_memory_regions,
	.init_very_early = msm8930_early_memory,
	.restart = msm_restart,
MACHINE_END

MACHINE_START(MSM8627_MTP, "QCT MSM8627 MTP")
	.map_io = msm8930_map_io,
	.reserve = msm8930_reserve,
	.init_irq = msm8930_init_irq,
	.handle_irq = gic_handle_irq,
	.timer = &msm_timer,
	.init_machine = msm8930_cdp_init,
	.init_early = msm8930_allocate_memory_regions,
	.init_very_early = msm8930_early_memory,
	.restart = msm_restart,
MACHINE_END
