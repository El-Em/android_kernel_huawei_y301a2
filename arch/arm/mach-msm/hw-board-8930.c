#include "hw-board-8930.h"
#include "board-8930.h"
#ifdef CONFIG_HUAWEI_KERNEL
#include <hsad/config_interface.h>
#include <linux/touch_platform_config.h>

#ifdef CONFIG_INPUT_HW_ATE
char buf_virtualkeys[500];
ssize_t  buf_vkey_size=0;
#else
static char buf_virtualkeys[500];
static ssize_t  buf_vkey_size=0;
#endif
#endif

#ifdef CONFIG_HUAWEI_NFC_PN544
static struct msm_xo_voter *nfc_clock = NULL;

static struct pm_gpio nfc_ven_enable = {
    .direction        = PM_GPIO_DIR_OUT,
    .output_buffer    = PM_GPIO_OUT_BUF_CMOS,
    .output_value     = 1,
    .pull             = PM_GPIO_PULL_NO,
    .vin_sel          = PM8038_GPIO_VIN_L11,
    .out_strength     = PM_GPIO_STRENGTH_HIGH,
    .function         = PM_GPIO_FUNC_NORMAL,
    .inv_int_pol      = 0,
    .disable_pin      = 0,
};


static struct pm_gpio nfc_ven_disable = {
    .direction        = PM_GPIO_DIR_OUT,
    .output_buffer    = PM_GPIO_OUT_BUF_CMOS,
    .output_value     = 0,
    .pull             = PM_GPIO_PULL_NO,
    .vin_sel          = PM8038_GPIO_VIN_L11,
    .out_strength     = PM_GPIO_STRENGTH_LOW,
    .function         = PM_GPIO_FUNC_NORMAL,
    .inv_int_pol      = 0,
    .disable_pin      = 0,
};
#endif

#ifdef CONFIG_HUAWEI_KERNEL
atomic_t touch_detected_yet = ATOMIC_INIT(0);
int power_switch(int pm)
{
    static struct regulator *regulator_VDD = NULL;
    static struct regulator *regulator_VBUS = NULL;
    int ret = 0;
    char product_type[20] = {0};

    get_product_type(product_type, sizeof(product_type));
    
    if(IC_PM_ON == pm)
    {
    /*8930 Big-board use Lvs2 and the future mini-board will use L11*/
        if(0 == strncmp(product_type,"MSM930",sizeof(product_type)))
        {
            /*Lvs2 is not programable, it always output 1.8V when enabled*/
            regulator_VBUS = regulator_get(NULL, "8038_lvs2");
            if(IS_ERR(regulator_VBUS))
            {
                pr_err("%s:Lvs2 power init get failed\n", __func__);
                return -EPERM;
            }
            ret = regulator_enable(regulator_VBUS);
            if(ret != 0)
            {
                pr_err("%s:Lvs2 regulator_enable failed\n", __func__);
                return -EPERM;
            }
        }
        else
        {
            regulator_VBUS = regulator_get(NULL, "8038_l11");
            if(IS_ERR(regulator_VBUS))
            {
                pr_err("%s:L11 power init get failed\n", __func__);
                return -EPERM;
            }
            ret = regulator_set_voltage(regulator_VBUS, IO_PM_VOLTAGE, IO_PM_VOLTAGE);
            if(ret != 0)
            {
                pr_err("%s:L11 regulator_set_voltage failed\n", __func__);
                return -EPERM;
            }
            ret = regulator_enable(regulator_VBUS);
            if(ret != 0)
            {
                pr_err("%s:L11 regulator_enable failed\n", __func__);
                return -EPERM;
            }
        }
        //mdelay(5);

        
        regulator_VDD = regulator_get(NULL,"8038_l9");
        if(IS_ERR(regulator_VDD))
        {
            pr_err("%s:L9 power init get failed\n", __func__);
            return -EPERM;
        }
        ret = regulator_set_voltage(regulator_VDD, IC_PM_VDD, IC_PM_VDD);
        if(ret != 0)
        {
            pr_err("%s:L9 regulator_set_voltage faild\n",__func__);
            return -EPERM;
        }
        ret = regulator_enable(regulator_VDD);
        if(ret != 0) 
        {
            pr_err("%s:L9 regulator_enable failed\n", __func__);
            return -EPERM;
        }
        
        //msleep(50);
    }
    else if(IC_PM_OFF == pm)
    {
        if(NULL != regulator_VDD)
        {
            ret = regulator_disable(regulator_VDD);
            if(ret != 0)
            {
                pr_err("%s:L9 power disable failed \n", __func__);
            }
        }
        if(NULL != regulator_VBUS)
        {
            ret = regulator_disable(regulator_VBUS);
            if(ret != 0)
            {
                pr_err("%s:VBUS power disable failed \n", __func__);
            }
        }
    }
    else 
    {
        ret = -EPERM;
        pr_err("%s: !ERROR! TP power switch not support yet!\n", __func__);	
    }
	return ret;
}
int set_touch_interrupt_gpio(void)
{
    int ret = 0;
    static bool is_gpio_requested = false;
    if(false == is_gpio_requested)
    {
        ret = gpio_request(MSM_8930_TOUCH_INT_PIN, "TOUCH_INT");
        if(ret != 0)
        {
            printk("%s: Failed to get gpio %d. code: %d.\n",
                __func__, MSM_8930_TOUCH_INT_PIN, ret);
            return ret;
        }

        ret = gpio_tlmm_config(GPIO_CFG(MSM_8930_TOUCH_INT_PIN, 
                            0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, 
                            GPIO_CFG_2MA), GPIO_CFG_ENABLE);
        if(ret != 0)
        {
            printk("%s: Failed to config gpio %d. code: %d.\n",
                __func__, MSM_8930_TOUCH_INT_PIN, ret);
            gpio_free(MSM_8930_TOUCH_INT_PIN);
            return ret;
        }
        is_gpio_requested = true;
    }
    
    ret = gpio_direction_input(MSM_8930_TOUCH_INT_PIN);
    if(ret != 0)
    {
        printk("%s: Failed to setup gpio %d. code: %d\n",
            __func__, MSM_8930_TOUCH_INT_PIN, ret);
        gpio_free(MSM_8930_TOUCH_INT_PIN);
        return ret;
    }

    return ret;
}

/*this function reset touch panel */
int touch_reset(void)
{
    int ret = 0;
    static bool is_gpio_requested = false;
    if(false == is_gpio_requested)
    {
        ret = gpio_request(MSM_8930_TOUCH_RESET_PIN, "TOUCH_RESET");
        if(ret != 0)
        {
            printk("%s: Failed to get gpio %d. code: %d.\n",
                __func__, MSM_8930_TOUCH_RESET_PIN, ret);
            return ret;
        }

        ret = gpio_tlmm_config(GPIO_CFG(MSM_8930_TOUCH_RESET_PIN, 
                            0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, 
                            GPIO_CFG_2MA), GPIO_CFG_ENABLE);
        if(ret != 0)
        {
            printk("%s: Failed to config gpio %d. code: %d.\n",
                __func__, MSM_8930_TOUCH_RESET_PIN, ret);
            gpio_free(MSM_8930_TOUCH_RESET_PIN);
            return ret;
        }
        is_gpio_requested = true;
    }


    ret = gpio_direction_output(MSM_8930_TOUCH_RESET_PIN, 1);
    if(ret != 0)
    {
        printk("%s: Failed to setup gpio %d. code: %d\n",
            __func__, MSM_8930_TOUCH_RESET_PIN, ret);
        gpio_free(MSM_8930_TOUCH_RESET_PIN);
        return ret;
    }
    mdelay(5);
    ret = gpio_direction_output(MSM_8930_TOUCH_RESET_PIN, 0);
    if(ret != 0)
    {
        printk("%s: Failed to setup gpio %d. code: %d\n",
            __func__, MSM_8930_TOUCH_RESET_PIN, ret);
        gpio_free(MSM_8930_TOUCH_RESET_PIN);
        return ret;
    }
    mdelay(10);
    ret = gpio_direction_output(MSM_8930_TOUCH_RESET_PIN, 1);
    if(ret != 0)
    {
        printk("%s: Failed to setup gpio %d. code: %d\n",
            __func__, MSM_8930_TOUCH_RESET_PIN, ret);
        gpio_free(MSM_8930_TOUCH_RESET_PIN);
        return ret;
    }
    msleep(50);//must more than 10ms.

    return ret;
}

/*this function return reset gpio at 8930 platform */
int get_touch_reset_gpio(void)
{
    return MSM_8930_TOUCH_RESET_PIN;
}

/*we use this to detect the probe is detected*/
void set_touch_probe_flag(int detected)
{
    if(detected >= 0)
    {
        atomic_set(&touch_detected_yet, 1);
    }
    else
    {
        atomic_set(&touch_detected_yet, 0);
    }

    return;
}

int read_touch_probe_flag(void)
{
    return atomic_read(&touch_detected_yet);
}

/*this function get the tp resolution*/
int get_touch_resolution(struct tp_resolution_conversion *tp_resolution_type)
{
    char tp_resolution[MAX_TP_RESOLUTION_LEN] = {0};

    if(NULL == tp_resolution_type)
    {
        printk("%s: para error!\n",__func__);
        return -1;
    }
    
    get_tp_resolution(tp_resolution,sizeof(tp_resolution));
    
    if(0 == strncmp(tp_resolution,WVGA,sizeof(tp_resolution)))
    {
        tp_resolution_type->lcd_x = LCD_X_WVGA;
        tp_resolution_type->lcd_y = LCD_Y_WVGA;
        tp_resolution_type->lcd_all = LCD_ALL_WVGA;
    }
    else if(0 == strncmp(tp_resolution,QHD,sizeof(tp_resolution)))
    {
        tp_resolution_type->lcd_x = LCD_X_QHD;
        tp_resolution_type->lcd_y = LCD_Y_QHD;
        tp_resolution_type->lcd_all = LCD_ALL_QHD_45INCHTP;
    }
    else if(0 == strncmp(tp_resolution,HD,sizeof(tp_resolution)))
    {
        tp_resolution_type->lcd_x = LCD_X_HD;
        tp_resolution_type->lcd_y = LCD_Y_HD;
        tp_resolution_type->lcd_all = LCD_ALL_HD_5INCHTP;
    }
    else
    {
        tp_resolution_type->lcd_x = LCD_X_WVGA;
        tp_resolution_type->lcd_y = LCD_Y_WVGA;
        tp_resolution_type->lcd_all = LCD_ALL_WVGA;
        printk("%s: default tp resolution!\n",__func__);
    }
    
    return 1;
}

/*If product has independent button ,return 1*/
int read_button_flag(void)
{
	char product_family[MAX_PRODUCT_LEN] = {0};
	get_product_family(product_family,sizeof(product_family));
	if(0 == strncmp(product_family,Y301,sizeof(product_family)))
	{
		return 1 ;
	}
	else
	{
		return 0 ;
	}
}

/*If product has independent button ,init the button map*/
int get_touch_button_map(struct tp_button_map *tp_button_map)
{	
	int button_num = 0;
	int button_map[MAX_BUTTON_NUM] = {0};
	int i = 0;
	char virtualkeys_map[MAX_VIRTUALKEYS_MAP_LEN] = {0};
	get_virtualkeys_map(virtualkeys_map,sizeof(virtualkeys_map));
	
	if(0 == strncmp(virtualkeys_map,MENU_HOME_BACK,sizeof(virtualkeys_map)))
	{
		button_map[0] = KEY_MENU;
		button_map[1] = KEY_HOME;
		button_map[2] = KEY_BACK;
		button_num = 3;
	}
	else if(0 == strncmp(virtualkeys_map,BACK_HOME_MENU,sizeof(virtualkeys_map)))
	{
		button_map[0] = KEY_BACK;
		button_map[1] = KEY_HOME;
		button_map[2] = KEY_MENU;
		button_num = 3;
	}
	else
	{
		return 0 ;
	}
	
	tp_button_map->button_num = button_num;
	for(i=0;i<button_num;i++)
	{
		tp_button_map->button_map[i] = button_map[i];
	}
	return 1;
}
/*delete some lines, the function is moved to power_switch() in hw-board-8930.c*/
static ssize_t virtualkeys_show(struct kobject *kobj,
                   struct kobj_attribute *attr, char *buf)
{
        memcpy( buf, buf_virtualkeys, buf_vkey_size );
        return buf_vkey_size;
}

static struct kobj_attribute synaptics_virtualkeys_attr = {
    .attr = {
        .name = "virtualkeys.synaptics",
        .mode = S_IRUGO,
    },
    .show = &virtualkeys_show,
};
static ssize_t virtualkey_show(struct kobject *kobj,
					struct kobj_attribute *attr, char *buf)
{
		memcpy( buf, buf_virtualkeys, buf_vkey_size );
		return buf_vkey_size; 
}

static struct kobj_attribute cyttsp4_virtualkeys_attr = {
	.attr = {
		.name = "virtualkeys.cyttsp4_mt",
		.mode = S_IRUGO,
	},
	.show = &virtualkey_show,
};
#ifdef CONFIG_HUAWEI_CYPRESS_TOUCHSCREEN
static struct kobj_attribute cypress_virtualkeys_attr = {
    .attr = {
        .name = "virtualkeys.cyttsp3-i2c",
        .mode = S_IRUGO,
    },
    .show = &virtualkeys_show,
};
#endif
static struct attribute *virtualkeys_properties_attrs[] = {
    &synaptics_virtualkeys_attr.attr,
    &cyttsp4_virtualkeys_attr.attr,
#ifdef CONFIG_HUAWEI_CYPRESS_TOUCHSCREEN
    &cypress_virtualkeys_attr.attr,
#endif
    NULL
};

static struct attribute_group virtualkeys_properties_attr_group = {
    .attrs = virtualkeys_properties_attrs,
};

void __init virtualkeys_init(void)
{
    struct kobject *properties_kobj = NULL;
    int ret = 0;
    char tp_resolution[MAX_TP_RESOLUTION_LEN] = {0};
    char virtualkeys_map[MAX_VIRTUALKEYS_MAP_LEN] = {0};

    get_tp_resolution(tp_resolution,sizeof(tp_resolution));
    get_virtualkeys_map(virtualkeys_map,sizeof(virtualkeys_map));
    
    if(0 == strncmp(tp_resolution,WVGA,sizeof(tp_resolution)))
    {
        if(0 == strncmp(virtualkeys_map,MENU_HOME_BACK,sizeof(virtualkeys_map)))
        {
            buf_vkey_size = sprintf(buf_virtualkeys,
                    __stringify(EV_KEY) ":" __stringify(KEY_MENU)  ":80:850:100:80"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":240:850:100:80"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_BACK) ":400:850:100:80"
                   "\n");
        }
        else if(0 == strncmp(virtualkeys_map,BACK_HOME_MENU,sizeof(virtualkeys_map)))
        {
            buf_vkey_size = sprintf(buf_virtualkeys,
                    __stringify(EV_KEY) ":" __stringify(KEY_BACK)  ":80:850:100:80"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":240:850:100:80"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":400:850:100:80"
                   "\n");
        }
        else
        {
            buf_vkey_size = sprintf(buf_virtualkeys,
                    __stringify(EV_KEY) ":" __stringify(KEY_BACK)  ":80:850:100:80"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":240:850:100:80"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":400:850:100:80"
                   "\n");
            printk("%s: default virtualkeys map for WVGA!\n",__func__);
        }
    }
    else if(0 == strncmp(tp_resolution,QHD,sizeof(tp_resolution)))
    {
        if(0 == strncmp(virtualkeys_map,MENU_HOME_BACK,sizeof(virtualkeys_map)))
        {
            buf_vkey_size = sprintf(buf_virtualkeys,
                    __stringify(EV_KEY) ":" __stringify(KEY_MENU)  ":80:1035:160:120"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":270:1035:160:120"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_BACK) ":460:1035:160:120"
                   "\n");
        }
        else if(0 == strncmp(virtualkeys_map,BACK_HOME_MENU,sizeof(virtualkeys_map)))
        {
            buf_vkey_size = sprintf(buf_virtualkeys,
                    __stringify(EV_KEY) ":" __stringify(KEY_BACK)  ":80:1035:160:120"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":270:1035:160:120"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":460:1035:160:120"
                   "\n");
        }
        else
        {
            buf_vkey_size = sprintf(buf_virtualkeys,
                    __stringify(EV_KEY) ":" __stringify(KEY_BACK)  ":80:1035:160:120"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":270:1035:160:120"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":460:1035:160:120"
                   "\n");
            printk("%s: default virtualkeys map for QHD!\n",__func__);
        }
    }
    /*change the button region of G740*/
    else if(0 == strncmp(tp_resolution,HD,sizeof(tp_resolution)))
    {
        if(0 == strncmp(virtualkeys_map,MENU_HOME_BACK,sizeof(virtualkeys_map)))
        {
            buf_vkey_size = sprintf(buf_virtualkeys,
                    __stringify(EV_KEY) ":" __stringify(KEY_MENU)  ":80:1330:100:80"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":360:1330:100:80"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_BACK) ":600:1330:100:80"
                   "\n");
        }
        else if(0 == strncmp(virtualkeys_map,BACK_HOME_MENU,sizeof(virtualkeys_map)))
        {
            buf_vkey_size = sprintf(buf_virtualkeys,
                    __stringify(EV_KEY) ":" __stringify(KEY_BACK)  ":138:1340:120:100"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":360:1340:140:100"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":582:1340:120:100"
                   "\n");
        }
        else
        {
            buf_vkey_size = sprintf(buf_virtualkeys,
                    __stringify(EV_KEY) ":" __stringify(KEY_BACK)  ":80:1330:100:80"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":360:1330:100:80"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":600:1330:100:80"
                   "\n");
            printk("%s: default virtualkeys map for WVGA!\n",__func__);
        }
    }
    else
    {
        buf_vkey_size = sprintf(buf_virtualkeys,
                    __stringify(EV_KEY) ":" __stringify(KEY_BACK)  ":57:850:100:80"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_HOME)   ":240:850:100:80"
                   ":" __stringify(EV_KEY) ":" __stringify(KEY_MENU) ":423:850:100:80"
                   "\n");
        printk("%s: default virtualkeys map!\n",__func__);
    }
    
    properties_kobj = kobject_create_and_add("board_properties", NULL);
    if(NULL == properties_kobj)
    {
        pr_err("failed to create board_properties\n");
        return;
    }
    ret = sysfs_create_group(properties_kobj,
                    &virtualkeys_properties_attr_group);
    if (ret != 0)
        pr_err("failed to create board_properties\n");
}
#endif

#ifdef CONFIG_HUAWEI_CYPRESS_TOUCHSCREEN
int cyttsp3_hw_reset(void)
{
	int retval = 0;

    int gpio_config = 0;
	gpio_config = GPIO_CFG(MSM_8930_TOUCH_RESET_PIN,0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP,GPIO_CFG_2MA);
	retval = gpio_tlmm_config(gpio_config, GPIO_CFG_ENABLE);
	if (retval)
	{
		pr_err("%s:touch int gpio config failed\n", __func__);
		return retval;
	}
	
	retval = gpio_request(MSM_8930_TOUCH_RESET_PIN, NULL);
	if (retval < 0) {
		pr_err("%s: Fail request RST pin r=%d\n", __func__, retval);
		pr_err("%s: Try free RST gpio=%d\n", __func__,
			MSM_8930_TOUCH_RESET_PIN);
		gpio_free(MSM_8930_TOUCH_RESET_PIN);
		retval = gpio_request(MSM_8930_TOUCH_RESET_PIN, NULL);
		if (retval < 0) {
			pr_err("%s: Fail 2nd request RST pin r=%d\n", __func__,
				retval);
		}
	}

	if (!(retval < 0)) {
		pr_info("%s: strobe RST(%d) pin\n", __func__,
			MSM_8930_TOUCH_RESET_PIN);
		gpio_set_value(MSM_8930_TOUCH_RESET_PIN, 1);
		msleep(10);//msleep(20);
		gpio_set_value(MSM_8930_TOUCH_RESET_PIN, 0);
		msleep(5);//msleep(40); 
		gpio_set_value(MSM_8930_TOUCH_RESET_PIN, 1);
		msleep(20);
		gpio_free(MSM_8930_TOUCH_RESET_PIN);
	}

	return retval;
}

int cyttsp3_hw_recov(int on)
{
	int retval = 0;

	switch (on) {
	case 0:
		cyttsp3_hw_reset();
		retval = 0;
		break;
	case CY_WAKE_DFLT:
		retval = gpio_request(MSM_8930_TOUCH_INT_PIN, NULL);
		if (retval < 0) {
			pr_err("%s: Fail request IRQ pin r=%d\n",
				__func__, retval);
			pr_err("%s: Try free IRQ gpio=%d\n", __func__,
				MSM_8930_TOUCH_INT_PIN);
			gpio_free(MSM_8930_TOUCH_INT_PIN);
			retval = gpio_request(MSM_8930_TOUCH_INT_PIN, NULL);
			if (retval < 0) {
				pr_err("%s: Fail 2nd request IRQ pin r=%d\n",
					__func__, retval);
			}
		}

		if (!(retval < 0)) {
			retval = gpio_direction_output
				(MSM_8930_TOUCH_INT_PIN, 0);
			if (retval < 0) {
				pr_err("%s: Fail switch IRQ pin to OUT r=%d\n",
					__func__, retval);
			} else {
				udelay(2000);
				retval = gpio_direction_input
					(MSM_8930_TOUCH_INT_PIN);
				if (retval < 0) {
					pr_err("%s: Fail switch IRQ pin to IN"
						" r=%d\n", __func__, retval);
				}
			}
			gpio_free(MSM_8930_TOUCH_INT_PIN);
		}
		break;
	default:
		retval = -ENOSYS;
		break;
	}

	return retval;
}

int cyttsp3_irq_stat(void)
{
	int irq_stat = 0;
	int retval = 0;

	retval = gpio_request(MSM_8930_TOUCH_INT_PIN, NULL);
	if (retval < 0) {
		pr_err("%s: Fail request IRQ pin r=%d\n", __func__, retval);
		pr_err("%s: Try free IRQ gpio=%d\n", __func__,
			MSM_8930_TOUCH_INT_PIN);
		gpio_free(MSM_8930_TOUCH_INT_PIN);
		retval = gpio_request(MSM_8930_TOUCH_INT_PIN, NULL);
		if (retval < 0) {
			pr_err("%s: Fail 2nd request IRQ pin r=%d\n",
				__func__, retval);
		}
	}

	if (!(retval < 0)) {
		irq_stat = gpio_get_value(MSM_8930_TOUCH_INT_PIN);
		gpio_free(MSM_8930_TOUCH_INT_PIN);
	}

	return irq_stat;
}
#endif


#ifdef CONFIG_HUAWEI_NFC_PN544
/* this function is used to reset pn544 by controlling the ven pin */
int pn544_ven_reset(void)
{	
    int ret1 = 0;
    int ret2 = 0;
    int ret3 = 0;
	
    ret1 = pm8xxx_gpio_config(PM8038_GPIO_PM_TO_SYS(2), &nfc_ven_enable);
    mdelay(5);
	
    ret2 = pm8xxx_gpio_config(PM8038_GPIO_PM_TO_SYS(2), &nfc_ven_disable);
    mdelay(10);
	
    ret3 = pm8xxx_gpio_config(PM8038_GPIO_PM_TO_SYS(2), &nfc_ven_enable);
    mdelay(5);
	
    if ((ret1 != 0) || (ret2 != 0) || (ret3 != 0))
    {
        printk("%s: Nfc ven gpio config fail, ret1 = %d, ret2 = %d, ret3 = %d.\n",__func__, ret1, ret2, ret3);
        return -1;
    }

    return 0;
}

int pn544_interrupt_gpio_config(void)
{
    int ret=0;
    int gpio_config=0;
    gpio_config = GPIO_CFG(GPIO_NFC_INT, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA);
    ret = gpio_tlmm_config(gpio_config, GPIO_CFG_ENABLE);
    ret = gpio_request(GPIO_NFC_INT, "gpio 114 for NFC pn544");
    ret = gpio_direction_input(GPIO_NFC_INT);
    return 0;
}

int pn544_fw_download_pull_down(void)
{
    gpio_set_value(GPIO_NFC_LOAD, 1);
    mdelay(5);
    gpio_set_value(GPIO_NFC_LOAD, 0);
    mdelay(5);
    return 0;	
}

int pn544_fw_download_pull_high(void)
{
    gpio_set_value(GPIO_NFC_LOAD, 0);
    mdelay(5);
    gpio_set_value(GPIO_NFC_LOAD, 1);
    mdelay(5);
    return 0;
}

int pn544_clock_output_ctrl(int vote)
{
    int rc = 0;
    const char * id = "nfcp";

    printk("%s in: debug!\n",__func__);
    if (nfc_clock == NULL) {
    	printk("%s in: vote is null!\n",__func__);
        nfc_clock = msm_xo_get(MSM_XO_TCXO_D1, id);
        if (IS_ERR(nfc_clock)) {
            rc = PTR_ERR(nfc_clock);

            pr_err("Failed to get TCXO_D1 voter (%ld)\n", PTR_ERR(nfc_clock));
            printk("%s out: get clock failed! rc=%d!\n",__func__, rc);

            goto fail_xo_get;
        }
    }

    if (vote == 0)
    {
        if (nfc_clock != NULL)
        {
            rc = msm_xo_mode_vote(nfc_clock, MSM_XO_MODE_OFF);
            printk("%s: clock off! rc=%d!\n",__func__, rc);
            if (rc < 0)
            {
                pr_err("Configuring TCXO to MSM_XO_MODE_OFF failed  (%d)\n", rc);
            }
//            msm_xo_put(nfc_clock);
//            nfc_clock = NULL;
        }
        printk("%s out: clock off! rc=%d!\n",__func__, rc);
        return rc;
    }


    rc = msm_xo_mode_vote(nfc_clock, MSM_XO_MODE_ON);
//    rc = msm_xo_mode_vote(nfc_clock, MSM_XO_MODE_PIN_CTRL);
    printk("SC:%s: clock on! rc=%d!\n",__func__, rc);
    if (rc < 0) {
        pr_err("Configuring TCXO to MSM_XO_MODE_ON failed  (%d)\n", rc);
        goto fail_xo_mode_vote;
    } 
    printk("SC:%s out: clock on! rc=%d!\n",__func__, rc);
    return rc;
 
    
fail_xo_mode_vote:
    msm_xo_put(nfc_clock);
fail_xo_get:
    
    printk("SC:%s out: with err! rc=%d!\n",__func__, rc);
    return rc;
}

// expand func function: add close PMU output function
// mode = 0 : close for clock pmu request mode,  mode = 1 : Set for clock pmu request mode
int pn544_clock_output_mode_ctrl(int vote)
{
    int rc = 0;
    const char * id = "nfcp";

    printk("SC:%s in: debug!\n",__func__);
    if (nfc_clock == NULL) {
    	printk("SC:%s in: vote is null!\n",__func__);
        nfc_clock = msm_xo_get(MSM_XO_TCXO_D1, id);
        if (IS_ERR(nfc_clock)) {
            rc = PTR_ERR(nfc_clock);

            pr_err("Failed to get TCXO_D1 voter (%ld)\n", PTR_ERR(nfc_clock));
            printk("SC:%s out: get clock failed! rc=%d!\n",__func__, rc);
            goto fail_xo_get;
        }
    }

    if (vote == 0)
    {
        if (nfc_clock != NULL)
        {
            msm_xo_mode_vote(nfc_clock, MSM_XO_MODE_OFF);
            printk("%s: clock off! rc=%d!\n",__func__, rc);
            if (rc < 0)
            {
                pr_err("Configuring TCXO to MSM_XO_MODE_OFF failed  (%d)\n", rc);
            }
//            msm_xo_put(nfc_clock);
//            nfc_clock = NULL;
        }
        printk("%s out: clock off! rc=%d!\n",__func__, rc);
        return rc;
    }
        
        

    /* Configure TCXO to be slave to CLK_REQ_NFC */
//    rc = msm_xo_mode_vote(nfc_clock, MSM_XO_MODE_PIN_CTRL);
    rc = msm_xo_mode_vote(nfc_clock, MSM_XO_MODE_ON);

    printk("%s: clock on! rc=%d!\n",__func__, rc);
    if (rc < 0) {
        pr_err("Configuring TCXO to Pin controllable failed (%d)\n", rc);
        goto fail_xo_mode_vote;
    } 
    printk("%s out: clock on! rc=%d!\n",__func__, rc);
    return rc;
 
    
fail_xo_mode_vote:
    msm_xo_put(nfc_clock);
fail_xo_get:
    printk("%s out: with err! rc=%d!\n",__func__, rc); 
    return rc;
}
#endif

/* -------------------- huawei sensors -------------------- */
#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_ACCELEROMETER_ADI_ADXL346
int gsensor_support_dummyaddr_adi346(void)
{
    int ret = -1;	/*default value means actual address*/

    ret = (int)GS_ADI346;

    return ret;
}
#endif

#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_ACCELEROMETER_KXTIK1004
int gsensor_support_dummyaddr_kxtik(void)
{
    int ret = -1;	/*default value means actual address*/
    ret = (int)GS_KXTIK1004;
    return ret;
}
#endif

#ifdef CONFIG_HUAWEI_FEATURE_PROXIMITY_EVERLIGHT_APS_9900
int aps9900_gpio_config_interrupt(void)
{
    int gpio_config = 0;
    int ret = 0;
  
    gpio_config = GPIO_CFG(MSM_8930_APS_INT_PIN, 0, GPIO_CFG_INPUT,GPIO_CFG_PULL_UP, GPIO_CFG_2MA);
    ret = gpio_tlmm_config(gpio_config, GPIO_CFG_ENABLE);
    return ret; 
}
#endif
