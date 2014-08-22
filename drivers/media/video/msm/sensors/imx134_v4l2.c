/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
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
#include "msm.h"
#define SENSOR_NAME "imx134"
#define PLATFORM_DRIVER_NAME "msm_camera_imx134"
#define imx134_obj imx134_##obj

DEFINE_MUTEX(imx134_mut);
static struct msm_sensor_ctrl_t imx134_s_ctrl;
 
static struct msm_camera_i2c_reg_conf imx134_start_settings[] = {
	{0x0100, 0x01},
};

static struct msm_camera_i2c_reg_conf imx134_stop_settings[] = {
	{0x0100, 0x00},
};

static struct msm_camera_i2c_reg_conf imx134_groupon_settings[] = {
	{0x0104, 0x01},
};

static struct msm_camera_i2c_reg_conf imx134_groupoff_settings[] = {
	{0x0104, 0x00},
};

static struct msm_camera_i2c_reg_conf imx134_prev_settings[] = {
	//1640*1232 @30fps
	//Clock Setting
	{0x011E, 0x18},
	{0x011F, 0x00},
	{0x0301, 0x05},
	{0x0303, 0x01},
	{0x0305, 0x0C},
	{0x0309, 0x05},
	{0x030B, 0x01},
	{0x030C, 0x00},
	{0x030D, 0xC8},
	{0x030E, 0x01},
	{0x3A06, 0x11},
	//Mode Setting
	{0x0108, 0x03},
	{0x0112, 0x0A},
	{0x0113, 0x0A},
	{0x0381, 0x01},
	{0x0383, 0x01},
	{0x0385, 0x01},
	{0x0387, 0x01},
	{0x0390, 0x01},
	{0x0391, 0x22},
	{0x0392, 0x00},
	{0x0401, 0x00},
	{0x0404, 0x00},
	{0x0405, 0x10},
	{0x4082, 0x01},
	{0x4083, 0x01},
	{0x7006, 0x04},
	//Optionnal Function Setting
	{0x0700, 0x00},
	{0x3A63, 0x00},
	{0x4100, 0xF8},
	{0x4203, 0xFF},
	{0x4344, 0x00},
	{0x441C, 0x01},
	//Size setting
	{0x0340, 0x05},
	{0x0341, 0xC8},
	{0x0342, 0x0E},
	{0x0343, 0x10},
	{0x0344, 0x00},
	{0x0345, 0x00},
	{0x0346, 0x00},
	{0x0347, 0x00},
	{0x0348, 0x0C},
	{0x0349, 0xCF},
	{0x034A, 0x09},
	{0x034B, 0x9F},
	{0x034C, 0x06},
	{0x034D, 0x68},
	{0x034E, 0x04},
	{0x034F, 0xD0},
	{0x0350, 0x00},
	{0x0351, 0x00},
	{0x0352, 0x00},
	{0x0353, 0x00},
	{0x0354, 0x06},
	{0x0355, 0x68},
	{0x0356, 0x04},
	{0x0357, 0xD0},
	{0x301D, 0x30},
	{0x3310, 0x06},
	{0x3311, 0x68},
	{0x3312, 0x04},
	{0x3313, 0xD0},
	{0x331C, 0x04},
	{0x331D, 0x06},
	{0x4084, 0x00},
	{0x4085, 0x00},
	{0x4086, 0x00},
	{0x4087, 0x00},
	{0x4400, 0x00},
	//Golbal Timing Setting
	{0x0830, 0x67},
	{0x0831, 0x1F},
	{0x0832, 0x47},
	{0x0833, 0x1F},
	{0x0834, 0x1F},
	{0x0835, 0x17},
	{0x0836, 0x77},
	{0x0837, 0x27},
	{0x0839, 0x1F},
	{0x083A, 0x17},
	{0x083B, 0x02},
	//Intergration Timg setting
	{0x0202, 0x05},
	{0x0203, 0xC4},
	//Gain Setting
	{0x0205, 0x00},
	{0x020E, 0x01},
	{0x020F, 0x00},
	{0x0210, 0x01},
	{0x0211, 0x00},
	{0x0212, 0x01},
	{0x0213, 0x00},
	{0x0214, 0x01},
	{0x0215, 0x00},
	//HDR Setting
	{0x0230, 0x00},
	{0x0231, 0x00},
	{0x0233, 0x00},
	{0x0234, 0x00},
	{0x0235, 0x40},
	{0x0238, 0x00},
	{0x0239, 0x04},
	{0x023B, 0x00},
	{0x023C, 0x01},
	{0x33B0, 0x04},
	{0x33B1, 0x00},
	{0x33B3, 0x00},
	{0x33B4, 0x01},
	{0x3800, 0x00},
	//Mode Clear
	{0x3A43,	0x01},
};

static struct msm_camera_i2c_reg_conf imx134_snap_settings[] = {
	//3280*2464 @15fps
	//Clock Setting
	{0x011E, 0x18},
	{0x011F, 0x00},
	{0x0301, 0x05},
	{0x0303, 0x01},
	{0x0305, 0x0C},
	{0x0309, 0x05},
	{0x030B, 0x01},
	{0x030C, 0x00},
	{0x030D, 0xC8},
	{0x030E, 0x01},
	{0x3A06, 0x11},
	//Mode setting
	{0x0108, 0x03},
	{0x0112, 0x0A},
	{0x0113, 0x0A},
	{0x0381, 0x01},
	{0x0383, 0x01},
	{0x0385, 0x01},
	{0x0387, 0x01},
	{0x0390, 0x00},
	{0x0391, 0x11},
	{0x0392, 0x00},
	{0x0401, 0x00},
	{0x0404, 0x00},
	{0x0405, 0x10},
	{0x4082, 0x01},
	{0x4083, 0x01},
	{0x7006, 0x04},
	//OptionnalFunction setting
	{0x0700, 0x00},
	{0x3A63, 0x00},
	{0x4100, 0xF8},
	{0x4203, 0xFF},
	{0x4344, 0x00},
	{0x441C, 0x01},
	//Size setting
	{0x0340, 0x0B},
	{0x0341, 0x90},
	{0x0342, 0x0E},
	{0x0343, 0x10},
	{0x0344, 0x00},
	{0x0345, 0x00},
	{0x0346, 0x00},
	{0x0347, 0x00},
	{0x0348, 0x0C},
	{0x0349, 0xCF},
	{0x034A, 0x09},
	{0x034B, 0x9F},
	{0x034C, 0x0C},
	{0x034D, 0xD0},
	{0x034E, 0x09},
	{0x034F, 0xA0},
	{0x0350, 0x00},
	{0x0351, 0x00},
	{0x0352, 0x00},
	{0x0353, 0x00},
	{0x0354, 0x0C},
	{0x0355, 0xD0},
	{0x0356, 0x09},
	{0x0357, 0xA0},
	{0x301D, 0x30},
	{0x3310, 0x0C},
	{0x3311, 0xD0},
	{0x3312, 0x09},
	{0x3313, 0xA0},
	{0x331C, 0x01},
	{0x331D, 0xAE},
	{0x4084, 0x00},
	{0x4085, 0x00},
	{0x4086, 0x00},
	{0x4087, 0x00},
	{0x4400, 0x00},
	//Global Timing Setting
	{0x0830, 0x67},
	{0x0831, 0x1F},
	{0x0832, 0x47},
	{0x0833, 0x1F},
	{0x0834, 0x1F},
	{0x0835, 0x17},
	{0x0836, 0x77},
	{0x0837, 0x27},
	{0x0839, 0x1F},
	{0x083A, 0x17},
	{0x083B, 0x02},
	//Integration Time Setting
	{0x0202, 0x0B},
	{0x0203, 0x8C},
	//Gain Setting
	{0x0205, 0x00},
	{0x020E, 0x01},
	{0x020F, 0x00},
	{0x0210, 0x01},
	{0x0211, 0x00},
	{0x0212, 0x01},
	{0x0213, 0x00},
	{0x0214, 0x01},
	{0x0215, 0x00},
	//HDR Setting
	{0x0230, 0x00},
	{0x0231, 0x00},
	{0x0233, 0x00},
	{0x0234, 0x00},
	{0x0235, 0x40},
	{0x0238, 0x00},
	{0x0239, 0x04},
	{0x023B, 0x00},
	{0x023C, 0x01},
	{0x33B0, 0x04},
	{0x33B1, 0x00},
	{0x33B3, 0x00},
	{0x33B4, 0x01},
	{0x3800, 0x00},
	//Mode Clear
	{0x3A43,	0x01},
};
static struct msm_camera_i2c_reg_conf imx134_1080p_settings[] = {	
	//1920*1088 @30fps
	//Clock Setting
	{0x011E, 0x18},
	{0x011F, 0x00},
	{0x0301, 0x06},
	{0x0303, 0x01},
	{0x0305, 0x06},
	{0x0309, 0x05},
	{0x030B, 0x02},
	{0x030C, 0x00},
	{0x030D, 0xC8},
	{0x030E, 0x01},
	{0x3A06, 0x12},
	//Mode Setting
	{0x0108, 0x03},
	{0x0112, 0x0A},
	{0x0113, 0x0A},
	{0x0381, 0x01},
	{0x0383, 0x01},
	{0x0385, 0x01},
	{0x0387, 0x01},
	{0x0390, 0x00},
	{0x0391, 0x11},
	{0x0392, 0x00},
	{0x0401, 0x02},
	{0x0404, 0x00},
	{0x0405, 0x1B},
	{0x4082, 0x00},
	{0x4083, 0x00},
	{0x7006, 0x04},
	//Optionnal Function Setting
	{0x0700, 0x00},
	{0x3A63, 0x00},
	{0x4100, 0xF8},
	{0x4203, 0xFF},
	{0x4344, 0x00},
	{0x441C, 0x01},
	//Size setting
	{0x0340, 0x09},
	{0x0341, 0xB6},
	{0x0342, 0x0E},
	{0x0343, 0x10},
	{0x0344, 0x00},
	{0x0345, 0x00},
	{0x0346, 0x00},
	{0x0347, 0x00},
	{0x0348, 0x0C},
	{0x0349, 0xCF},
	{0x034A, 0x09},
	{0x034B, 0x9F},
	{0x034C, 0x07},
	{0x034D, 0x80},
	{0x034E, 0x04},
	{0x034F, 0x40},
	{0x0350, 0x00},
	{0x0351, 0x14},
	{0x0352, 0x01},
	{0x0353, 0x3A},
	{0x0354, 0x0C},
	{0x0355, 0xA8},
	{0x0356, 0x07},
	{0x0357, 0x2C},
	{0x301D, 0x30},
	{0x3310, 0x07},
	{0x3311, 0x80},
	{0x3312, 0x04},
	{0x3313, 0x40},
	{0x331C, 0x00},
	{0x331D, 0xBE},
	{0x4084, 0x07},
	{0x4085, 0x80},
	{0x4086, 0x04},
	{0x4087, 0x40},
	{0x4400, 0x00},
	//Golbal Timing Setting
	{0x0830, 0x67},
	{0x0831, 0x1F},
	{0x0832, 0x47},
	{0x0833, 0x1F},
	{0x0834, 0x1F},
	{0x0835, 0x17},
	{0x0836, 0x77},
	{0x0837, 0x27},
	{0x0839, 0x1F},
	{0x083A, 0x17},
	{0x083B, 0x02},
	//Integration Time Setting
	{0x0202, 0x09},
	{0x0203, 0xB2},
	//Gain Setting
	{0x0205, 0x00},
	{0x020E, 0x01},
	{0x020F, 0x00},
	{0x0210, 0x01},
	{0x0211, 0x00},
	{0x0212, 0x01},
	{0x0213, 0x00},
	{0x0214, 0x01},
	{0x0215, 0x00},
	//HDR Setting
	{0x0230, 0x00},
	{0x0231, 0x00},
	{0x0233, 0x00},
	{0x0234, 0x00},
	{0x0235, 0x40},
	{0x0238, 0x00},
	{0x0239, 0x04},
	{0x023B, 0x00},
	{0x023C, 0x01},
	{0x33B0, 0x04},
	{0x33B1, 0x00},
	{0x33B3, 0x00},
	{0x33B4, 0x01},
	{0x3800, 0x00},
	//Mode Clear
	{0x3A43,	0x01},
};
static struct msm_camera_i2c_reg_conf imx134_recommend_settings[] = {
	//Basic Config
	{0x0101, 0x03},
	{0x0105, 0x01},
	{0x0110, 0x00},
	{0x0220, 0x01},
	{0x3302, 0x11},
	{0x3833, 0x20},
	{0x3893, 0x00},
	{0x3906, 0x08},
	{0x3907, 0x01},
	{0x391B, 0x01},
	{0x3C09, 0x01},
	{0x600A, 0x00},
	//Analog Setting(Sensor Optimizatin)
	{0x3008, 0xB0},
	{0x320A, 0x01},
	{0x320D, 0x10},
	{0x3216, 0x2E},
	{0x322C, 0x02},
	{0x3409, 0x0C},
	{0x340C, 0x2D},
	{0x3411, 0x39},
	{0x3414, 0x1E},
	{0x3427, 0x04},
	{0x3480, 0x1E},
	{0x3484, 0x1E},
	{0x3488, 0x1E},
	{0x348C, 0x1E},
	{0x3490, 0x1E},
	{0x3494, 0x1E},
	{0x3511, 0x8F},
	{0x3617, 0x2D},
	//Defect Correction Recommended Setting
	{0x380A, 0x00},
	{0x380B, 0x00},
	{0x4103, 0x00},
	//Color Artifact Recommended Setting
	{0x4243, 0x9A},
	{0x4330, 0x01},
	{0x4331, 0x90},
	{0x4332, 0x02},
	{0x4333, 0x58},
	{0x4334, 0x03},
	{0x4335, 0x20},
	{0x4336, 0x03},
	{0x4337, 0x84},
	{0x433C, 0x01},
	{0x4340, 0x02},
	{0x4341, 0x58},
	{0x4342, 0x03},
	{0x4343, 0x52},
	//Moir¨¦ reduction Parameter Setting
	{0x4364, 0x0B},
	{0x4368, 0x00},
	{0x4369, 0x0F},
	{0x436A, 0x03},
	{0x436B, 0xA8},
	{0x436C, 0x00},
	{0x436D, 0x00},
	{0x436E, 0x00},
	{0x436F, 0x06},
	//CNR parameter setting
	{0x4281, 0x21},
	{0x4282, 0x18},
	{0x4283, 0x04},
	{0x4284, 0x08},
	{0x4287, 0x7F},
	{0x4288, 0x08},
	{0x428B, 0x7F},
	{0x428C, 0x08},
	{0x428F, 0x7F},
	{0x4297, 0x00},
	{0x4298, 0x7E},
	{0x4299, 0x7E},
	{0x429A, 0x7E},
	{0x42A4, 0xFB},
	{0x42A5, 0x7E},
	{0x42A6, 0xDF},
	{0x42A7, 0xB7},
	{0x42AF, 0x03},
	//ARNR Parameter Setting
	{0x4207, 0x03},
	{0x4216, 0x08},
	{0x4217, 0x08},
	//DLC Parameter Setting
	{0x4218, 0x00},
	{0x421B, 0x20},
	{0x421F, 0x04},
	{0x4222, 0x02},
	{0x4223, 0x22},
	{0x422E, 0x54},
	{0x422F, 0xFB},
	{0x4230, 0xFF},
	{0x4231, 0xFE},
	{0x4232, 0xFF},
	{0x4235, 0x58},
	{0x4236, 0xF7},
	{0x4237, 0xFD},
	{0x4239, 0x4E},
	{0x423A, 0xFC},
	{0x423B, 0xFD},
	//HDR Setting
	{0x4300, 0x00},
	{0x4316, 0x12},
	{0x4317, 0x22},
	{0x4318, 0x00},
	{0x4319, 0x00},
	{0x431A, 0x00},
	{0x4324, 0x03},
	{0x4325, 0x20},
	{0x4326, 0x03},
	{0x4327, 0x84},
	{0x4328, 0x03},
	{0x4329, 0x20},
	{0x432A, 0x03},
	{0x432B, 0x20},
	{0x432C, 0x01},
	{0x432D, 0x01},
	{0x4338, 0x02},
	{0x4339, 0x00},
	{0x433A, 0x00},
	{0x433B, 0x02},
	{0x435A, 0x03},
	{0x435B, 0x84},
	{0x435E, 0x01},
	{0x435F, 0xFF},
	{0x4360, 0x01},
	{0x4361, 0xF4},
	{0x4362, 0x03},
	{0x4363, 0x84},
	{0x437B, 0x01},
	{0x4401, 0x3F},
	{0x4402, 0xFF},
	{0x4404, 0x13},
	{0x4405, 0x26},
	{0x4406, 0x07},
	{0x4408, 0x20},
	{0x4409, 0xE5},
	{0x440A, 0xFB},
	{0x440C, 0xF6},
	{0x440D, 0xEA},
	{0x440E, 0x20},
	{0x4410, 0x00},
	{0x4411, 0x00},
	{0x4412, 0x3F},
	{0x4413, 0xFF},
	{0x4414, 0x1F},
	{0x4415, 0xFF},
	{0x4416, 0x20},
	{0x4417, 0x00},
	{0x4418, 0x1F},
	{0x4419, 0xFF},
	{0x441A, 0x20},
	{0x441B, 0x00},
	{0x441D, 0x40},
	{0x441E, 0x1E},
	{0x441F, 0x38},
	{0x4420, 0x01},
	{0x4444, 0x00},
	{0x4445, 0x00},
	{0x4446, 0x1D},
	{0x4447, 0xF9},
	{0x4452, 0x00},
	{0x4453, 0xA0},
	{0x4454, 0x08},
	{0x4455, 0x00},
	{0x4456, 0x0F},
	{0x4457, 0xFF},
	{0x4458, 0x18},
	{0x4459, 0x18},
	{0x445A, 0x3F},
	{0x445B, 0x3A},
	{0x445C, 0x00},
	{0x445D, 0x28},
	{0x445E, 0x01},
	{0x445F, 0x90},
	{0x4460, 0x00},
	{0x4461, 0x60},
	{0x4462, 0x00},
	{0x4463, 0x00},
	{0x4464, 0x00},
	{0x4465, 0x00},
	{0x446C, 0x00},
	{0x446D, 0x00},
	{0x446E, 0x00},
	//LSC Setting
	{0x452A, 0x02},
	//White Balance Setting
	{0x0712, 0x01},
	{0x0713, 0x00},
	{0x0714, 0x01},
	{0x0715, 0x00},
	{0x0716, 0x01},
	{0x0717, 0x00},
	{0x0718, 0x01},
	{0x0719, 0x00},
	//Shading setting
	{0x4500, 0x1F},
};

static struct v4l2_subdev_info imx134_subdev_info[] = {
	{
	.code   = V4L2_MBUS_FMT_SBGGR10_1X10,
	.colorspace = V4L2_COLORSPACE_JPEG,
	.fmt    = 1,
	.order    = 0,
	},
	/* more can be supported, to be added later */
};
static struct msm_camera_i2c_conf_array imx134_init_conf[] = {
	{&imx134_recommend_settings[0],
	ARRAY_SIZE(imx134_recommend_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};
static struct msm_camera_i2c_conf_array imx134_confs[] = {
	{&imx134_snap_settings[0],
	ARRAY_SIZE(imx134_snap_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&imx134_prev_settings[0],
	ARRAY_SIZE(imx134_prev_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
	{&imx134_1080p_settings[0],
	ARRAY_SIZE(imx134_1080p_settings), 0, MSM_CAMERA_I2C_BYTE_DATA},
};

static struct msm_sensor_output_info_t imx134_dimensions[] = {
	{/*For snapshot*/
		.x_output = 0xCD0,
		.y_output = 0x9A0,
		.line_length_pclk = 0xE10,
		.frame_length_lines = 0xB90,
		.vt_pixel_clk =160000000,
		.op_pixel_clk = 160000000,
		.binning_factor = 0,
	},
	{/*For preview*/
		.x_output = 0x668,
		.y_output = 0x4D0,
		.line_length_pclk = 0xE10,
		.frame_length_lines = 0x5C8,
		.vt_pixel_clk = 160000000,
		.op_pixel_clk = 160000000,
		.binning_factor = 1,
	},
	{/*For 1080P */
		.x_output = 0x780,
		.y_output = 0x440,
		.line_length_pclk = 0xE10,
		.frame_length_lines = 0x9B6,
		.vt_pixel_clk = 266700000,
		.op_pixel_clk = 160000000,
		.binning_factor = 1,
	},
};

static struct msm_sensor_output_reg_addr_t imx134_reg_addr = {
	.x_output = 0x034C,
	.y_output = 0x034E,
	.line_length_pclk = 0x0342,
	.frame_length_lines = 0x0340,
};

static struct msm_sensor_exp_gain_info_t imx134_exp_gain_info = {
	.coarse_int_time_addr = 0x0202,
	.global_gain_addr = 0x0205,
	.vert_offset = 4,
};

////////////////////////otp start//////////////////////////////
#define  MID        0x75
#define  R_OVER_G_BASE      583  //need to modify according to the godlen sample
#define  B_OVER_G_BASE      604  //need to modify according to the godlen sample

uint16_t data_checksum = 0;
uint16_t module_id_checksum = 0;
uint16_t global_checksum = 0; 
uint8_t module_id[5]={0}; 
uint8_t otp_data[286]={0};

int32_t imx134_write_sensor(uint16_t waddr, uint16_t wdata)
{
	int32_t rc = 0;	
	struct msm_sensor_ctrl_t *s_ctrl = &imx134_s_ctrl;
	
	rc = msm_camera_i2c_write(
			s_ctrl->sensor_i2c_client,
			waddr, wdata,
			MSM_CAMERA_I2C_BYTE_DATA);
	if( rc< 0) {
		  CDBG("%s: write sensor error!\n", __func__);  
	}
	return rc;
}

int32_t imx134_read_eeprom(uint16_t  slave_id, uint8_t rxaddr, uint8_t *rxdata,uint16_t addr_type)
{
	int32_t rc = 0;
	struct i2c_client *client = imx134_s_ctrl.sensor_i2c_client->client;	
	uint16_t saddr = slave_id >> 1;
	struct i2c_msg msgs[] = {
		{
			.addr  = saddr,
			.flags = 0,
			.len   = addr_type,
			.buf   = &rxaddr,
		},
		{
			.addr  = saddr,
			.flags = I2C_M_RD,
			.len   = MSM_CAMERA_I2C_BYTE_DATA,
			.buf   = rxdata,
		},
	};
	
	rc = i2c_transfer(client->adapter, msgs, 2);
	if (rc < 0)
		S_I2C_DBG("imx134_read_eeprom failed 0x%x\n", saddr);
	return rc;
}

int32_t imx134_read_eeprom_seq(uint8_t slave_id, uint8_t address, uint8_t* data, int datasize,uint16_t addr_type) 
{ 
	uint8_t val; 
	int32_t i = 0; 
	int32_t rc = 0;
	
	for(i = 0; i < datasize; i++) 
	{ 
		if (i < (256-address)) 
		{ 
			  rc = imx134_read_eeprom(slave_id, address + i, &val, addr_type); 
			  *(data + i) = val; 
		} 
		else 
		{ 
			  rc = imx134_read_eeprom(slave_id + 2, i - (256 - address), &val, addr_type); 
			  *(data + i) = val; 
		} 
	}

	return rc; 
} 

int32_t  imx134_close_lsc(void)
{
	imx134_write_sensor(0x4500, 0x0); 
	imx134_write_sensor(0x0700, 0x0); 
	imx134_write_sensor(0x3a63, 0x0);
	return 0;

}

int32_t  imx134_open_lsc(void)
{
	imx134_write_sensor(0x4500, 0x1f); 
	imx134_write_sensor(0x0700, 0x01); 
	imx134_write_sensor(0x3a63, 0x01); 
	return 0;
}

int32_t  imx134_write_awb_to_sensor(uint16_t* awb_data) 
{

	uint16_t r_over_g_base     =   R_OVER_G_BASE;
	uint16_t b_over_g_base    =   B_OVER_G_BASE;
	uint16_t  r_over_g = awb_data[0];
	uint16_t b_over_g  = awb_data[1];
	 
	uint16_t r_test=0, b_test=0, g_test=0; 
	uint16_t  r_test_h3,r_test_l8,b_test_h3,b_test_l8,g_test_h3,g_test_l8; 
 
	int g_test_r, g_test_b; 
	if(b_over_g < b_over_g_base)  
	{ 
		if (r_over_g < r_over_g_base) 
		{ 
			g_test = 0x100; 
			b_test = 0x100 * b_over_g_base / b_over_g; 
			r_test = 0x100 * r_over_g_base / r_over_g;  
		} 
		else  
		{ 
			r_test = 0x100; 
			g_test = 0x100 * r_over_g / r_over_g_base; 
			b_test = g_test * b_over_g_base / b_over_g; 
		} 
	} 
	else  
	{   
		if (r_over_g < r_over_g_base) 
		{ 
			b_test = 0x100; 
			g_test = 0x100 * b_over_g / b_over_g_base; 
			r_test = g_test * r_over_g_base / r_over_g; 
		} 
		else 
		{ 
			g_test_b = 0x100 * b_over_g / b_over_g_base; 
			g_test_r = 0x100 * r_over_g / r_over_g_base; 
			if(g_test_b > g_test_r ) 
			{ 
				b_test = 0x100; 
				g_test = g_test_b; 
				r_test = g_test * r_over_g_base / r_over_g; 
			} 
			else 
			{ 
				r_test = 0x100; 
				g_test = g_test_r; 
				b_test = g_test * b_over_g_base / b_over_g; 
			} 
		} 
	} 
	 
	if (r_test<0x100) 
	{ 
		r_test = 0x100; 
	} 
	if (g_test<0x100) 
	{ 
		g_test = 0x100; 
	} 
	if (b_test<0x100) 
	{ 
		b_test = 0x100; 
	} 
  
	r_test_h3 =( r_test>>8)&0x0F; 
	r_test_l8 = r_test &0xFF; 
	b_test_h3 = (b_test>>8)&0x0F; 
	b_test_l8 = b_test &0xFF; 
	g_test_h3 = (g_test>>8)&0x0F; 
	g_test_l8 = g_test &0xFF; 
    
	imx134_write_sensor(0x020F,g_test_l8);   
	imx134_write_sensor(0x0211,r_test_l8);     
	imx134_write_sensor(0x0213,b_test_l8); 
	imx134_write_sensor(0x0215,g_test_l8);    
	return 0;
} 

 int32_t valid_otp(uint8_t *temp)
 {
 	int32_t rc = 0;
 	if(temp[0]==0xff&&temp[1]==0xff&&temp[2]==0xff) 
	{ 
		printk("%s: module has no otp data!\n", __func__);  
		return -EINVAL; 
	} 
	
	if(temp[3] != MID) 
	{  
		  printk("%s: module id error!\n", __func__);  
		  return -EINVAL; 
	}
	return rc;
 
 }
 
int32_t imx134_read_module_id_from_eeprom(void)
{
	int32_t rc = 0;
	uint8_t start_addr = 0; 
	uint8_t sum = 5;
	int i = 0; 
	uint8_t slave_id = 0xA4; 
	/*read otp of  module id*/
	rc = imx134_read_eeprom_seq(slave_id,start_addr,module_id,sum, MSM_CAMERA_I2C_BYTE_ADDR);
	
	if(rc < 0) {  
		 printk("%s: i2c read error!\n", __func__); 
		 return -EINVAL; 
	} 
	
	if(valid_otp(module_id)){
		return -EINVAL; 
	}
	
	if( 0x1 == (module_id[4] >> 4)){		
		printk("imx134 is sunny module \n");	
		imx134_s_ctrl.sensor_name = "23060131FA-IMX-S";
		goto END; 
	}
 
	if( 0x3 == (module_id[4] >> 4)){		
		printk("imx134 is liteon module \n");	
		imx134_s_ctrl.sensor_name = "23060131FA-IMX-L";
	}
END:
	for(i = 0; i < sum; i++) 
	{ 
		module_id_checksum += module_id[i]; 
	} 
	return rc;

}
  	
 int32_t imx134_read_from_eeprom(void)
 { 
	uint8_t start_addr = 0x05; 
	int32_t rc = 0;
	uint16_t sum = 286;
	uint8_t slave_id = 0xA4; 
	int i = 0; 
	
	rc = imx134_read_eeprom_seq(slave_id,start_addr,otp_data,sum, MSM_CAMERA_I2C_BYTE_ADDR);	
	if(rc < 0) {  
		 printk("%s: i2c read error!\n", __func__); 
		 return -EINVAL; 
	}
	
	for(i = 0; i < sum; i++) 
	{ 
		data_checksum += otp_data[i]; 
	} 	
	return rc; 
	
 } 

 int32_t imx134_write_otp_to_sensor(void) 
{ 
	int i; 
	int32_t rc = 0;
	uint8_t* lsc_data = NULL;
	uint16_t awb_data[3] = {0};
	awb_data[0] = (otp_data[0]<<8)|otp_data[1]; //r
	awb_data[1] = (otp_data[2]<<8)|otp_data[3]; //b
	awb_data[2] = (otp_data[4]<<8)|otp_data[5]; //g
	
	printk("%s: r= 0x%04X\n", __func__,awb_data[0]);
	printk("%s: b= 0x%04X\n", __func__,awb_data[1]); 
	printk("%s: g= 0x%04X\n", __func__,awb_data[2]); 
	imx134_write_awb_to_sensor(&awb_data[0]);
	
	CDBG("%s: write lsc start\n", __func__); 
	lsc_data =otp_data+6;
	for(i = 0; i < 280; i++) 
	{ 
		imx134_write_sensor(0x4800+i, lsc_data[i]); 
	} 
	imx134_open_lsc();
	CDBG("%s: write lsc end\n", __func__); 
	
	return rc; 
} 

int32_t checksum(void)
{
	int32_t b_success = true; 
	uint8_t val1,val2,val3,val4,val5,val6; 

	imx134_read_eeprom(0xa6, 0x40, &val1,MSM_CAMERA_I2C_BYTE_ADDR); 
	imx134_read_eeprom(0xa6, 0x41, &val2,MSM_CAMERA_I2C_BYTE_ADDR); 
	imx134_read_eeprom(0xa6, 0x42, &val3,MSM_CAMERA_I2C_BYTE_ADDR); 
	imx134_read_eeprom(0xa6, 0x43, &val4,MSM_CAMERA_I2C_BYTE_ADDR); 
	
	if (val1 == 0xff || val3 == 0xff) 
	{ 
		printk("%s: no AF data, checksum error!\n", __func__);
		b_success = false; 
	} 
	else 
	{   
		if (b_success) 
		{ 
			global_checksum = data_checksum + module_id_checksum + val1 + val2 + val3 + val4 ; 

			imx134_read_eeprom(0xa6, 0x44, &val5,MSM_CAMERA_I2C_BYTE_ADDR); 	   
			if ((global_checksum % 255) != val5)  
			{ 
				 printk("%s: checksum error!\n", __func__);
				 b_success = false; 
			} 
			
			global_checksum = 0;
			data_checksum = 0;
			module_id_checksum = 0;
			imx134_read_eeprom(0xa6, 0x45, &val6,MSM_CAMERA_I2C_BYTE_ADDR); 
			if (val6 != 0xa5)  
			{ 
				 printk("%s: checksum for new module error!\n", __func__);
				 b_success = false; 
			} 
		} 
	}
	return  b_success; 
}
  
////////////////////////otp end /////////////////////////////

int32_t imx134_match_module(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	
	rc = imx134_read_module_id_from_eeprom();
	if (rc < 0){ 
		return rc; 
	}
	
	imx134_close_lsc();
	rc = imx134_read_from_eeprom();
	if (rc < 0){ 
		return rc; 
	}
	
	if(false == checksum()){
		return -EINVAL; 
	}
	rc = imx134_write_otp_to_sensor();
	return rc;
}

int32_t imx134_match_id(struct msm_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	uint16_t chipid_Low = 0,chipid_High = 0,chipid = 0;
    	
    	rc = msm_camera_i2c_read(
    			s_ctrl->sensor_i2c_client,
    			0x0017, &chipid_Low,
    			MSM_CAMERA_I2C_BYTE_DATA);
	rc = msm_camera_i2c_read(
    			s_ctrl->sensor_i2c_client,
    			0x0016, &chipid_High,
    			MSM_CAMERA_I2C_BYTE_DATA);	
    	if (rc < 0) {
    		pr_err("%s: %s: read id failed\n", __func__,
    			s_ctrl->sensordata->sensor_name);
    		return rc;
    	}

	chipid =  (chipid_High<<8)|chipid_Low;
    	CDBG("msm_sensor id: %x, high=%x, low=%x\n", chipid, chipid_High, chipid_Low);
    	if (chipid != 0x0134) {
    		pr_err("%s: imx134_match_id chip id does not match!\n", __func__);
    		return -ENODEV;
    	}
	
	return rc;
}
	
int32_t imx134_sensor_setting(struct msm_sensor_ctrl_t *s_ctrl,
			int update_type, int res)
{
	int32_t rc = 0;
	s_ctrl->func_tbl->sensor_stop_stream(s_ctrl);
	if (update_type == MSM_SENSOR_REG_INIT) {
		/*uesd to write special initialization arrays of some sensors*/
		if(s_ctrl->func_tbl->sensor_write_init_settings)
		{
			s_ctrl->func_tbl->sensor_write_init_settings(s_ctrl);
		}
		else
		{
			msm_sensor_write_init_settings(s_ctrl);
		}

	} else if (update_type == MSM_SENSOR_UPDATE_PERIODIC) {
		msm_sensor_write_res_settings(s_ctrl, res);
		msleep(20);
		v4l2_subdev_notify(&s_ctrl->sensor_v4l2_subdev,
			NOTIFY_PCLK_CHANGE, &s_ctrl->msm_sensor_reg->
			output_settings[res].op_pixel_clk);
		
		msleep(10);
	}
	s_ctrl->func_tbl->sensor_start_stream(s_ctrl);
	return rc;
}
int32_t imx134_write_exp_gain(struct msm_sensor_ctrl_t *s_ctrl,
		uint16_t gain, uint32_t line)
{
	uint32_t fl_lines;
	uint8_t offset;
	fl_lines = s_ctrl->curr_frame_length_lines;
	fl_lines = (fl_lines * s_ctrl->fps_divider) / Q10;
	offset = s_ctrl->sensor_exp_gain_info->vert_offset;
	if (line > (fl_lines - offset))
		fl_lines = line + offset;

	CDBG("%s: here, res=%d , gain =%x ,line=%d \n",__func__, s_ctrl->curr_res, gain, line);
	s_ctrl->func_tbl->sensor_group_hold_on(s_ctrl);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_output_reg_addr->frame_length_lines, fl_lines,
		MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->coarse_int_time_addr, line,
		MSM_CAMERA_I2C_WORD_DATA);
	msm_camera_i2c_write(s_ctrl->sensor_i2c_client,
		s_ctrl->sensor_exp_gain_info->global_gain_addr, gain,
		MSM_CAMERA_I2C_BYTE_DATA);
	s_ctrl->func_tbl->sensor_group_hold_off(s_ctrl);
	return 0;
}
static const struct i2c_device_id imx134_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t)&imx134_s_ctrl},
	{ }
};

static struct i2c_driver imx134_i2c_driver = {
	.id_table = imx134_i2c_id,
	.probe  = msm_sensor_i2c_probe,
	.driver = {
		.name = SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client imx134_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static int __init msm_sensor_init_module(void)
{
	return i2c_add_driver(&imx134_i2c_driver);
}

static struct v4l2_subdev_core_ops imx134_subdev_core_ops = {
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops imx134_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops imx134_subdev_ops = {
	.core = &imx134_subdev_core_ops,
	.video  = &imx134_subdev_video_ops,
};

static struct msm_sensor_fn_t imx134_func_tbl = {
	.sensor_start_stream = msm_sensor_start_stream,
	.sensor_stop_stream = msm_sensor_stop_stream,
	.sensor_group_hold_on = msm_sensor_group_hold_on,
	.sensor_group_hold_off = msm_sensor_group_hold_off,
	.sensor_set_fps = msm_sensor_set_fps,
	.sensor_write_exp_gain = imx134_write_exp_gain,
	.sensor_write_snapshot_exp_gain = imx134_write_exp_gain,
	.sensor_setting =imx134_sensor_setting,
	.sensor_csi_setting = msm_sensor_setting1,
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = msm_sensor_config,
	.sensor_power_up = msm_sensor_power_up,
	.sensor_power_down = msm_sensor_power_down,
	.sensor_match_id = imx134_match_id,
	.sensor_get_csi_params = msm_sensor_get_csi_params,
	.sensor_match_module = imx134_match_module,
};

static struct msm_sensor_reg_t imx134_regs = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.start_stream_conf = imx134_start_settings,
	.start_stream_conf_size = ARRAY_SIZE(imx134_start_settings),
	.stop_stream_conf = imx134_stop_settings,
	.stop_stream_conf_size = ARRAY_SIZE(imx134_stop_settings),
	.group_hold_on_conf = imx134_groupon_settings,
	.group_hold_on_conf_size = ARRAY_SIZE(imx134_groupon_settings),
	.group_hold_off_conf = imx134_groupoff_settings,
	.group_hold_off_conf_size = ARRAY_SIZE(imx134_groupoff_settings),
	.init_settings = &imx134_init_conf[0],
	.init_size = ARRAY_SIZE(imx134_init_conf),
	.mode_settings = &imx134_confs[0],
	.output_settings = &imx134_dimensions[0],
	.num_conf = ARRAY_SIZE(imx134_confs),
};

static struct msm_sensor_ctrl_t imx134_s_ctrl = {
	.msm_sensor_reg = &imx134_regs,
	.sensor_i2c_client = &imx134_sensor_i2c_client,
	.sensor_i2c_addr = 0x20,
	.sensor_output_reg_addr = &imx134_reg_addr,
	.sensor_exp_gain_info = &imx134_exp_gain_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
	.msm_sensor_mutex = &imx134_mut,
	.sensor_i2c_driver = &imx134_i2c_driver,
	.sensor_v4l2_subdev_info = imx134_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(imx134_subdev_info),
	.sensor_v4l2_subdev_ops = &imx134_subdev_ops,
	.func_tbl = &imx134_func_tbl,
	.clk_rate = MSM_SENSOR_MCLK_24HZ,
	.sensor_name = "23060131FA-IMX-S",
};

module_init(msm_sensor_init_module);
MODULE_DESCRIPTION("Sony 8MP Bayer sensor driver");
MODULE_LICENSE("GPL v2");
