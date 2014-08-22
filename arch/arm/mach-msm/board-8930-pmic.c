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

#include <linux/interrupt.h>
#include <linux/mfd/pm8xxx/pm8038.h>
#include <linux/mfd/pm8xxx/pm8xxx-adc.h>
#include <linux/msm_ssbi.h>
#include <asm/mach-types.h>
#include <mach/msm_bus_board.h>
#include <mach/restart.h>
#include <mach/socinfo.h>
#include "devices.h"
#include "board-8930.h"
#include <hsad/config_interface.h>

struct pm8xxx_gpio_init {
	unsigned			gpio;
	struct pm_gpio			config;
};

struct pm8xxx_mpp_init {
	unsigned			mpp;
	struct pm8xxx_mpp_config_data	config;
};

#define PM8038_GPIO_INIT(_gpio, _dir, _buf, _val, _pull, _vin, _out_strength, \
			_func, _inv, _disable) \
{ \
	.gpio	= PM8038_GPIO_PM_TO_SYS(_gpio), \
	.config	= { \
		.direction	= _dir, \
		.output_buffer	= _buf, \
		.output_value	= _val, \
		.pull		= _pull, \
		.vin_sel	= _vin, \
		.out_strength	= _out_strength, \
		.function	= _func, \
		.inv_int_pol	= _inv, \
		.disable_pin	= _disable, \
	} \
}

#define PM8038_GPIO_INIT_COPY(_var, _gpio, _dir, _buf, _val, _pull, _vin, _out_strength, \
			_func, _inv, _disable) \
	_var.gpio	= PM8038_GPIO_PM_TO_SYS(_gpio); \
	_var.config.direction	= _dir; \
	_var.config.output_buffer	= _buf; \
	_var.config.output_value	= _val; \
	_var.config.pull		= _pull; \
	_var.config.vin_sel	= _vin; \
	_var.config.out_strength	= _out_strength; \
	_var.config.function	= _func; \
	_var.config.inv_int_pol	= _inv; \
	_var.config.disable_pin	= _disable;

#define PM8038_MPP_INIT(_mpp, _type, _level, _control) \
{ \
	.mpp	= PM8038_MPP_PM_TO_SYS(_mpp), \
	.config	= { \
		.type		= PM8XXX_MPP_TYPE_##_type, \
		.level		= _level, \
		.control	= PM8XXX_MPP_##_control, \
	} \
}

#define PM8038_GPIO_DISABLE(_gpio) \
	PM8038_GPIO_INIT(_gpio, PM_GPIO_DIR_IN, 0, 0, 0, PM8038_GPIO_VIN_L11, \
			 0, 0, 0, 1)

#define PM8038_GPIO_OUTPUT(_gpio, _val) \
	PM8038_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, PM8038_GPIO_VIN_L11, \
			PM_GPIO_STRENGTH_HIGH, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8038_GPIO_INPUT(_gpio, _pull) \
	PM8038_GPIO_INIT(_gpio, PM_GPIO_DIR_IN, PM_GPIO_OUT_BUF_CMOS, 0, \
			_pull, PM8038_GPIO_VIN_L11, \
			PM_GPIO_STRENGTH_NO, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8038_GPIO_OUTPUT_FUNC(_gpio, _val, _func) \
	PM8038_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, PM8038_GPIO_VIN_L11, \
			PM_GPIO_STRENGTH_HIGH, \
			_func, 0, 0)

#define PM8038_GPIO_OUTPUT_VIN(_gpio, _val, _vin) \
	PM8038_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, _vin, \
			PM_GPIO_STRENGTH_HIGH, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8038_GPIO_INPUT_COPY(_var, _gpio, _pull) \
	PM8038_GPIO_INIT_COPY(_var, _gpio, PM_GPIO_DIR_IN, PM_GPIO_OUT_BUF_CMOS, 0, \
			_pull, PM8038_GPIO_VIN_L11, \
			PM_GPIO_STRENGTH_NO, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8038_GPIO_OUTPUT_VIN_COPY(_var, _gpio, _val, _vin) \
	PM8038_GPIO_INIT_COPY(_var, _gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, _vin, \
			PM_GPIO_STRENGTH_HIGH, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8917_GPIO_INIT(_gpio, _dir, _buf, _val, _pull, _vin, _out_strength, \
			_func, _inv, _disable) \
{ \
	.gpio	= PM8917_GPIO_PM_TO_SYS(_gpio), \
	.config	= { \
		.direction	= _dir, \
		.output_buffer	= _buf, \
		.output_value	= _val, \
		.pull		= _pull, \
		.vin_sel	= _vin, \
		.out_strength	= _out_strength, \
		.function	= _func, \
		.inv_int_pol	= _inv, \
		.disable_pin	= _disable, \
	} \
}

#define PM8917_MPP_INIT(_mpp, _type, _level, _control) \
{ \
	.mpp	= PM8917_MPP_PM_TO_SYS(_mpp), \
	.config	= { \
		.type		= PM8XXX_MPP_TYPE_##_type, \
		.level		= _level, \
		.control	= PM8XXX_MPP_##_control, \
	} \
}

#define PM8917_GPIO_DISABLE(_gpio) \
	PM8917_GPIO_INIT(_gpio, PM_GPIO_DIR_IN, 0, 0, 0, PM_GPIO_VIN_S4, \
			 0, 0, 0, 1)

#define PM8917_GPIO_OUTPUT(_gpio, _val) \
	PM8917_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_HIGH, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8917_GPIO_INPUT(_gpio, _pull) \
	PM8917_GPIO_INIT(_gpio, PM_GPIO_DIR_IN, PM_GPIO_OUT_BUF_CMOS, 0, \
			_pull, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_NO, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

#define PM8917_GPIO_OUTPUT_FUNC(_gpio, _val, _func) \
	PM8917_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, PM_GPIO_VIN_S4, \
			PM_GPIO_STRENGTH_HIGH, \
			_func, 0, 0)

#define PM8917_GPIO_OUTPUT_VIN(_gpio, _val, _vin) \
	PM8917_GPIO_INIT(_gpio, PM_GPIO_DIR_OUT, PM_GPIO_OUT_BUF_CMOS, _val, \
			PM_GPIO_PULL_NO, _vin, \
			PM_GPIO_STRENGTH_HIGH, \
			PM_GPIO_FUNC_NORMAL, 0, 0)

/* GPIO and MPP configurations for MSM8930 + PM8038 targets */

/* Initial PM8038 GPIO configurations */
static struct pm8xxx_gpio_init pm8038_gpios[] __initdata = {
	/* keys GPIOs */
	PM8038_GPIO_INPUT(3, PM_GPIO_PULL_UP_30),
	PM8038_GPIO_INPUT(8, PM_GPIO_PULL_UP_30),
	PM8038_GPIO_INPUT(10, PM_GPIO_PULL_UP_30),
	PM8038_GPIO_INPUT(11, PM_GPIO_PULL_UP_30),
#ifdef CONFIG_HUAWEI_KERNEL
    /* set gpio7 to input mode to work as volume- */
	/* haptics gpio */
    PM8038_GPIO_INPUT(7, PM_GPIO_PULL_UP_30),
#else
	/* haptics gpio */
	PM8038_GPIO_OUTPUT_FUNC(7, 0, PM_GPIO_FUNC_1),
#endif
#ifdef CONFIG_HUAWEI_KERNEL
	PM8038_GPIO_OUTPUT_VIN(1, 0, PM8038_GPIO_VIN_VPH),
	PM8038_GPIO_OUTPUT_VIN(2, 0, PM8038_GPIO_VIN_VPH),
	PM8038_GPIO_OUTPUT_VIN(5, 0, PM8038_GPIO_VIN_VPH),
	PM8038_GPIO_OUTPUT_VIN(6, 0, PM8038_GPIO_VIN_VPH),
	/* config GPIO8 of PMIC as sleep clock of BCM4330 */
	PM8038_GPIO_OUTPUT_FUNC(8, 0, PM8038_GPIO_VIN_VPH),
	PM8038_GPIO_OUTPUT_VIN(9, 0, PM8038_GPIO_VIN_VPH),
	/*config GPIO12 for VSP, VSN power source*/
	PM8038_GPIO_OUTPUT_FUNC(12, 1, PM_GPIO_FUNC_NORMAL),
#else
	/* MHL PWR EN */
	PM8038_GPIO_OUTPUT_VIN(5, 1, PM8038_GPIO_VIN_VPH),
#endif	
};
/* BCM4330 sleep clock config */
#ifdef CONFIG_HUAWEI_KERNEL
static struct pm8xxx_gpio_init pm8038_gpios_hw[] __initdata = {
	PM8038_GPIO_OUTPUT_FUNC(8, 0, PM_GPIO_FUNC_1),
};
#endif

/* Initial PM8038 MPP configurations */
static struct pm8xxx_mpp_init pm8038_mpps[] __initdata = {
    PM8038_MPP_INIT(6, D_INPUT, 0, DIN_TO_DBUS1),
};

/* GPIO and MPP configurations for MSM8930 + PM8917 targets */

/* Initial PM8917 GPIO configurations */
static struct pm8xxx_gpio_init pm8917_gpios[] __initdata = {
	/* Backlight enable control */
	PM8917_GPIO_OUTPUT(24, 1),
	/* keys GPIOs */
	PM8917_GPIO_INPUT(27, PM_GPIO_PULL_UP_30),
	PM8917_GPIO_INPUT(28, PM_GPIO_PULL_UP_30),
	PM8917_GPIO_INPUT(36, PM_GPIO_PULL_UP_30),
	PM8917_GPIO_INPUT(37, PM_GPIO_PULL_UP_30),
	/* haptics gpio */
	PM8917_GPIO_OUTPUT_FUNC(38, 0, PM_GPIO_FUNC_2),
	/* MHL PWR EN */
	PM8917_GPIO_OUTPUT_VIN(25, 1, PM_GPIO_VIN_VPH),
};

/* Initial PM8917 MPP configurations */
static struct pm8xxx_mpp_init pm8917_mpps[] __initdata = {
	/* Configure MPP01 for USB ID detection */
	PM8917_MPP_INIT(1, D_INPUT, PM8921_MPP_DIG_LEVEL_S4, DIN_TO_INT),
};

#ifdef CONFIG_HUAWEI_GPIO_UNITE
static struct pm8xxx_gpio_init hw_pm8038_gpios[PM8038_GPIO_NUM];
static int hw_pm8038_gpio_init(void)
{
	int i;
	int rc;
	int config_index = 0;
	struct pm_gpio_cfg_t* pPMGpioCfg = get_pm_gpio_config_table();	

	if(NULL == pPMGpioCfg)
	{
		pr_err(KERN_ERR "get pm gpio config table failed \n"); 
		return -EFAULT;
	}

	for(i = 0; i < PM8038_GPIO_NUM; i++)
	{	
		if(pPMGpioCfg[i].cfg.direction != NOSET)
		{
			hw_pm8038_gpios[config_index].gpio =  pPMGpioCfg[i].gpio_number;
			hw_pm8038_gpios[config_index].config = pPMGpioCfg[i].cfg;

			rc = pm8xxx_gpio_config(PM8038_GPIO_PM_TO_SYS(hw_pm8038_gpios[config_index].gpio),  \
				&hw_pm8038_gpios[config_index].config);
			if (rc) 
			{
				pr_err("%s: pm8xxx_gpio_config: rc=%d\n", __func__, rc);
				break;
			}

			config_index++;
		}
	}	
	return 0;
}
#endif
#ifdef CONFIG_HUAWEI_KERNEL
static void __init hw_msm8930_gpio_init(void)
{
	int i = 0;
	int rc = -1;
	int bt_fm_chip_type = (int)BT_FM_UNKNOWN_DEVICE;
	
    /* get bt/fm chip type from the device feature configuration (.xml file) */
    bt_fm_chip_type = get_bt_fm_device_type();
    if(-1 == bt_fm_chip_type)
    {
		pr_err("%s: Get Bt chip type failed.\n",__func__);
	    return;
    }

	pr_err("%s: Current Bt chip is %d.\n",__func__,bt_fm_chip_type);

	if(BT_FM_BROADCOM_BCM4330 != bt_fm_chip_type)
	{
		return;
	}

	for (i = 0; i < ARRAY_SIZE(pm8038_gpios_hw); i++) 
	{
		rc = pm8xxx_gpio_config(pm8038_gpios_hw[i].gpio,
					&pm8038_gpios_hw[i].config);
		if (rc) 
		{
			pr_err("%s: pm8xxx_gpio_config: rc=%d\n", __func__, rc);
			break;
		}
	}
    pr_err("%s: bcm sleep clock init succed.\n",__func__);
	return;
}
#endif
void __init msm8930_pm8038_gpio_mpp_init(void)
{
	int i, rc;
	int number=0;

	/*if product config need to change the type, it will read and change it*/
	for (i = 0; i < ARRAY_SIZE(pm8038_gpios); i++) {
			number = pm8038_gpios[i].gpio - PM8038_GPIO_BASE + 1;
			switch (get_pmic_gpio_type(number)){
			case PMIC_GPIO_INPUT:
				printk("changing pmic gpio=%d to PMIC_GPIO_INPUT\n", number);
				PM8038_GPIO_INPUT_COPY(pm8038_gpios[i], number, PM_GPIO_PULL_UP_30);
				break;
			case PMIC_GPIO_OUTPUT:
				printk("changing pmic gpio=%d to PMIC_GPIO_OUTPUT\n", number);
				PM8038_GPIO_OUTPUT_VIN_COPY(pm8038_gpios[i], number, 0, PM8038_GPIO_VIN_VPH);
				break;
			default:
				break;
		}

		rc = pm8xxx_gpio_config(pm8038_gpios[i].gpio,
					&pm8038_gpios[i].config);
		if (rc) {
			pr_err("%s: pm8xxx_gpio_config: rc=%d\n", __func__, rc);
			break;
		}
	}

	/* Initial MPP configuration. */
	for (i = 0; i < ARRAY_SIZE(pm8038_mpps); i++) {
		rc = pm8xxx_mpp_config(pm8038_mpps[i].mpp,
					&pm8038_mpps[i].config);
		if (rc) {
			pr_err("%s: pm8xxx_mpp_config: rc=%d\n", __func__, rc);
			break;
		}
	}

	#ifdef CONFIG_HUAWEI_GPIO_UNITE
	rc = hw_pm8038_gpio_init();
	if (rc) 
	{
		pr_err("%s: hw_pm8038_gpio_init: rc=%d\n", __func__, rc);
	}
	#endif
#ifdef CONFIG_HUAWEI_KERNEL
	hw_msm8930_gpio_init();
#endif
}

void __init msm8930_pm8917_gpio_mpp_init(void)
{
	int i, rc;

	for (i = 0; i < ARRAY_SIZE(pm8917_gpios); i++) {
		rc = pm8xxx_gpio_config(pm8917_gpios[i].gpio,
					&pm8917_gpios[i].config);
		if (rc) {
			pr_err("%s: pm8xxx_gpio_config: rc=%d\n", __func__, rc);
			break;
		}
	}

	/* Initial MPP configuration. */
	for (i = 0; i < ARRAY_SIZE(pm8917_mpps); i++) {
		rc = pm8xxx_mpp_config(pm8917_mpps[i].mpp,
					&pm8917_mpps[i].config);
		if (rc) {
			pr_err("%s: pm8xxx_mpp_config: rc=%d\n", __func__, rc);
			break;
		}
	}
}

static struct pm8xxx_adc_amux pm8038_adc_channels_data[] = {
	{"vcoin", CHANNEL_VCOIN, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"vbat", CHANNEL_VBAT, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"dcin", CHANNEL_DCIN, CHAN_PATH_SCALING4, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"ichg", CHANNEL_ICHG, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"vph_pwr", CHANNEL_VPH_PWR, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"ibat", CHANNEL_IBAT, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"batt_therm", CHANNEL_BATT_THERM, CHAN_PATH_SCALING1, AMUX_RSV2,
		ADC_DECIMATION_TYPE2, ADC_SCALE_BATT_THERM},
	{"batt_id", CHANNEL_BATT_ID, CHAN_PATH_SCALING1, AMUX_RSV2,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"usbin", CHANNEL_USBIN, CHAN_PATH_SCALING3, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"pmic_therm", CHANNEL_DIE_TEMP, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_PMIC_THERM},
	{"625mv", CHANNEL_625MV, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"125v", CHANNEL_125V, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"chg_temp", CHANNEL_CHG_TEMP, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"pa_therm1", ADC_MPP_1_AMUX4, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_PA_THERM},
	{"xo_therm", CHANNEL_MUXOFF, CHAN_PATH_SCALING1, AMUX_RSV0,
		ADC_DECIMATION_TYPE2, ADC_SCALE_XOTHERM},
	{"pa_therm0", ADC_MPP_1_AMUX3, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_PA_THERM},
	/* use ADC_MPP_1_AMUX6 channel to read pa_therm_mpp5 */
#ifdef CONFIG_HUAWEI_KERNEL
	{"pa_therm_mpp5", ADC_MPP_1_AMUX6, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_PA_THERM},
#endif
};

static struct pm8xxx_adc_properties pm8038_adc_data = {
	.adc_vdd_reference	= 1800, /* milli-voltage for this adc */
	.bitresolution		= 15,
	.bipolar                = 0,
};

static struct pm8xxx_adc_platform_data pm8038_adc_pdata = {
	.adc_channel            = pm8038_adc_channels_data,
	.adc_num_board_channel  = ARRAY_SIZE(pm8038_adc_channels_data),
	.adc_prop               = &pm8038_adc_data,
	.adc_mpp_base		= PM8038_MPP_PM_TO_SYS(1),
};

static struct pm8xxx_irq_platform_data pm8xxx_irq_pdata __devinitdata = {
	.irq_base		= PM8038_IRQ_BASE,
	.devirq			= MSM_GPIO_TO_INT(104),
	.irq_trigger_flag	= IRQF_TRIGGER_LOW,
};

static struct pm8xxx_gpio_platform_data pm8xxx_gpio_pdata __devinitdata = {
	.gpio_base	= PM8038_GPIO_PM_TO_SYS(1),
};

static struct pm8xxx_mpp_platform_data pm8xxx_mpp_pdata __devinitdata = {
	.mpp_base	= PM8038_MPP_PM_TO_SYS(1),
};

static struct pm8xxx_rtc_platform_data pm8xxx_rtc_pdata __devinitdata = {
#ifdef CONFIG_HUAWEI_FEATURE_POWEROFF_ALARM
	/* do not write the time into rtc register, use timerservice to set offset */
	.rtc_write_enable	= false,
	.rtc_alarm_powerup	= true,
#else
	.rtc_write_enable	= false,
	.rtc_alarm_powerup	= false,
#endif
};

static struct pm8xxx_pwrkey_platform_data pm8xxx_pwrkey_pdata = {
	.pull_up		= 1,
	.kpd_trigger_delay_us	= 15625,
	.wakeup			= 1,
};

static int pm8921_therm_mitigation[] = {
	1100,
	700,
	600,
	325,
};

#define MAX_VOLTAGE_MV		4200
#define CHG_TERM_MA		100
static struct pm8921_charger_platform_data pm8921_chg_pdata __devinitdata = {
	.update_time		= 60000,
	.max_voltage		= MAX_VOLTAGE_MV,
	.min_voltage		= 3200,
	.uvd_thresh_voltage	= 4050,
	.alarm_low_mv		= 3400,
	.alarm_high_mv		= 4000,
	.resume_voltage_delta	= 60,
	.resume_charge_percent	= 99,
	.term_current		= CHG_TERM_MA,
	.cool_temp		= 10,
	.warm_temp		= 40,
	.temp_check_period	= 1,
	.max_bat_chg_current	= 1100,
	.cool_bat_chg_current	= 350,
	.warm_bat_chg_current	= 350,
	.cool_bat_voltage	= 4100,
	.warm_bat_voltage	= 4100,
	.thermal_mitigation	= pm8921_therm_mitigation,
	.thermal_levels		= ARRAY_SIZE(pm8921_therm_mitigation),
	.led_src_config		= LED_SRC_VPH_PWR,
	.rconn_mohm		= 18,
};

#define PM8038_WLED_MAX_CURRENT		25
#define PM8XXX_LED_PWM_PERIOD		1000
#define PM8XXX_LED_PWM_DUTY_MS		20
#define PM8038_RGB_LED_MAX_CURRENT	12

#ifdef CONFIG_HUAWEI_KERNEL
static struct led_info pm8038_led_info[] = {
	[0] = {
		.name			= "wled",
		.default_trigger	= "bkl_trigger",
	},
	[1] = {
		.name			= "red",
	},
	[2] = {
		.name			= "green",
	},
	[3] = {
		.name			= "blue",
	},
	[4] = {
		.name			= "button-backlight",
		.gpio			= PM8038_MPP_PM_TO_SYS(3),
	},
	[5] = {
		.name			= "keyboard-backlight",
		.gpio			= PM8038_MPP_PM_TO_SYS(4),
	},
	[6] = {
		.name			= "ptt_led",
		.gpio			= PM8038_MPP_PM_TO_SYS(4),
	},
};
#else
static struct led_info pm8038_led_info[] = {
	[0] = {
		.name			= "wled",
		.default_trigger	= "bkl_trigger",
	},
	[1] = {
		.name			= "led:rgb_red",
		.default_trigger	= "battery-charging",
	},
	[2] = {
		.name			= "led:rgb_green",
	},
	[3] = {
		.name			= "led:rgb_blue",
	},
};
#endif

static struct led_platform_data pm8038_led_core_pdata = {
	.num_leds = ARRAY_SIZE(pm8038_led_info),
	.leds = pm8038_led_info,
};

static struct wled_config_data wled_cfg = {
	.dig_mod_gen_en = true,
	.cs_out_en = true,
	.ctrl_delay_us = 0,
	#ifdef CONFIG_FB_PM8038_CABC_PIN
	.cabc_en = true,
	#endif
	.op_fdbck = true,
	.ovp_val = WLED_OVP_32V,
	.boost_curr_lim = WLED_CURR_LIMIT_525mA,
	.num_strings = 2,
};
/*disable this code because we don't use breathing led*/
#ifndef CONFIG_HUAWEI_KERNEL
static int pm8038_led0_pwm_duty_pcts[56] = {
		1, 4, 8, 12, 16, 20, 24, 28, 32, 36,
		40, 44, 46, 52, 56, 60, 64, 68, 72, 76,
		80, 84, 88, 92, 96, 100, 100, 100, 98, 95,
		92, 88, 84, 82, 78, 74, 70, 66, 62, 58,
		58, 54, 50, 48, 42, 38, 34, 30, 26, 22,
		14, 10, 6, 4, 1
};

/*
 * Note: There is a bug in LPG module that results in incorrect
 * behavior of pattern when LUT index 0 is used. So effectively
 * there are 63 usable LUT entries.
 */
static struct pm8xxx_pwm_duty_cycles pm8038_led0_pwm_duty_cycles = {
	.duty_pcts = (int *)&pm8038_led0_pwm_duty_pcts,
	.num_duty_pcts = ARRAY_SIZE(pm8038_led0_pwm_duty_pcts),
	.duty_ms = PM8XXX_LED_PWM_DUTY_MS,
	.start_idx = 1,
};
#endif
static struct pm8xxx_led_config pm8038_led_configs[] = {
	[0] = {
		.id = PM8XXX_ID_WLED,
		.mode = PM8XXX_LED_MODE_MANUAL,
		.max_current = PM8038_WLED_MAX_CURRENT - 1, //set to 24mA
		/*open wled module*/
		.default_state = 1,
		.wled_cfg = &wled_cfg,
	},
	[1] = {
		.id = PM8XXX_ID_RGB_LED_RED,
		.mode = PM8XXX_LED_MODE_PWM1,
		.max_current = PM8038_RGB_LED_MAX_CURRENT,
		.pwm_channel = 5,
		.pwm_period_us = PM8XXX_LED_PWM_PERIOD,
		#ifndef CONFIG_HUAWEI_KERNEL
		.pwm_duty_cycles = &pm8038_led0_pwm_duty_cycles,
		#endif
	},
	[2] = {
		.id = PM8XXX_ID_RGB_LED_GREEN,
		.mode = PM8XXX_LED_MODE_PWM1,
		.max_current = PM8038_RGB_LED_MAX_CURRENT,
		.pwm_channel = 4,
		.pwm_period_us = PM8XXX_LED_PWM_PERIOD,
		#ifndef CONFIG_HUAWEI_KERNEL
		.pwm_duty_cycles = &pm8038_led0_pwm_duty_cycles,
		#endif
	},
	[3] = {
		.id = PM8XXX_ID_RGB_LED_BLUE,
		.mode = PM8XXX_LED_MODE_PWM1,
		.max_current = PM8038_RGB_LED_MAX_CURRENT,
		.pwm_channel = 3,
		.pwm_period_us = PM8XXX_LED_PWM_PERIOD,
		#ifndef CONFIG_HUAWEI_KERNEL
		.pwm_duty_cycles = &pm8038_led0_pwm_duty_cycles,
		#endif
	},
#ifdef CONFIG_HUAWEI_KERNEL
	[4] = {
		.id = PM8XXX_ID_LED_KB_LIGHT,
		.mode = PM8XXX_LED_MODE_MANUAL,
	},
	[5] = {
		.id = PM8XXX_ID_LED_0,  //use this ID for qwerty keyboard
		.mode = PM8XXX_LED_MODE_MANUAL,
	},
#endif
	[6] = {
		.id = PM8XXX_ID_LED_1,  //share kb and PTT as it uses same MPP4
		.mode = PM8XXX_LED_MODE_MANUAL,
	},
};
static struct pm8xxx_led_platform_data pm8xxx_leds_pdata = {
	.led_core = &pm8038_led_core_pdata,
	.configs = pm8038_led_configs,
	.num_configs = ARRAY_SIZE(pm8038_led_configs),
};

static struct pm8xxx_ccadc_platform_data pm8xxx_ccadc_pdata = {
	.r_sense_uohm		= 10000,
	.calib_delay_ms		= 600000,
};

static struct pm8xxx_misc_platform_data pm8xxx_misc_pdata = {
	.priority		= 0,
};

/*
 *	0x254=0xC8 (Threshold=110, preamp bias=01)
 *	0x255=0xC1 (Hold=110, max attn=0000, mute=1)
 *	0x256=0xB0 (decay=101, attack=10, delay=0)
 */

static struct pm8xxx_spk_platform_data pm8xxx_spk_pdata = {
	.spk_add_enable		= false,
    /* Case 01037575 Reduce the noise gate threshold to fix music intermittent problem*/
	.cd_ng_threshold	= 0x0,
	.cd_nf_preamp_bias	= 0x1,
	.cd_ng_hold		= 0x6,
	.cd_ng_max_atten	= 0x0,
	.noise_mute		= 1,
	.cd_ng_decay_rate	= 0x5,
	.cd_ng_attack_rate	= 0x2,
	.cd_delay		= 0x0,
};

static struct pm8921_bms_platform_data pm8921_bms_pdata __devinitdata = {
	.battery_type			= BATT_UNKNOWN,
	.r_sense_uohm			= 10000,
	.v_cutoff			= 3400,
	.max_voltage_uv			= MAX_VOLTAGE_MV * 1000,
	.shutdown_soc_valid_limit	= 20,
	.adjust_soc_low_threshold	= 25,
	.chg_term_ua			= CHG_TERM_MA * 1000,
	.rconn_mohm			= 18,
	.normal_voltage_calc_ms		= 20000,
	.low_voltage_calc_ms		= 1000,
	.alarm_low_mv			= 3400,
	.alarm_high_mv			= 4000,
};

#ifdef CONFIG_HUAWEI_KERNEL
struct pm8xxx_vibrator_platform_data pm8xxx_vibrator_pdata = {
    .initial_vibrate_ms = 500,
    .max_timeout_ms     = 15000,//15 s
    .min_timeout_ms     = 10,//10 ms
    .level_mV           = 3000,
};
#endif
static struct pm8038_platform_data pm8038_platform_data __devinitdata = {
	.irq_pdata		= &pm8xxx_irq_pdata,
	.gpio_pdata		= &pm8xxx_gpio_pdata,
	.mpp_pdata		= &pm8xxx_mpp_pdata,
	.rtc_pdata              = &pm8xxx_rtc_pdata,
	.pwrkey_pdata		= &pm8xxx_pwrkey_pdata,
	.misc_pdata		= &pm8xxx_misc_pdata,
	.regulator_pdatas	= msm8930_pm8038_regulator_pdata,
	.charger_pdata		= &pm8921_chg_pdata,
	.bms_pdata		= &pm8921_bms_pdata,
	.adc_pdata		= &pm8038_adc_pdata,
	.leds_pdata		= &pm8xxx_leds_pdata,
	.ccadc_pdata		= &pm8xxx_ccadc_pdata,
	.spk_pdata		= &pm8xxx_spk_pdata,
#ifdef CONFIG_HUAWEI_KERNEL
	.vibrator_pdata = &pm8xxx_vibrator_pdata,
#endif
};

static struct msm_ssbi_platform_data msm8930_ssbi_pm8038_pdata __devinitdata = {
	.controller_type = MSM_SBI_CTRL_PMIC_ARBITER,
	.slave	= {
		.name			= "pm8038-core",
		.platform_data		= &pm8038_platform_data,
	},
};

/* PM8917 platform data */

static struct pm8xxx_adc_amux pm8917_adc_channels_data[] = {
	{"vcoin", CHANNEL_VCOIN, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"vbat", CHANNEL_VBAT, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"dcin", CHANNEL_DCIN, CHAN_PATH_SCALING4, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"ichg", CHANNEL_ICHG, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"vph_pwr", CHANNEL_VPH_PWR, CHAN_PATH_SCALING2, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"ibat", CHANNEL_IBAT, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"batt_therm", CHANNEL_BATT_THERM, CHAN_PATH_SCALING1, AMUX_RSV2,
		ADC_DECIMATION_TYPE2, ADC_SCALE_BATT_THERM},
	{"batt_id", CHANNEL_BATT_ID, CHAN_PATH_SCALING1, AMUX_RSV2,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"usbin", CHANNEL_USBIN, CHAN_PATH_SCALING3, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"pmic_therm", CHANNEL_DIE_TEMP, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_PMIC_THERM},
	{"625mv", CHANNEL_625MV, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"125v", CHANNEL_125V, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"chg_temp", CHANNEL_CHG_TEMP, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_DEFAULT},
	{"xo_therm", CHANNEL_MUXOFF, CHAN_PATH_SCALING1, AMUX_RSV0,
		ADC_DECIMATION_TYPE2, ADC_SCALE_XOTHERM},
	{"pa_therm0", ADC_MPP_1_AMUX3, CHAN_PATH_SCALING1, AMUX_RSV1,
		ADC_DECIMATION_TYPE2, ADC_SCALE_PA_THERM},
};

static struct pm8xxx_adc_properties pm8917_adc_data = {
	.adc_vdd_reference	= 1800, /* milli-voltage for this adc */
	.bitresolution		= 15,
	.bipolar                = 0,
};

static struct pm8xxx_adc_platform_data pm8917_adc_pdata = {
	.adc_channel            = pm8917_adc_channels_data,
	.adc_num_board_channel  = ARRAY_SIZE(pm8917_adc_channels_data),
	.adc_prop               = &pm8917_adc_data,
	.adc_mpp_base		= PM8917_MPP_PM_TO_SYS(1),
};

static struct pm8921_platform_data pm8917_platform_data __devinitdata = {
	.irq_pdata		= &pm8xxx_irq_pdata,
	.gpio_pdata		= &pm8xxx_gpio_pdata,
	.mpp_pdata		= &pm8xxx_mpp_pdata,
	.rtc_pdata              = &pm8xxx_rtc_pdata,
	.pwrkey_pdata		= &pm8xxx_pwrkey_pdata,
	.misc_pdata		= &pm8xxx_misc_pdata,
	.regulator_pdatas	= msm8930_pm8917_regulator_pdata,
	.charger_pdata		= &pm8921_chg_pdata,
	.bms_pdata		= &pm8921_bms_pdata,
	.adc_pdata		= &pm8917_adc_pdata,
	.ccadc_pdata		= &pm8xxx_ccadc_pdata,
};

static struct msm_ssbi_platform_data msm8930_ssbi_pm8917_pdata __devinitdata = {
	.controller_type = MSM_SBI_CTRL_PMIC_ARBITER,
	.slave	= {
		.name			= "pm8921-core",
		.platform_data		= &pm8917_platform_data,
	},
};

void __init msm8930_init_pmic(void)
{
	if (socinfo_get_pmic_model() != PMIC_MODEL_PM8917) {
		/* PM8038 configuration */
		pmic_reset_irq = PM8038_IRQ_BASE + PM8038_RESOUT_IRQ;
		msm8960_device_ssbi_pmic.dev.platform_data =
					&msm8930_ssbi_pm8038_pdata;
		pm8038_platform_data.num_regulators
			= msm8930_pm8038_regulator_pdata_len;
		if (machine_is_msm8930_mtp())
			pm8921_bms_pdata.battery_type = BATT_PALLADIUM;
		else if (machine_is_msm8930_cdp())
			pm8921_chg_pdata.has_dc_supply = true;
	} else {
		/* PM8917 configuration */
		pmic_reset_irq = PM8917_IRQ_BASE + PM8921_RESOUT_IRQ;
		msm8960_device_ssbi_pmic.dev.platform_data =
					&msm8930_ssbi_pm8917_pdata;
		pm8917_platform_data.num_regulators
			= msm8930_pm8917_regulator_pdata_len;
		if (machine_is_msm8930_mtp())
			pm8921_bms_pdata.battery_type = BATT_PALLADIUM;
		else if (machine_is_msm8930_cdp())
			pm8921_chg_pdata.has_dc_supply = true;
	}

/* modify for 1.7232 baseline upgrade */
}
