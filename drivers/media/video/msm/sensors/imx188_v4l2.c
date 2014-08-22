/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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

#include "msm_sensor.h"
#define SENSOR_NAME "imx188"
#define PLATFORM_DRIVER_NAME "msm_camera_imx188"
#define imx188_obj imx188_##obj
#define IMX188_CAMERA_ID_GPIO  75

DEFINE_MUTEX(imx188_mut);
static struct msm_sensor_ctrl_t imx188_s_ctrl;

static struct msm_camera_i2c_reg_conf imx188_start_settings[] = {
	{0x0100, 0x01},
};

static struct msm_camera_i2c_reg_conf imx188_stop_settings[] = {
	{0x0100, 0x00},
};

static struct msm_camera_i2c_reg_conf imx188_groupon_settings[] = {
	{0x0104, 0x01},
};

static struct msm_camera_i2c_reg_conf imx188_groupoff_settings[] = {
	{0x0104, 0x00},
};

static struct msm_camera_i2c_reg_conf imx188_init_settings[] = {
	{0x3094,	0x32}, 
	{0x309A,	0xA3}, 
	{0x309E,	0x00}, 
	{0x3166,	0x1C}, 
	{0x3167, 0x1B},
	{0x3168, 0x32},
	{0x3169, 0x31},
	{0x316A, 0x1C},
	{0x316B, 0x1B},
	{0x316C, 0x32},
	{0x316D, 0x31},
	{0x0305, 0x04},
	{0x0307, 0x87},
	{0x303C, 0x4B},
	{0x30A4, 0x02},
	{0x0112, 0x0A},
	{0x0113, 0x0A},
	{0x0340, 0x03},
	{0x0341, 0x84},
	{0x0342, 0x0B},
	{0x0343, 0xB8},
	{0x0344, 0x00},
	{0x0345, 0x08},
	{0x0346, 0x00},
	{0x0347, 0x28},
	{0x0348, 0x05},
	{0x0349, 0x17},
	{0x034A, 0x03},
	{0x034B, 0x08},
	{0x034C, 0x05},
	{0x034D, 0x10},
	{0x034E, 0x02},
	{0x034F, 0xE1},
	{0x0381, 0x01},
	{0x0383, 0x01},
	{0x0385, 0x01},
	{0x0387, 0x01},
	{0x3040, 0x08},
	{0x3041, 0x97},
	{0x3048, 0x00},
	{0x304E, 0x0A},
	{0x3050, 0x02},
	{0x309B, 0x00},
	{0x30D5, 0x00},
	{0x31A1, 0x07},
	{0x31A2, 0x65},
	{0x31A3, 0x04},
	{0x31A4, 0xFC},
	{0x31A5, 0x08},
	{0x31AA, 0x65},
	{0x31AB, 0x04},
	{0x31AC, 0x60},
	{0x31AD, 0x09},
	{0x31B0, 0x00},
	{0x3301, 0x05},
	{0x3318, 0x66},
	{0x0202, 0x03},
	{0x0203, 0x7F},
	{0x0205, 0xE0},
	{0x0101, 0x03},

};

static struct v4l2_subdev_info imx188_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_SGRBG10_1X10,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};

static struct msm_camera_i2c_conf_array imx188_init_conf[] = {
	{&imx188_init_settings[0],
	ARRAY_SIZE(imx188_init_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_camera_i2c_conf_array imx188_confs[] = {
	{NULL,0, 0, MSM_CAMERA_I2C_BYTE_DATA},
	{NULL,0, 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_sensor_output_info_t imx188_dimensions[] = {	
	{
		.x_output = 0x510,/*1296*/
		.y_output = 0x2E1,/*737*/
		.line_length_pclk = 0xBB8,/*hts*/
		.frame_length_lines = 0x384,/*vts*/
		.vt_pixel_clk =81000000,/*hts*vts*framerate*/
		.op_pixel_clk = 40500000,/*mipi data rate / bpp*/
		.binning_factor = 0,
	},
	{
		.x_output = 0x510,/*1296*/
		.y_output = 0x2E1,/*737*/
		.line_length_pclk = 0xBB8,
		.frame_length_lines = 0x384,
		.vt_pixel_clk = 81000000,
		.op_pixel_clk =40500000,
		.binning_factor = 0,
	},
	
};

static struct msm_sensor_output_reg_addr_t imx188_reg_addr = {
	.x_output = 0x034C,
	.y_output = 0x034E,
	.line_length_pclk = 0x0342,
	.frame_length_lines = 0x0340,
};

static struct msm_sensor_id_info_t imx188_id_info = {
	.sensor_id_reg_addr = 0x0000,
	.sensor_id = 0x0188,
};

static struct msm_sensor_exp_gain_info_t imx188_exp_gain_info = {
	.coarse_int_time_addr = 0x0202,
	.global_gain_addr = 0x0204,
	.vert_offset = 5,
};

static int32_t imx188_sensor_match_module(struct msm_sensor_ctrl_t *s_ctrl)
{

	//check mode type
	if(!gpio_request(IMX188_CAMERA_ID_GPIO, "imx188"))
	{	
		/*MODULE_ID of BYD module is connected to DOVDD,MODULE_ID of Sunny module is connected to DGND*/
		if(gpio_get_value(IMX188_CAMERA_ID_GPIO) == 1)
		{
			s_ctrl->sensor_name="23060114FF-IMX-B";
		}
		else
		{
			s_ctrl->sensor_name="23060114FF-IMX-S";
			s_ctrl->sensordata->sensor_platform_info->mount_angle=0;	
		}
		gpio_free(IMX188_CAMERA_ID_GPIO);
	}
	else
	{
		printk("%s: gpio request fail\n",__func__);
		s_ctrl->sensor_name="23060114FF-IMX-B";
	}
	printk("%s: module name=%s\n",__func__,s_ctrl->sensor_name);
	return 0;
}

static const struct i2c_device_id imx188_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&imx188_s_ctrl},
	{ }
};

static struct i2c_driver imx188_i2c_driver = {
	.id_table = imx188_i2c_id,
	.probe  = msm_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client imx188_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static int __init msm_sensor_init_module(void)
{
	return i2c_add_driver(&imx188_i2c_driver);
}

static struct v4l2_subdev_core_ops imx188_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops imx188_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops imx188_subdev_ops = {
	.core = &imx188_subdev_core_ops,
	.video  = &imx188_subdev_video_ops,
};

static struct msm_sensor_fn_t imx188_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = msm_sensor_set_fps,
	.sensor_write_exp_gain = msm_sensor_write_exp_gain1,
	.sensor_write_snapshot_exp_gain = msm_sensor_write_exp_gain1,
	.sensor_setting =msm_sensor_setting,
	.sensor_csi_setting = msm_sensor_setting1,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = msm_sensor_power_up,
	.sensor_power_down = msm_sensor_power_down,
	.sensor_get_csi_params = msm_sensor_get_csi_params,
	.sensor_match_module = imx188_sensor_match_module,
};

static struct msm_sensor_reg_t imx188_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf =imx188_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(imx188_start_settings),
	.stop_stream_conf = imx188_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(imx188_stop_settings),
	.group_hold_on_conf = imx188_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(imx188_groupon_settings),
	.group_hold_off_conf =imx188_groupoff_settings,
	.group_hold_off_conf_size =
		ARRAY_SIZE(imx188_groupoff_settings),
	.init_settings = &imx188_init_conf[0],
	.init_size = ARRAY_SIZE(imx188_init_conf),
	.mode_settings = &imx188_confs[0],
	.output_settings = &imx188_dimensions[0],
	.num_conf = ARRAY_SIZE(imx188_confs),
};

static struct msm_sensor_ctrl_t imx188_s_ctrl = {
	.msm_sensor_reg = &imx188_regs,
	.sensor_i2c_client = &imx188_sensor_i2c_client,
	.sensor_i2c_addr = 0x34,
	.sensor_output_reg_addr = &imx188_reg_addr,
	.sensor_id_info = &imx188_id_info,
	.sensor_exp_gain_info = &imx188_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.msm_sensor_mutex = &imx188_mut,
	.sensor_i2c_driver = &imx188_i2c_driver,
	.sensor_v4l2_subdev_info = imx188_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(imx188_subdev_info),
	.sensor_v4l2_subdev_ops = &imx188_subdev_ops,
	.func_tbl = &imx188_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
	.sensor_name = "23060114FF-IMX-B",
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Sony 1MP Bayer sensor driver");
MODULE_LICENSE("GPL v2");
