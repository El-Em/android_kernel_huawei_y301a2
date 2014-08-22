/* Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
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
#ifdef CONFIG_HUAWEI_KERNEL
#include <linux/delay.h>
#include <linux/rfkill.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/mfd/marimba.h>
#include <linux/io.h>
#include <asm/gpio.h>
#include <asm/mach-types.h>
#include <mach/rpc_pmapp.h>
#include <linux/gpio.h>
#include <hsad/config_interface.h>
#include "devices.h"
#include "board-8930.h"

#define GPIO_BT_UART_RTS    41 
#define GPIO_BT_UART_CTS    40
#define GPIO_BT_RX          39
#define GPIO_BT_TX          38

/*wake signals*/
#define GPIO_BT_WAKE_BT     89
#define GPIO_BT_WAKE_MSM    90

/*control signals*/
#define GPIO_BT_REG_ON  68
#define GPIO_BT_RESET_N     28  
/*pcm signals*/
#define GPIO_BT_PCM_OUT     63 
#define GPIO_BT_PCM_IN      64
#define GPIO_BT_PCM_SYNC    65
#define GPIO_BT_PCM_CLK     66

/*gpio function*/
#define GPIO_BT_FUN_0        0
#define GPIO_BT_FUN_1        1 
#define GPIO_BT_FUN_2        2 
#define GPIO_BT_ON           1
#define GPIO_BT_OFF          0

#define VREG_S3_VOLTAGE_VALUE	1800000



static struct resource bluesleep_resources[] = {
    {
    .name	= "gpio_host_wake",
    .start	= GPIO_BT_WAKE_MSM,
    .end	= GPIO_BT_WAKE_MSM,
    .flags	= IORESOURCE_IO,
    },
    {
    .name	= "gpio_ext_wake",
    .start	= GPIO_BT_WAKE_BT,
    .end	= GPIO_BT_WAKE_BT,
    .flags	= IORESOURCE_IO,
    },
    {
    .name	= "host_wake",
    .start	= MSM_GPIO_TO_INT(GPIO_BT_WAKE_MSM),
    .end	= MSM_GPIO_TO_INT(GPIO_BT_WAKE_MSM),
    .flags	= IORESOURCE_IRQ,
    },
};

/* bt devices define here */
struct platform_device msm_bt_power_device = {
    .name = "bt_power",
};

struct platform_device msm_bluesleep_device = {
    .name = "bcm_bluesleep",
    .id		= -1,
    .num_resources	= ARRAY_SIZE(bluesleep_resources),
    .resource	= bluesleep_resources,
};
//we need to add uart device here!! 
static struct platform_device *bt_common_devices[] __initdata = {
	&msm_device_uart_dm11,
	&msm_bt_power_device, 
	&msm_bluesleep_device,
};


enum {
    BT_WAKE,
    BT_RFR,
    BT_CTS,
    BT_RX,
    BT_TX,
    BT_PCM_DOUT,
    BT_PCM_DIN,
    BT_PCM_SYNC,
    BT_PCM_CLK,
    BT_HOST_WAKE,
};
/* config all msm bt gpio here!*/

static struct msm_gpio bt_config_bcm4330_power_on[] = {
    { GPIO_CFG(GPIO_BT_UART_RTS, GPIO_BT_FUN_1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL,   GPIO_CFG_2MA),
        "UART1DM_RFR" },
    { GPIO_CFG(GPIO_BT_UART_CTS, GPIO_BT_FUN_1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL,   GPIO_CFG_2MA),
        "UART1DM_CTS" },
    { GPIO_CFG(GPIO_BT_RX, GPIO_BT_FUN_1, GPIO_CFG_INPUT,  GPIO_CFG_NO_PULL,   GPIO_CFG_2MA),
        "UART1DM_Rx" },
    { GPIO_CFG(GPIO_BT_TX, GPIO_BT_FUN_1, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL,   GPIO_CFG_2MA),
        "UART1DM_Tx" },
    /*following 2 are the wakeup between 4330 and MSM*/
    { GPIO_CFG(GPIO_BT_WAKE_BT, GPIO_BT_FUN_0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL,  GPIO_CFG_2MA ),
        "MSM_WAKE_BT"  },
    { GPIO_CFG(GPIO_BT_WAKE_MSM, GPIO_BT_FUN_0, GPIO_CFG_INPUT , GPIO_CFG_NO_PULL ,  GPIO_CFG_2MA ),	
        "BT_WAKE_MSM"  },
    /*following 4 are the PCM between 4330 and MSM*/
    { GPIO_CFG(GPIO_BT_PCM_OUT, GPIO_BT_FUN_1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	
        "PCM_DOUT" },
    { GPIO_CFG(GPIO_BT_PCM_IN, GPIO_BT_FUN_1, GPIO_CFG_INPUT,  GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	
        "PCM_DIN" },
    { GPIO_CFG(GPIO_BT_PCM_SYNC, GPIO_BT_FUN_1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	
        "PCM_SYNC" },
    { GPIO_CFG(GPIO_BT_PCM_CLK, GPIO_BT_FUN_1, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),	
        "PCM_CLK " }
};

static struct msm_gpio bt_config_power_control[] = {  
    /*following 2 are bt on/off control*/
    { GPIO_CFG(GPIO_BT_REG_ON, GPIO_BT_FUN_0, GPIO_CFG_OUTPUT , GPIO_CFG_NO_PULL ,  GPIO_CFG_2MA ), 
        "BT_REG_ON"  },
    { GPIO_CFG(GPIO_BT_RESET_N, GPIO_BT_FUN_0, GPIO_CFG_OUTPUT , GPIO_CFG_NO_PULL ,  GPIO_CFG_2MA ), 
        "BT_PWR_ON"  }
};

static struct msm_gpio bt_config_bcm4330_power_off[] = {
    { GPIO_CFG(GPIO_BT_UART_RTS, GPIO_BT_FUN_0, GPIO_CFG_INPUT,  GPIO_CFG_PULL_DOWN,   GPIO_CFG_2MA),
        "UART1DM_RFR" },
    { GPIO_CFG(GPIO_BT_UART_CTS, GPIO_BT_FUN_0, GPIO_CFG_INPUT,  GPIO_CFG_PULL_DOWN,   GPIO_CFG_2MA),
        "UART1DM_CTS" },
    { GPIO_CFG(GPIO_BT_RX, GPIO_BT_FUN_0, GPIO_CFG_INPUT,  GPIO_CFG_PULL_DOWN,   GPIO_CFG_2MA),
        "UART1DM_Rx" },
    { GPIO_CFG(GPIO_BT_TX, GPIO_BT_FUN_0, GPIO_CFG_INPUT,  GPIO_CFG_PULL_DOWN,   GPIO_CFG_2MA),
        "UART1DM_Tx" },
    /*following 2 are the wakeup between 4330 and MSM*/
    { GPIO_CFG(GPIO_BT_WAKE_BT, GPIO_BT_FUN_0, GPIO_CFG_INPUT , GPIO_CFG_PULL_DOWN ,  GPIO_CFG_2MA ),
        "MSM_WAKE_BT"  },
    { GPIO_CFG(GPIO_BT_WAKE_MSM, GPIO_BT_FUN_0, GPIO_CFG_INPUT , GPIO_CFG_PULL_DOWN ,  GPIO_CFG_2MA ),	
        "BT_WAKE_MSM"  },
    /*following 4 are the PCM between 4330 and MSM*/
    { GPIO_CFG(GPIO_BT_PCM_OUT, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA), 
        "PCM_DOUT" },
    { GPIO_CFG(GPIO_BT_PCM_IN, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
        "PCM_DIN" },
    { GPIO_CFG(GPIO_BT_PCM_SYNC, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
        "PCM_SYNC" },
    { GPIO_CFG(GPIO_BT_PCM_CLK, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
     	"PCM_CLK" }
	};



/* configure all bt power here! */
static const char *vregs_bt_bcm4330_name[] = {
    "8038_l11"
};

void bt_wake_msm_config(void)
{
    //hw_bt_wakeup_gpio_type bt_wake_msm_gpio = GPIO_BT_WAKE_MSM;
    unsigned int bt_wake_msm_gpio = GPIO_BT_WAKE_MSM;
     
    bluesleep_resources[0].start = bt_wake_msm_gpio;
    bluesleep_resources[0].end = bt_wake_msm_gpio;
    bluesleep_resources[2].start = MSM_GPIO_TO_INT(bt_wake_msm_gpio);
    bluesleep_resources[2].end = MSM_GPIO_TO_INT(bt_wake_msm_gpio);
    bt_config_bcm4330_power_on[5].gpio_cfg = GPIO_CFG(bt_wake_msm_gpio, GPIO_BT_FUN_0, GPIO_CFG_INPUT , GPIO_CFG_NO_PULL ,  GPIO_CFG_2MA );
    bt_config_bcm4330_power_off[5].gpio_cfg = GPIO_CFG(bt_wake_msm_gpio, GPIO_BT_FUN_0, GPIO_CFG_INPUT , GPIO_CFG_PULL_DOWN ,  GPIO_CFG_2MA );
    
    
    printk(KERN_DEBUG "bt_wake_msm_gpio = %d\n", bt_wake_msm_gpio); 
}

static struct regulator *vregs_bt_bcm4330[ARRAY_SIZE(vregs_bt_bcm4330_name)];

/* put power on for bt*/
static int bluetooth_bcm4330_power_regulators(int on)
{
    int i = 0;
    int rc = 0;

    for (i = 0; i < ARRAY_SIZE(vregs_bt_bcm4330_name); i++) {
        rc = on ? regulator_enable(vregs_bt_bcm4330[i]) :
            regulator_disable(vregs_bt_bcm4330[i]);
        if (rc < 0) {
        printk(KERN_ERR "%s: vreg %s %s failed (%d)\n",
            __func__, vregs_bt_bcm4330_name[i],
    			       on ? "enable" : "disable", rc);
        return -EIO;
        }
    }

    /*gpio power for bcm4330*/
    if(on)
    {

        rc = gpio_direction_output(GPIO_BT_REG_ON, GPIO_BT_ON);  /*bt_reg_on: off---> on */
        if (rc) 
        {
            printk(KERN_ERR "%s:  bt power1 on fail (%d)\n",
                   __func__, rc);
            return -EIO;
        }
          
        mdelay(1);
        rc = gpio_direction_output(GPIO_BT_RESET_N, GPIO_BT_ON);  /*bt_reset_n: off---->on*/
        if (rc) 
        {
            printk(KERN_ERR "%s:  bt power2 off fail (%d)\n",
                   __func__, rc);
            return -EIO;
        }


    }
    else
    {
        rc = gpio_direction_output(GPIO_BT_RESET_N, GPIO_BT_OFF);  /*bt_reset_n:on-->off*/
        if (rc) 
        {
            printk(KERN_ERR "%s:  bt power2 off fail (%d)\n",
                   __func__, rc);
            return -EIO;
        }
        mdelay(1);

        rc = gpio_direction_output(GPIO_BT_REG_ON, GPIO_BT_OFF);  /*bt_reg_on :on -->off*/
        if (rc) 
        {
            printk(KERN_ERR "%s:  bt power1 off fail (%d)\n",
                   __func__, rc);
            return -EIO;
        }
        mdelay(1);

    }		
    return 0;
}

static int bluetooth_bcm4330_power(int on)
{
    int rc = 0;
    int pin = 0;

    printk(KERN_ERR "%s: bcm4330 power on/off called.\n",__func__);
    if (on)
    {
        /* msm: config the msm  bt gpios*/
        for(pin = 0; pin < ARRAY_SIZE(bt_config_bcm4330_power_on); pin++)
        {
            rc = gpio_tlmm_config(bt_config_bcm4330_power_on[pin].gpio_cfg,GPIO_CFG_ENABLE);
            if(rc)
            {
                printk(KERN_ERR "%s: bcm4330_config_gpio on failed (%d)\n",
                __func__, rc);
                return rc;
            }
        }
        
        rc = bluetooth_bcm4330_power_regulators(on);
        if (rc < 0) 
        {
            printk(KERN_ERR "%s: bcm4330_power_regulators on failed (%d)\n",
                __func__, rc);
            return rc;
        }
    }
    else
    {
        /* msm: config the msm  bt gpios*/
        for(pin = 0; pin < ARRAY_SIZE(bt_config_bcm4330_power_off); pin++)
        {
            rc = gpio_tlmm_config(bt_config_bcm4330_power_off[pin].gpio_cfg,GPIO_CFG_ENABLE);
            if(rc)
            {
                printk(KERN_ERR "%s: bcm4330_config_gpio on failed (%d)\n",
                __func__, rc);
                return rc;
            }
        }
        
        /* check for initial rfkill block (power off) */
        if (platform_get_drvdata(&msm_bt_power_device) == NULL)
        {
            printk(KERN_DEBUG "bluetooth rfkill block error : \n");
            goto out;
        }
      
        rc = bluetooth_bcm4330_power_regulators(on);
        if (rc < 0) 
        {
            printk(KERN_ERR "%s: bcm4330_power_regulators off failed (%d)\n",
                __func__, rc);
            return rc;
        }
       


    }	
out:
    printk(KERN_DEBUG "Bluetooth BCM4330 power switch: %d\n", on);

    return 0;
}

	
void __init bt_bcm4330_power_init(void)
{
    int i = 0;
    int rc = -1;
    int pin = 0;
    struct regulator *reg_8038_l13 = NULL;
    struct regulator *reg_8038_l25 = NULL;
    printk(KERN_ERR "bt_bcm4330_power_init pre\n");
    
    /* msm: config the msm  bt gpios*/
    for(pin = 0; pin < ARRAY_SIZE(bt_config_bcm4330_power_on); pin++)
    {
        rc = gpio_tlmm_config(bt_config_bcm4330_power_on[pin].gpio_cfg,GPIO_CFG_ENABLE);
        if(rc)
        {
			/*change the log comment*/
            printk(KERN_ERR "%s: bcm bt device config gpio failed (%d)\n",
            __func__, rc);
             return ;
         }
    }	  
    
	/*change the log comment*/
	printk(KERN_ERR "%s: bcm bt device_config_gpio on succed! (%d)\n",
            __func__, rc);
    /*enable LDO13 & LDO25 to reduce current consume of bcm4330 sleep clock*/
    reg_8038_l13 = regulator_get(NULL, "8038_l13");
    if (IS_ERR(reg_8038_l13))
            printk("Unable to get the 8038_l13 regulator!\n");
    
    rc = regulator_enable(reg_8038_l13);
    if (rc < 0) {
        printk(KERN_ERR "%s: 8038_l13 enable failed (%d)\n",
        __func__, rc);
        }

    reg_8038_l25 = regulator_get(NULL, "8038_l25");
    if (IS_ERR(reg_8038_l25))
            printk("Unable to get the 8038_l25 regulator!\n");
    
    rc = regulator_enable(reg_8038_l25);
    if (rc < 0) {
        printk(KERN_ERR "%s: 8038_l25 enable failed (%d)\n",
        __func__, rc);
        }
	/* 1. here will check the power and set voltage to 1.8V */
    for (i = 0; i < ARRAY_SIZE(vregs_bt_bcm4330_name); i++)
    {
        vregs_bt_bcm4330[i] = regulator_get(NULL, vregs_bt_bcm4330_name[i]);
        if (IS_ERR(vregs_bt_bcm4330[i]))
        {
            printk(KERN_ERR "%s: vreg get %s failed (%ld)\n",
                __func__, vregs_bt_bcm4330_name[i],
                PTR_ERR(vregs_bt_bcm4330_name[i]));
        	return;
        }
        rc = regulator_set_voltage(vregs_bt_bcm4330[i], VREG_S3_VOLTAGE_VALUE, VREG_S3_VOLTAGE_VALUE);
        if (rc)
        {
		     printk("%s: regulator_s3 regulator_set_voltage failed\n", __func__);
       return;
	    }
    }
    printk(KERN_ERR "bt bcm4330 vreg is ok.\n");
  
  
    /* 2. handle bt power control: becareful */
    /* BT_REG_ON */
    rc = gpio_request(GPIO_BT_REG_ON,bt_config_power_control[0].label);
    if(rc)
    {
        printk(KERN_ERR
                "%s: GPIO_BT_REG_ON request failed (%d).\n",
                        __func__, rc);
        return;
    }        
    rc = gpio_tlmm_config(bt_config_power_control[0].gpio_cfg,GPIO_CFG_ENABLE);
    if(rc)
    {
        printk(KERN_ERR
                "%s: GPIO_BT_REG_ON config failed (%d).\n",
                        __func__, rc);
        return;
    }

   /* BT_RESET_N */
    rc = gpio_request(GPIO_BT_RESET_N,bt_config_power_control[1].label);
    if(rc)
    {
        printk(KERN_ERR
                "%s: BT_RESET_N request failed (%d).\n",
                        __func__, rc);
        return;
    }        
    rc = gpio_tlmm_config(bt_config_power_control[1].gpio_cfg,GPIO_CFG_ENABLE);
    if(rc)
    {
        printk(KERN_ERR
                "%s: BT_RESET_N config failed (%d).\n",
                        __func__, rc);
        return;
    }
    
    printk("bt power control gpio request and config succed.\n");
    /*rc = msm_gpios_request_enable(bt_config_power_control,
                            ARRAY_SIZE(bt_config_power_control));
    if (rc < 0) {
            printk(KERN_ERR
                    "%s: bt power control request_enable failed (%d)\n",
                            __func__, rc);
            return;
    }*/
    
    /* BT_RST_N: pull down */
    rc = gpio_direction_output(GPIO_BT_RESET_N, GPIO_BT_OFF); 
    if (rc) 
    {
        printk(KERN_ERR "%s:  bt power off fail (%d)\n",
               __func__, rc);
        return ;
    }
    printk(KERN_ERR "bt bcm4330 BT_RST_N pulled down.\n");
    
    mdelay(1);

    /* BT_REG_ON: pull down */
    rc = gpio_direction_output(GPIO_BT_REG_ON, GPIO_BT_OFF);  /*bt_reg_on (68) :*/
    if (rc) 
    {
        printk(KERN_ERR "%s:  bt power1 off fail (%d)\n",
               __func__, rc);
        return ;
    }
    mdelay(1);

    printk(KERN_ERR "bt bcm4330 BT_REG_ON pulled down.\n");
    
    /* 3. add bt device to paltform */
    platform_add_devices(bt_common_devices, ARRAY_SIZE(bt_common_devices));
    
    /* 4. bt wakeup config */
    bt_wake_msm_config();    
    
    /* 5. config platform_data*/
    msm_bt_power_device.dev.platform_data = &bluetooth_bcm4330_power;
    
    printk(KERN_ERR "bt bcm4330 init succesfully.\n");
	
}

#endif //CONFIG_HUAWEI_KERNEL
