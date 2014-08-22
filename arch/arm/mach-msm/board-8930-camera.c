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

#include <asm/mach-types.h>
#include <linux/gpio.h>
#include <mach/camera.h>
#include <mach/msm_bus_board.h>
#include <mach/gpiomux.h>
#include "devices.h"
#include "board-8930.h"

#ifdef CONFIG_MSM_CAMERA

#if (defined(CONFIG_GPIO_SX150X) || defined(CONFIG_GPIO_SX150X_MODULE)) && \
	defined(CONFIG_I2C)

static struct i2c_board_info cam_expander_i2c_info[] = {
	{
		I2C_BOARD_INFO("sx1508q", 0x22),
		.platform_data = &msm8930_sx150x_data[SX150X_CAM]
	},
};

static struct msm_cam_expander_info cam_expander_info[] = {
	{
		cam_expander_i2c_info,
		MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	},
};
#endif

static struct gpiomux_setting cam_settings[] = {
	{
		.func = GPIOMUX_FUNC_GPIO, /*suspend*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_DOWN,
	},

	{
		.func = GPIOMUX_FUNC_1, /*active 1*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*active 2*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_1, /*active 3*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_5, /*active 4*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_6, /*active 5*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_2, /*active 6*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_3, /*active 7*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*i2c suspend*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_KEEPER,
	},
	{
		.func = GPIOMUX_FUNC_2, /*active 9*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

};


static struct msm_gpiomux_config msm8930_cam_common_configs[] = {
	{
		.gpio = 2,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 3,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[1],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},

#ifndef CONFIG_HUAWEI_KERNEL 	
	{
		.gpio = 54,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	
	{
		.gpio = 20,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
	{
		.gpio = 21,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
	
#endif	
   {
		.gpio = 4,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[9],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 76,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 5,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[1],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	
	{
		.gpio = 107,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	
};

static struct msm_gpiomux_config msm8930_cam_2d_configs[] = {
	#ifndef CONFIG_HUAWEI_KERNEL 	
	{
		.gpio = 18,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
	{
		.gpio = 19,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
	#endif
	{
		.gpio = 20,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 21,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
};

#define VFE_CAMIF_TIMER1_GPIO 2
#define VFE_CAMIF_TIMER2_GPIO 3
#define VFE_CAMIF_TIMER3_GPIO_INT 4
static struct msm_camera_sensor_strobe_flash_data strobe_flash_xenon = {
	.flash_trigger = VFE_CAMIF_TIMER2_GPIO,
	.flash_charge = VFE_CAMIF_TIMER1_GPIO,
	.flash_charge_done = VFE_CAMIF_TIMER3_GPIO_INT,
	.flash_recharge_duration = 50000,
	.irq = MSM_GPIO_TO_INT(VFE_CAMIF_TIMER3_GPIO_INT),
};

#ifdef CONFIG_MSM_CAMERA_FLASH
static struct msm_camera_sensor_flash_src msm_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_EXT,
	._fsrc.ext_driver_src.led_en = VFE_CAMIF_TIMER1_GPIO,
	._fsrc.ext_driver_src.led_flash_en = VFE_CAMIF_TIMER2_GPIO,
	._fsrc.ext_driver_src.flash_id = MAM_CAMERA_EXT_LED_FLASH_TPS61310,
};
#endif

static struct msm_bus_vectors cam_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_preview_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 27648000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_video_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 600000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_snapshot_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 600000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
};

static struct msm_bus_vectors cam_zsl_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 600000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
};

static struct msm_bus_vectors cam_video_ls_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 600000000,
		.ib  = 4264000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
};

static struct msm_bus_vectors cam_dual_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 302071680,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
};

static struct msm_bus_vectors cam_adv_video_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 274406400,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};


static struct msm_bus_paths cam_bus_client_config[] = {
	{
		ARRAY_SIZE(cam_init_vectors),
		cam_init_vectors,
	},
	{
		ARRAY_SIZE(cam_preview_vectors),
		cam_preview_vectors,
	},
	{
		ARRAY_SIZE(cam_video_vectors),
		cam_video_vectors,
	},
	{
		ARRAY_SIZE(cam_snapshot_vectors),
		cam_snapshot_vectors,
	},
	{
		ARRAY_SIZE(cam_zsl_vectors),
		cam_zsl_vectors,
	},
	{
		ARRAY_SIZE(cam_video_ls_vectors),
		cam_video_ls_vectors,
	},
	{
		ARRAY_SIZE(cam_dual_vectors),
		cam_dual_vectors,
	},
	{
		ARRAY_SIZE(cam_adv_video_vectors),
		cam_adv_video_vectors,
	},

};

static struct msm_bus_scale_pdata cam_bus_client_pdata = {
		cam_bus_client_config,
		ARRAY_SIZE(cam_bus_client_config),
		.name = "msm_camera",
};

static struct msm_camera_device_platform_data msm_camera_csi_device_data[] = {
	{
		.csid_core = 0,
		.is_vpe    = 1,
		.cam_bus_scale_table = &cam_bus_client_pdata,
	},
	{
		.csid_core = 1,
		.is_vpe    = 1,
		.cam_bus_scale_table = &cam_bus_client_pdata,
	},
};

static struct camera_vreg_t msm_8930_cam_vreg[] = {
	{"cam_iovdd", REG_VS, 0, 0, 0},
	{"cam_avdd", REG_LDO, 2800000, 2850000, 85600},
};

static struct camera_vreg_t msm_imx188_cam_vreg[] = {
	{"cam_avdd", REG_LDO, 2800000, 2850000, 85600,0},
	{"cam_iovdd", REG_VS, 0, 0, 0,1000},
	{"cam_dvdd", REG_LDO, 1200000, 1250000,0,10},
	
};
static struct camera_vreg_t msm_ov9724_cam_vreg[] = {
	{"cam_avdd", REG_LDO, 2850000, 2850000, 85600,0},
	{"cam_iovdd", REG_VS, 0, 0, 0,0},
	{"cam_dvdd", REG_LDO, 1200000, 1200000,0,0},
};
static struct gpio msm8930_common_cam_gpio[] = {
	{20, GPIOF_DIR_IN, "CAMIF_I2C_DATA"},
	{21, GPIOF_DIR_IN, "CAMIF_I2C_CLK"},
};

static struct gpio msm8930_front_cam_gpio[] = {
	{4, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{76, GPIOF_DIR_OUT, "CAM_RESET"},
};

static struct gpio msm8930_back_cam_gpio[] = {
	{5, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{107, GPIOF_DIR_OUT, "CAM_RESET"},
	{54, GPIOF_DIR_OUT, "CAM_STBY_N"},
};

static struct msm_gpio_set_tbl msm8930_front_cam_gpio_set_tbl[] = {
	{76, GPIOF_OUT_INIT_LOW, 1000},
	{76, GPIOF_OUT_INIT_HIGH, 4000},
};

static struct msm_gpio_set_tbl msm8930_back_cam_gpio_set_tbl[] = {
	{107, GPIOF_OUT_INIT_LOW, 1000},
	{107, GPIOF_OUT_INIT_HIGH, 4000},
};

static struct msm_gpio_set_tbl mt9e013_cam_gpio_req_init_tbl[] = {
	{107, GPIOF_OUT_INIT_HIGH, 1000},  //set the flag of gpio, then usleep the times
};

static struct msm_camera_gpio_conf msm_8930_front_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_front_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_front_cam_gpio),
	.cam_gpio_set_tbl = msm8930_front_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_front_cam_gpio_set_tbl),
};

static struct msm_camera_gpio_conf msm_8930_back_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_back_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio),
	.cam_gpio_set_tbl = msm8930_back_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio_set_tbl),
	.cam_gpio_req_init_tbl = mt9e013_cam_gpio_req_init_tbl,
	.cam_gpio_req_init_tbl_size = ARRAY_SIZE(mt9e013_cam_gpio_req_init_tbl),
};

static struct i2c_board_info msm_act_main_cam_i2c_info = {
	I2C_BOARD_INFO("msm_actuator", 0x11),
};

static struct msm_actuator_info msm_act_main_cam_0_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_0,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = 0,
	.vcm_enable     = 0,
};

static struct msm_camera_sensor_flash_data flash_imx074 = {
	.flash_type	= MSM_CAMERA_FLASH_NONE,//MSM_CAMERA_FLASH_LED,
#ifdef CONFIG_MSM_CAMERA_FLASH
	.flash_src	= &msm_flash_src
#endif
};

static struct msm_camera_csi_lane_params imx074_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};

static struct msm_camera_sensor_platform_info sensor_board_info_imx074 = {
	.mount_angle	= 90,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_back_cam_gpio_conf,
	.csi_lane_params = &imx074_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_imx074_data = {
	.sensor_name	= "imx074",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_imx074,
	.strobe_flash_data = &strobe_flash_xenon,
	.sensor_platform_info = &sensor_board_info_imx074,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.actuator_info = &msm_act_main_cam_0_info,
};

static struct msm_camera_sensor_flash_data flash_mt9m114 = {
	.flash_type = MSM_CAMERA_FLASH_NONE
};

static struct msm_camera_csi_lane_params mt9m114_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x1,
};

static struct msm_camera_sensor_platform_info sensor_board_info_mt9m114 = {
	.mount_angle = 90,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_front_cam_gpio_conf,
	.csi_lane_params = &mt9m114_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9m114_data = {
	.sensor_name = "mt9m114",
	.pdata = &msm_camera_csi_device_data[1],
	.flash_data = &flash_mt9m114,
	.sensor_platform_info = &sensor_board_info_mt9m114,
	.csi_if = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = YUV_SENSOR,
};

static struct msm_camera_sensor_flash_data flash_ov2720 = {
	.flash_type	= MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_csi_lane_params ov2720_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x3,
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov2720 = {
	.mount_angle	= 0,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_front_cam_gpio_conf,
	.csi_lane_params = &ov2720_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov2720_data = {
	.sensor_name	= "ov2720",
	.pdata	= &msm_camera_csi_device_data[1],
	.flash_data	= &flash_ov2720,
	.sensor_platform_info = &sensor_board_info_ov2720,
	.csi_if	= 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
};

static struct msm_camera_sensor_flash_data flash_s5k3l1yx = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src = &msm_flash_src
};

static struct msm_camera_csi_lane_params s5k3l1yx_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};

static struct msm_camera_sensor_platform_info sensor_board_info_s5k3l1yx = {
	.mount_angle  = 90,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_back_cam_gpio_conf,
	.csi_lane_params = &s5k3l1yx_csi_lane_params,
};

static struct msm_actuator_info msm_act_main_cam_2_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_2,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = 0,
	.vcm_enable     = 0,
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k3l1yx_data = {
	.sensor_name          = "s5k3l1yx",
	.pdata                = &msm_camera_csi_device_data[0],
	.flash_data           = &flash_s5k3l1yx,
	.sensor_platform_info = &sensor_board_info_s5k3l1yx,
	.csi_if               = 1,
	.camera_type          = BACK_CAMERA_2D,
	.sensor_type          = BAYER_SENSOR,
	.actuator_info    = &msm_act_main_cam_2_info,
};
static struct msm_actuator_info msm_act_main_cam_9_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_9,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = 13,
	.vcm_enable     = 1,
};

static struct msm_camera_csi_lane_params mt9e013_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};

static struct msm_camera_sensor_flash_data flash_mt9e013 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_mt9e013 = {
	.mount_angle	= 0, 
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_back_cam_gpio_conf,
	.csi_lane_params = &mt9e013_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9e013_data = {
	.sensor_name	= "mt9e013",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_mt9e013,
	.sensor_platform_info = &sensor_board_info_mt9e013,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type          = BAYER_SENSOR, 
	.actuator_info    = &msm_act_main_cam_9_info,
};

#ifdef CONFIG_GC0313
static struct msm_camera_csi_lane_params gc0313_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x1,   //maybe, this can be define front csi_lane_params
};

static struct msm_camera_sensor_flash_data flash_gc0313 = {
	.flash_type = MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_platform_info sensor_board_info_gc0313 = {
	.mount_angle	= 270, 
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_front_cam_gpio_conf,
	.csi_lane_params = &gc0313_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_gc0313_data = {
	.sensor_name	= "gc0313",
	.pdata	= &msm_camera_csi_device_data[1],
	.flash_data	= &flash_gc0313,
	.sensor_platform_info = &sensor_board_info_gc0313,
	.csi_if	= 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type          = YUV_SENSOR, 
	.actuator_info    = NULL,
};
#endif

#ifdef CONFIG_BF3905
static struct msm_camera_csi_lane_params bf3905_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x1,   //maybe, this can be define front csi_lane_params
};

static struct msm_camera_sensor_flash_data flash_bf3905 = {
	.flash_type = MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_platform_info sensor_board_info_bf3905 = {
	.mount_angle	= 270, 
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_front_cam_gpio_conf,
	.csi_lane_params = &bf3905_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_bf3905_data = {
	.sensor_name	= "bf3905",
	.pdata	= &msm_camera_csi_device_data[1],
	.flash_data	= &flash_bf3905,
	.sensor_platform_info = &sensor_board_info_bf3905,
	.csi_if	= 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type          = YUV_SENSOR, 
	.actuator_info    = NULL,
};
#endif

static struct msm_camera_csi_lane_params mt9v113_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x1,
};


static struct msm_camera_sensor_flash_data flash_mt9v113 = {
	.flash_type = MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_platform_info sensor_board_info_mt9v113 = {
	.mount_angle	= 270, 
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_front_cam_gpio_conf,
	.csi_lane_params = &mt9v113_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9v113_data = {
	.sensor_name	= "mt9v113",
	.pdata	= &msm_camera_csi_device_data[1],
	.flash_data	= &flash_mt9v113,
	.sensor_platform_info = &sensor_board_info_mt9v113,
	.csi_if	= 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type          = YUV_SENSOR, 
	.actuator_info    = NULL,
};
static struct msm_camera_csi_lane_params ov5647_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};
static struct msm_gpio_set_tbl ov5647_cam_gpio_set_tbl[] = {
	{54, GPIOF_OUT_INIT_LOW, 5000},
	{54, GPIOF_OUT_INIT_HIGH, 1000},
	{107, GPIOF_OUT_INIT_HIGH, 3000},
};
static struct msm_gpio_set_tbl ov5647_gpio_req_init_tbl[] = {
	{54, GPIOF_OUT_INIT_LOW, 1000},
	{107, GPIOF_OUT_INIT_LOW, 1000},
};
static struct msm_actuator_info msm_act_main_cam_5_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_5,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = 13,
	.vcm_enable     = 1,
};

static struct msm_camera_gpio_conf msm_8930_ov5647_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_back_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio),
	.cam_gpio_set_tbl = ov5647_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(ov5647_cam_gpio_set_tbl),
	.cam_gpio_req_init_tbl = ov5647_gpio_req_init_tbl,
	.cam_gpio_req_init_tbl_size = ARRAY_SIZE(ov5647_gpio_req_init_tbl),
};

static struct msm_camera_sensor_flash_data flash_ov5647 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov5647 = {
	.mount_angle	= 90, 
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_ov5647_cam_gpio_conf,
	.csi_lane_params = &ov5647_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov5647_data = {
	.sensor_name	= "ov5647",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_ov5647,
	.sensor_platform_info = &sensor_board_info_ov5647,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type          = BAYER_SENSOR, 
	.actuator_info    = &msm_act_main_cam_5_info,
};
static struct gpio msm8930_cam_s5k4e1_gpio[] = {
	{5, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{107, GPIOF_DIR_OUT, "CAM_RESET"},

};
static struct msm_gpio_set_tbl msm8930_cam_s5k4e1_gpio_set_tbl[] = {
	{107, GPIOF_OUT_INIT_LOW, 1000},
	{107, GPIOF_OUT_INIT_HIGH, 4000},
};

static struct msm_gpio_set_tbl s5k4e1_gpio_req_init_tbl[] = {
	{107, GPIOF_OUT_INIT_LOW, 1000},
};
static struct msm_camera_gpio_conf msm_cam_s5k4e1_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_cam_s5k4e1_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_cam_s5k4e1_gpio),
	.cam_gpio_set_tbl = msm8930_cam_s5k4e1_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_cam_s5k4e1_gpio_set_tbl),
	.cam_gpio_req_init_tbl = s5k4e1_gpio_req_init_tbl,
	.cam_gpio_req_init_tbl_size = ARRAY_SIZE(s5k4e1_gpio_req_init_tbl),

};

static struct msm_camera_csi_lane_params s5k4e1_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};

static struct msm_camera_sensor_flash_data flash_s5k4e1 = {
	.flash_type	     = MSM_CAMERA_FLASH_LED,
	.flash_src	      = &msm_flash_src
};


static struct msm_camera_sensor_platform_info sensor_board_info_s5k4e1 = {
	.mount_angle	= 90, 
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_cam_s5k4e1_gpio_conf,
	.csi_lane_params = &s5k4e1_csi_lane_params,

};

static struct msm_actuator_info msm_act_main_cam_4_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_4,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = 13,
	.vcm_enable     = 1,
};


static struct msm_camera_sensor_info msm_camera_sensor_s5k4e1_data = {
	.sensor_name	= "s5k4e1",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_s5k4e1,
	.sensor_platform_info = &sensor_board_info_s5k4e1,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type          = BAYER_SENSOR, 
	.actuator_info    = &msm_act_main_cam_4_info,
};

static struct msm_camera_csi_lane_params s5k5ca_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};
static struct msm_gpio_set_tbl s5k5ca_gpio_req_init_tbl[] = {
	{54, GPIOF_OUT_INIT_HIGH, 0},  //set the flag of gpio, then usleep the times
	{107, GPIOF_OUT_INIT_LOW, 10000},
};
static struct msm_gpio_set_tbl s5k5ca_cam_gpio_set_tbl[] = {
	{54, GPIOF_OUT_INIT_LOW, 20000},  //set the flag of gpio, then usleep the times
	{107, GPIOF_OUT_INIT_HIGH, 20000},
};
/* power down config table */
static struct msm_gpio_set_tbl s5k5ca_cam_gpio_config_tbl_power_down[] = {
	{107, GPIOF_OUT_INIT_LOW, 20000},
	{54, GPIOF_OUT_INIT_HIGH, 150000},
};
static struct msm_camera_gpio_conf msm_8930_s5k5ca_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_back_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio),
	.cam_gpio_set_tbl = s5k5ca_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(s5k5ca_cam_gpio_set_tbl),
	.cam_gpio_req_init_tbl = s5k5ca_gpio_req_init_tbl,
	.cam_gpio_req_init_tbl_size = ARRAY_SIZE(s5k5ca_gpio_req_init_tbl),
	.cam_gpio_config_tbl_power_down = s5k5ca_cam_gpio_config_tbl_power_down,
	.cam_gpio_config_tbl_power_down_size = ARRAY_SIZE(s5k5ca_cam_gpio_config_tbl_power_down),
};

/* Add flash for s5k5ca, change FLASH_NONE to FLASH_LED */
static struct msm_camera_sensor_flash_data flash_s5k5ca = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src = &msm_flash_src
};
static struct camera_vreg_t msm_8930_cam_vreg_s5k5ca[] = {
	{"cam_iovdd", REG_VS, 0, 0, 0,300},
	{"cam_avdd", REG_LDO, 2800000, 2850000, 85600},
};
static struct msm_camera_sensor_platform_info sensor_board_info_s5k5ca = {
	.mount_angle	= 90, 
	.cam_vreg = msm_8930_cam_vreg_s5k5ca,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg_s5k5ca),
	.gpio_conf = &msm_8930_s5k5ca_cam_gpio_conf,
	.csi_lane_params = &s5k5ca_csi_lane_params,
};
static struct msm_camera_sensor_info msm_camera_sensor_s5k5ca_data = {
	.sensor_name	= "s5k5ca",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_s5k5ca,
	.sensor_platform_info = &sensor_board_info_s5k5ca,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type          = YUV_SENSOR, 
	.actuator_info    = NULL,
	.standby_is_supported = 1,
	.en_clk_first=1,
	.sensor_pwd= 54,
};
static struct msm_camera_csi_lane_params mt9t113_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};

static struct msm_gpio_set_tbl mt9t113_cam_gpio_set_tbl[] = {
	{107, GPIOF_OUT_INIT_LOW, 20000},  //set the flag of gpio, then usleep the times
	{107, GPIOF_OUT_INIT_HIGH, 20000},
};

static struct msm_gpio_set_tbl mt9t113_gpio_req_init_tbl[] = {
	{107, GPIOF_OUT_INIT_HIGH, 0},  //set the flag of gpio, then usleep the times
	{54, GPIOF_OUT_INIT_LOW, 10000},
};

static struct msm_camera_gpio_conf msm_8930_mt9t113_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_back_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio),
	.cam_gpio_set_tbl = mt9t113_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(mt9t113_cam_gpio_set_tbl),
	.cam_gpio_req_init_tbl = mt9t113_gpio_req_init_tbl,
	.cam_gpio_req_init_tbl_size = ARRAY_SIZE(mt9t113_gpio_req_init_tbl),
};

static struct msm_camera_sensor_flash_data flash_mt9t113 = {
	.flash_type = MSM_CAMERA_FLASH_NONE,
	.flash_src = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_mt9t113 = {
	.mount_angle	= 90, 
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_mt9t113_cam_gpio_conf,
	.csi_lane_params = &mt9t113_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9t113_data = {
	.sensor_name	= "mt9t113",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_mt9t113,
	.sensor_platform_info = &sensor_board_info_mt9t113,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type          = YUV_SENSOR, 
	.actuator_info    = NULL,
};
static struct platform_device msm_camera_server = {
	.name = "msm_cam_server",
	.id = 0,
};

static struct msm_gpio_set_tbl msm8930_cam_imx188_gpio_set_tbl[] = {
	{76, GPIOF_OUT_INIT_HIGH, 1000},
};

static struct msm_gpio_set_tbl imx188_gpio_req_init_tbl[] = {
	{76, GPIOF_OUT_INIT_LOW, 1000},
};

static struct msm_camera_gpio_conf msm_cam_imx188_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_front_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_front_cam_gpio),
	.cam_gpio_set_tbl = msm8930_cam_imx188_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_cam_imx188_gpio_set_tbl),
	.cam_gpio_req_init_tbl = imx188_gpio_req_init_tbl,
	.cam_gpio_req_init_tbl_size = ARRAY_SIZE(imx188_gpio_req_init_tbl),

};

static struct msm_camera_csi_lane_params imx188_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};

static struct msm_camera_sensor_flash_data flash_imx188 = {
	.flash_type	     = MSM_CAMERA_FLASH_NONE,
	.flash_src	      = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_imx188 = {
	.mount_angle	= 270, 
	.cam_vreg = msm_imx188_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_imx188_cam_vreg),
	.gpio_conf = &msm_cam_imx188_gpio_conf,
	.csi_lane_params = &imx188_csi_lane_params,

};

static struct msm_camera_sensor_info msm_camera_sensor_imx188_data = {
	.sensor_name	= "imx188",
	.pdata	= &msm_camera_csi_device_data[1],
	.flash_data	= &flash_imx188,
	.sensor_platform_info = &sensor_board_info_imx188,
	.csi_if	= 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type          = BAYER_SENSOR, 
	.actuator_info    = NULL,
};

#ifdef CONFIG_IMX134
static struct msm_camera_csi_lane_params imx134_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};

static struct camera_vreg_t msm_imx134_cam_vreg[] = {
	{"cam_avdd", REG_LDO, 2850000, 2850000, 85600,0},
	{"cam_iovdd", REG_VS, 0, 0, 0,0},
	{"cam_dvdd", REG_LDO, 1100000, 1100000,200000,2000},
};

static struct msm_gpio_set_tbl imx134_cam_gpio_set_tbl[] = {
	{107, GPIOF_OUT_INIT_HIGH, 2000}, //RESET
};
static struct msm_gpio_set_tbl imx134_gpio_req_init_tbl[] = {
	{107, GPIOF_OUT_INIT_LOW, 2000},
};
static struct msm_gpio_set_tbl imx134_gpio_power_down_tbl[] = {
	{107, GPIOF_OUT_INIT_LOW, 2000},
};

static struct msm_actuator_info msm_act_main_cam_6_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name       = MSM_ACTUATOR_MAIN_CAM_6,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = 13,
	.vcm_enable     = 1,
};

static struct msm_camera_gpio_conf msm_8930_imx134_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_back_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio),
	.cam_gpio_set_tbl = imx134_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(imx134_cam_gpio_set_tbl),
	.cam_gpio_req_init_tbl = imx134_gpio_req_init_tbl,
	.cam_gpio_req_init_tbl_size = ARRAY_SIZE(imx134_gpio_req_init_tbl),
	.cam_gpio_config_tbl_power_down = imx134_gpio_power_down_tbl,
	.cam_gpio_config_tbl_power_down_size = ARRAY_SIZE(imx134_gpio_power_down_tbl),
};

static struct msm_camera_sensor_flash_data flash_imx134 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_imx134 = {
	.mount_angle	= 90, 
	.cam_vreg = msm_imx134_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_imx134_cam_vreg),
	.gpio_conf = &msm_8930_imx134_cam_gpio_conf,
	.csi_lane_params = &imx134_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_imx134_data = {
	.sensor_name	= "imx134",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_imx134,
	.sensor_platform_info = &sensor_board_info_imx134,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type          = BAYER_SENSOR, 
	.actuator_info    = &msm_act_main_cam_6_info,
};
#endif

static struct msm_gpio_set_tbl msm8930_cam_ov9724_gpio_set_tbl[] = {
	{76, GPIOF_OUT_INIT_HIGH, 1000},
};

static struct msm_gpio_set_tbl ov9724_gpio_req_init_tbl[] = {
	{76, GPIOF_OUT_INIT_LOW, 1000},
};

static struct msm_camera_gpio_conf msm_cam_ov9724_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_front_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_front_cam_gpio),
	.cam_gpio_set_tbl = msm8930_cam_ov9724_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_cam_ov9724_gpio_set_tbl),
	.cam_gpio_req_init_tbl = ov9724_gpio_req_init_tbl,
	.cam_gpio_req_init_tbl_size = ARRAY_SIZE(ov9724_gpio_req_init_tbl),
};

static struct msm_camera_csi_lane_params ov9724_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};

static struct msm_camera_sensor_flash_data flash_ov9724 = {
	.flash_type	     = MSM_CAMERA_FLASH_NONE,
	.flash_src	      = &msm_flash_src
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov9724 = {
	.mount_angle	= 270, 
	.cam_vreg = msm_ov9724_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_ov9724_cam_vreg),
	.gpio_conf = &msm_cam_ov9724_gpio_conf,
	.csi_lane_params = &ov9724_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov9724_data = {
	.sensor_name	= "ov9724",
	.pdata	= &msm_camera_csi_device_data[1],
	.flash_data	= &flash_ov9724,
	.sensor_platform_info = &sensor_board_info_ov9724,
	.csi_if	= 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type          = BAYER_SENSOR, 
	.actuator_info    = NULL,
};



#ifdef CONFIG_S5K9A1

static struct msm_camera_sensor_flash_data flash_s5k9a1 = {
	.flash_type = MSM_CAMERA_FLASH_NONE,
};
/* Since main camear use same DVDD with slave camera, 
 * modify the DVDD to accommodate main camera votage. 
 */
static struct camera_vreg_t msm_8930_cam_vreg_s5k9a1[] = {
	{"cam_avdd", REG_LDO, 2850000, 2850000, 85600,0},
	{"cam_iovdd", REG_VS, 0, 0, 0,1000},
    {"cam_dvdd", REG_LDO, 1150000, 1150000,0,10},
};


// TODO: to check the init status of each GPIO

static struct msm_gpio_set_tbl msm8930_cam_s5k9a1_gpio_set_tbl[] = {
	{76, GPIOF_OUT_INIT_HIGH, 1000},
};

static struct msm_gpio_set_tbl s5k9a1_gpio_req_init_tbl[] = {
	{76, GPIOF_OUT_INIT_LOW, 1000},
};

static struct msm_camera_gpio_conf msm_8930_s5k9a1_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_front_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_front_cam_gpio),
	.cam_gpio_set_tbl = msm8930_cam_s5k9a1_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_cam_s5k9a1_gpio_set_tbl),
	.cam_gpio_req_init_tbl = s5k9a1_gpio_req_init_tbl,
	.cam_gpio_req_init_tbl_size = ARRAY_SIZE(s5k9a1_gpio_req_init_tbl),
};

static struct msm_camera_csi_lane_params s5k9a1_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};

static struct msm_camera_sensor_platform_info sensor_board_info_s5k9a1 = {
    // TODO:  need to check the mout type
	.mount_angle	= 270, 
	.cam_vreg = msm_8930_cam_vreg_s5k9a1,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg_s5k9a1),
	.gpio_conf = &msm_8930_s5k9a1_cam_gpio_conf,
	.csi_lane_params = &s5k9a1_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k9a1_data = {
    .sensor_name    = "s5k9a1",
    .pdata  = &msm_camera_csi_device_data[1],/* front sensor hardware connection is csi[1] */
    .flash_data = &flash_s5k9a1,
    .sensor_platform_info = &sensor_board_info_s5k9a1,
    .csi_if = 1,
    .camera_type = FRONT_CAMERA_2D,
    .sensor_type          = BAYER_SENSOR, 
    .actuator_info    = NULL,
};
#endif




void __init msm8930_init_cam(void)
{
	msm_gpiomux_install(msm8930_cam_common_configs,
			ARRAY_SIZE(msm8930_cam_common_configs));

	if (machine_is_msm8930_cdp()) {
		struct msm_camera_sensor_info *s_info;
		s_info = &msm_camera_sensor_s5k3l1yx_data;
		s_info->sensor_platform_info->mount_angle = 0;
		msm_flash_src._fsrc.ext_driver_src.led_en =
			GPIO_CAM_GP_LED_EN1;
		msm_flash_src._fsrc.ext_driver_src.led_flash_en =
			GPIO_CAM_GP_LED_EN2;
#if defined(CONFIG_I2C) && (defined(CONFIG_GPIO_SX150X) || \
	defined(CONFIG_GPIO_SX150X_MODULE))
		msm_flash_src._fsrc.ext_driver_src.expander_info =
			cam_expander_info;
#endif
	}

	platform_device_register(&msm_camera_server);
	platform_device_register(&msm8960_device_csiphy0);
	platform_device_register(&msm8960_device_csiphy1);
	platform_device_register(&msm8960_device_csid0);
	platform_device_register(&msm8960_device_csid1);
	platform_device_register(&msm8960_device_ispif);
	platform_device_register(&msm8960_device_vfe);
	platform_device_register(&msm8960_device_vpe);
}

#ifdef CONFIG_I2C
struct i2c_board_info msm8930_camera_i2c_boardinfo[] = {
	{
	//I2C_BOARD_INFO("imx074", 0x1A),
	.platform_data = &msm_camera_sensor_imx074_data,
	},	
	{
	//I2C_BOARD_INFO("ov2720", 0x6C),
	.platform_data = &msm_camera_sensor_ov2720_data,
	},
	{
	I2C_BOARD_INFO("mt9m114", 0x48),
	.platform_data = &msm_camera_sensor_mt9m114_data,
	},
	{
	//I2C_BOARD_INFO("s5k3l1yx", 0x20),
	.platform_data = &msm_camera_sensor_s5k3l1yx_data,
	},
	{
	I2C_BOARD_INFO("mt9e013", 0x6C),
	.platform_data = &msm_camera_sensor_mt9e013_data,
	},
	{
		I2C_BOARD_INFO("s5k4e1", 0x37),
		.platform_data = &msm_camera_sensor_s5k4e1_data,
 	},

	/*delete tps61310  define*/
#ifdef CONFIG_GC0313
	{
		I2C_BOARD_INFO("gc0313", 0x42),
		.platform_data = &msm_camera_sensor_gc0313_data,
	},
#endif	
	/*attention:the actual i2c addr : 0xdc!*/
#ifdef CONFIG_BF3905
	{
		I2C_BOARD_INFO("bf3905", 0x6e),
		.platform_data = &msm_camera_sensor_bf3905_data,
	},
#endif

	{
	I2C_BOARD_INFO("mt9v113", 0x7A),
	.platform_data = &msm_camera_sensor_mt9v113_data,
	},
	{
	I2C_BOARD_INFO("ov5647", 0x36),
	.platform_data = &msm_camera_sensor_ov5647_data,
	},
	{
	I2C_BOARD_INFO("s5k5ca", 0x5A),
	.platform_data = &msm_camera_sensor_s5k5ca_data,
	},

#ifdef CONFIG_S5K9A1
	{
	I2C_BOARD_INFO("s5k9a1", 0x50),
	.platform_data = &msm_camera_sensor_s5k9a1_data,
	},
#endif
    
	{
	I2C_BOARD_INFO("mt9t113", 0x78),
	.platform_data = &msm_camera_sensor_mt9t113_data,
	},
	{
	I2C_BOARD_INFO("imx188", 0x34),
	.platform_data = &msm_camera_sensor_imx188_data,
	},
#ifdef CONFIG_IMX134
	{
	I2C_BOARD_INFO("imx134", 0x20),
	.platform_data = &msm_camera_sensor_imx134_data,
	},
#endif
	{
	I2C_BOARD_INFO("ov9724", 0x1B),
	.platform_data = &msm_camera_sensor_ov9724_data,
	},
};

struct msm_camera_board_info msm8930_camera_board_info = {
	.board_info = msm8930_camera_i2c_boardinfo,
	.num_i2c_board_info = ARRAY_SIZE(msm8930_camera_i2c_boardinfo),
};
#endif
#endif
