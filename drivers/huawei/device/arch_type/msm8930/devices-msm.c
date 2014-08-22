#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>

#include<linux/init.h>
#include<linux/module.h>

#include <hsad/config_interface.h>

#define HW_DEVICE_DEBUG
#ifdef HW_DEVICE_DEBUG
#define HW_DEV_DBG(fmt, args...)\
printk(KERN_DEBUG "[HW_DEVICE]: " fmt,## args);
#else
#define HW_DEV_DBG(fmt, args...)
#endif


#define GET_GPIO_FAIL  -1
#define LCD_ID0		93
#define LCD_ID1		74
#define GET_LCD_ID_FAIL  -1 
#define LCD_ID_PULL_UP  1
#define LCD_ID_PULL_DOWN  0
/**
 * is_gpio_connect - check gpio is connect
 *
 * return 1 if connect,0 if not connect
 */
/* 8930 device does not need this function
static bool is_gpio_connect(int gpio)
{
    int gpio_state0,gpio_state1;

    gpio_direction_output(gpio,0);
    gpio_direction_input(gpio);
    gpio_state0 = gpio_get_value(gpio);
    gpio_direction_output(gpio,1);
    gpio_direction_input(gpio);
    gpio_state1 = gpio_get_value(gpio);

    return (gpio_state0 == gpio_state1);
}*/

/****************************************************************
function: get lcd id by gpio

*data structure*
*	   ID1	  ID0   * 
 *	----------------- * 
 *	|   |   |   |   | * 
 *	|   |   |   |   | * 
 *	----------------- *
 For each Gpio :
		00 means low  ,
		01 means high ,
		10 means float,
		11 is not defined,

 lcd id(hex):
 0	:ID0 low,	ID1 low
 1	:ID0 high,	ID1 low
 2	:ID0 float,	ID1 low

 4	:ID0 low,	ID1 high
 5	:ID0 high,	ID1 high
 6	:ID0 float,	ID1 high

 8	:ID0 low,	ID1 float
 9	:ID0 high,	ID1 float
 A	:ID0 float,	ID1 float, used for emulator
 ***************************************************************/
int get_lcd_id(void)
{
	int ret = 0;
	int id0,id1;
	int gpio_id0,gpio_id1;
	int pullup_read,pulldown_read;
	static int lcd_id = GET_LCD_ID_FAIL;
	id0=0;
	id1=0;
	pullup_read = 0;
	pulldown_read = 0;
	gpio_id0 = LCD_ID0;
	gpio_id1 = LCD_ID1;
	
	if( (lcd_id >= 0x0) && (lcd_id <= 0xA) )//if lcd_id had read successfully,just return lcd_id.
		return lcd_id;
	if(gpio_id0 <= GET_GPIO_FAIL ||gpio_id1 <= GET_GPIO_FAIL)
		return GET_LCD_ID_FAIL;
    HW_DEV_DBG("gpio_lcd_id0:%d gpio_lcd_id1:%d\n",gpio_id0,gpio_id1);
    ret = gpio_request(gpio_id0, "lcd_id0");
      if (ret) {
         printk(KERN_ERR"lcd_id0 gpio[%d] request failed\n", gpio_id0);
         goto lcd_id0_req_fail;
          }
        ret = gpio_request(gpio_id1, "lcd_id1");
	if (ret) {
	     printk(KERN_ERR"lcd_id1 gpio[%d] request failed\n", gpio_id1);
         goto lcd_id1_req_fail;
	}
	/*config id0 to pull down and read*/
	ret = gpio_tlmm_config(GPIO_CFG(gpio_id0,0,GPIO_CFG_INPUT,GPIO_CFG_PULL_DOWN,GPIO_CFG_2MA),GPIO_CFG_ENABLE);
	  if (ret) {
	     printk(KERN_ERR"config id0 to pull down failed\n");
	     goto get_lcd_id_fail;	
	    }
	udelay(10);//necessary for a delay, else following hw_gpio_input always get 0
	pulldown_read = gpio_get_value(gpio_id0);
	/*config id0 to pull up and read*/
	ret = gpio_tlmm_config(GPIO_CFG(gpio_id0,0,GPIO_CFG_INPUT,GPIO_CFG_PULL_UP,GPIO_CFG_2MA),GPIO_CFG_ENABLE);
	  if (ret) {
	       printk(KERN_ERR"config id0 to pull up failed\n");
	       goto get_lcd_id_fail;	
	     }
	udelay(10);
	pullup_read = gpio_get_value(gpio_id0);
	if(pulldown_read != pullup_read)//float
	{
		id0 = BIT(1);
		gpio_tlmm_config(GPIO_CFG(gpio_id0,0,GPIO_CFG_INPUT,GPIO_CFG_PULL_DOWN,GPIO_CFG_2MA),GPIO_CFG_ENABLE);
	}
	else//connect 
	{
		id0 = pullup_read;//pullup_read==pulldown_read
		switch(id0)
		{
			case LCD_ID_PULL_DOWN:
			//	gpio_tlmm_config(GPIO_CFG(gpio_id0,0,GPIO_CFG_INPUT,GPIO_CFG_PULL_DOWN,GPIO_CFG_2MA),GPIO_CFG_ENABLE);
			//	break;
			case LCD_ID_PULL_UP:
			//	gpio_tlmm_config(GPIO_CFG(gpio_id0,0,GPIO_CFG_INPUT,GPIO_CFG_PULL_UP,GPIO_CFG_2MA),GPIO_CFG_ENABLE);
			//	break;
			default:
				gpio_tlmm_config(GPIO_CFG(gpio_id0,0,GPIO_CFG_INPUT,GPIO_CFG_NO_PULL,GPIO_CFG_2MA),GPIO_CFG_ENABLE);
				break;
		}
			
	}
	/*config id1 to pull down and read*/
	ret = gpio_tlmm_config(GPIO_CFG(gpio_id1,0,GPIO_CFG_INPUT,GPIO_CFG_PULL_DOWN,GPIO_CFG_2MA),GPIO_CFG_ENABLE);
	  if (ret) {
	     printk(KERN_ERR"config id1 to pull down failed\n");
	     goto get_lcd_id_fail;	
	    }
	udelay(10);
	pulldown_read = gpio_get_value(gpio_id1);
	/*config id1 to pull up and read*/
	ret = gpio_tlmm_config(GPIO_CFG(gpio_id1,0,GPIO_CFG_INPUT,GPIO_CFG_PULL_UP,GPIO_CFG_2MA),GPIO_CFG_ENABLE);
	if (ret) {
	     printk(KERN_ERR"config id1 to pull up failed\n");
	     goto get_lcd_id_fail;	
	    }
	udelay(10);
	pullup_read = gpio_get_value(gpio_id1);
	if(pulldown_read != pullup_read)//float
	{
		id1 = BIT(1);
		gpio_tlmm_config(GPIO_CFG(gpio_id1,0,GPIO_CFG_INPUT,GPIO_CFG_PULL_DOWN,GPIO_CFG_2MA),GPIO_CFG_ENABLE);
	}
	else//connect
	{
		id1 = pullup_read;//pullup_read==pulldown_read
		switch(id1)
		{
			case LCD_ID_PULL_DOWN:
			//	gpio_tlmm_config(GPIO_CFG(gpio_id1,0,GPIO_CFG_INPUT,GPIO_CFG_PULL_DOWN,GPIO_CFG_2MA),GPIO_CFG_ENABLE);
			//	break;
			case LCD_ID_PULL_UP:
			//	gpio_tlmm_config(GPIO_CFG(gpio_id1,0,GPIO_CFG_INPUT,GPIO_CFG_PULL_UP,GPIO_CFG_2MA),GPIO_CFG_ENABLE);
			//	break;
			default:
				gpio_tlmm_config(GPIO_CFG(gpio_id1,0,GPIO_CFG_INPUT,GPIO_CFG_NO_PULL,GPIO_CFG_2MA),GPIO_CFG_ENABLE);
				break;
		}
	}
	gpio_free(gpio_id0);
    gpio_free(gpio_id1);

	lcd_id = (id1<<2) | id0;
	return lcd_id;
get_lcd_id_fail:
	gpio_free(gpio_id1);
lcd_id1_req_fail:
    gpio_free(gpio_id0);
lcd_id0_req_fail:
    return GET_LCD_ID_FAIL;
}
EXPORT_SYMBOL(get_lcd_id);
/**
 * gpio_config_interrupt - config gpio irq
 * gpio:gpio num of the irq
 * name:irq name
 * enable:get gpio or free gpio
 * return irq num if success,error num if error occur
 */
int gpio_config_interrupt(int gpio,char * name,int enable)
{
    int err = 0;

    if(gpio<0||gpio>151)
    {
        printk(KERN_ERR "%s:gpio[%d] is invalid\n",__func__,gpio);
        return -EINVAL;
    }
    if(enable)
    {
        err = gpio_request(gpio, name);
        if (err) {
            printk(KERN_ERR"%s: gpio_request failed for intr %d\n", name,gpio);
            return err;
        }
        err = gpio_direction_input(gpio);
        if (err) {
            printk(KERN_ERR"%s: gpio_direction_input failed for intr %d\n",name, gpio);
            gpio_free(gpio);
            return err;
        }
        return MSM_GPIO_TO_INT(gpio);
    }
    else
    {
        gpio_free(gpio);
        return 0;
    }
}



