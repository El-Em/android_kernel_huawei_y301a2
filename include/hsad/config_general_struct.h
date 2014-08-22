#ifndef _CONFIG_GENERIC_STRUCT_H
#define _CONFIG_GENERIC_STRUCT_H

#include <hsad/configdata.h>
#include <mach/gpiomux.h>
#include <linux/hwgpio.h> 
#include <linux/regulator/hw-uniregulator.h>
//module name list
#ifdef CONFIG_HUAWEI_GPIO_UNITE
#define GPIO_MODULE_NAME  "gpio"
#define PM_GPIO_MODULE_NAME  "pm gpio"
#endif
#define COMMON_MODULE_NAME  "common"
#ifdef CONFIG_HW_POWER_TREE
#define POWER_MODULE_NAME "power"
#endif
struct board_id_general_struct
{
	char	name[32];
	int		board_id;
	union{	
#ifdef CONFIG_HUAWEI_GPIO_UNITE
		struct gpio_config_type *gpio_ptr;
		struct pm_gpio_cfg_t *pm_gpio_ptr;
#endif
#ifdef CONFIG_HW_POWER_TREE
		struct hw_config_power_tree* power_tree_ptr; 
#endif
		config_pair	*config_pair_ptr;
	}data_array;
	struct list_head list;
};
#endif
