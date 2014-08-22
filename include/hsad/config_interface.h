#ifndef CONFIG_INTERFACE_H
#define CONFIG_INTERFACE_H
#include <linux/types.h>
#include <linux/fs.h>

#define MIC_MASK 0x0f
#define HANDSFREE_MIC_MASK 0xf0000
#define BTSCO_NREC_ADAPT_MASK 0xf0000000
#define SPK_NUM_MASK 0xf00000
typedef enum
{
    HANDSFREE_MIC_MASTER = 0x10000,
    HANDSFREE_MIC_SECONDARY = 0x20000,
}handsfree_mic_type;

typedef enum
{
    AUDIO_PROPERTY_INVALID = 0x0,
    SINGLE_MIC = 0x1,
    DUAL_MIC = 0x2,
    MIC_NONE = 0xf,
    AUDIO_TYPE_MAX = 0xffff
}audio_mic_type;

typedef enum
{
    BTSCO_NREC_ADAPT_OFF = 0x10000000,
    BTSCO_NREC_ADAPT_ON = 0x20000000,
}btsco_nrec_adapt_type;
typedef enum
{
    SINGLE_SPEAKER        = 0X100000,
    DUAL_SPEAKER_MONO     = 0x200000,
	DUAL_SPEAKER_STEREO   = 0X300000
}audio_spk_num;

typedef enum
{
    AUDIO_ENHANCE_INVALID = 0x0,
    AUDIO_ENHANCE_DTS = 0x1,
    AUDIO_ENHANCE_SRS = 0x2,
    AUDIO_ENHANCE_NONE = 0xf,
    AUDIO_ENHANCE_MAX = 0xffff
}audio_enhance_type;

typedef enum
{
    WIFI_TYPE_UNKNOWN = 0x0,
    WIFI_BROADCOM_4330 = 0x01, 		
    WIFI_BROADCOM_4330X = 0x02,		
    WIFI_QUALCOMM_WCN3660 = 0x03,
    WIFI_TYPE_MAX = 0xffff,	
}hw_wifi_device_type;

typedef enum
{
    CLOCK_TYPE_UNKNOWN = 0x0,
    CLOCK_19P2M_INTERNAL = 0x01,
	CLOCK_48M_EXTERNAL = 0x02, 
    CLOCK_TYPE_MAX = 0xffff,	
}hw_wifi_clock_type;

typedef enum
{
   RAISE_TRIGGER = 0, 
   FALL_TRIGGER,
   DOUBLE_EDGE_TRIGGER = 0xF,	
}hw_sd_trigger_type;

typedef enum
{
    FM_NVALID = 0x0,
    FM_BROADCOM ,
    FM_QUALCOMM ,
    FM_TYPE_MAX = 0xffff
}audio_fm_type;
typedef enum
{
    BT_FM_UNKNOWN_DEVICE = 0x0,
    BT_FM_BROADCOM_BCM4330 = 0x01,
    BT_FM_QUALCOMM_WCN3660 = 0x02,
    BT_FM_TYPE_MAX = 0xffff
}bt_fm_device_type;

typedef enum
{
    FM_SINR_5 = 5,
    FM_SINR_6 = 6,
    FM_SINR_7 = 7,
    FM_SINR_8 = 8,
    FM_SINR_MAX = 0xff
}fm_sinr_value;
/* FM RSSI Threshold used for BCM4330 */
typedef enum
{
    FM_RSSI_97 = 97,
    FM_RSSI_98 = 98,
	FM_RSSI_99 = 99,
    FM_RSSI_100 = 100,
    FM_RSSI_101 = 101,
    FM_RSSI_102 = 102,
	FM_RSSI_103 = 103,
    FM_RSSI_104 = 104,
	FM_RSSI_105 = 105,
    FM_RSSI_106 = 106,
    FM_RSSI_107 = 107,
    FM_RSSI_108 = 108,
    FM_RSSI_MAX = 0xFF
}fm_rssi_value;
#define MMI_KEY_UP      false
#define MMI_KEY_DOWN    true
#ifdef CONFIG_HUAWEI_KERNEL
#define IC_PM_ON   1
#define IC_PM_OFF  0
#define IC_PM_VDD       2850000
#define IO_PM_VOLTAGE   1800000

#define MAX_TP_RESOLUTION_LEN 10
#define MAX_VIRTUALKEYS_MAP_LEN 30
#define MAX_PRODUCT_LEN 10
#define MAX_TP_TYPE 10

/***********RESOLUTION TYPE***********/
#define WVGA "WVGA"
#define QHD "QHD"
#define HD "HD"
/***********PRODUCT SERIES***********/
#define G510 "G510"
#define Y300 "Y300"
#define Y301 "Y301"
#define G526L "G526L"
#define G701A1 "G701A1"
#define G740_L00 "G740_L00"

/***********TP TYPE***********/
#define COB "COB"
#define COF "COF"

/***********VIRTUALKEY MAP************/
#define BACK_HOME_MENU "BACK_HOME_MENU"
#define MENU_HOME_BACK "MENU_HOME_BACK"

extern int get_tp_point_supported(void);
extern void get_tp_resolution(char *tp_resolution,int count);
extern void get_virtualkeys_map(char *virtualkeys_map,int count);
#endif

/*confir use cob code and use manual update fw */
typedef enum
{
    TP_COF = 0x0,
    TP_COB = 0x1,
    TP_MAX = 0xF,
}tp_type;

typedef enum
{
    NOT_NEED_UPDATE_FW = 0x0,
    NEED_UPDATE_FW = 0x1,
    UPDATE_MAX = 0xF,
}tp_update_type;

typedef enum
{
    PMIC_GPIO_INPUT = 0x0,
    PMIC_GPIO_OUTPUT = 0x1,
    PMIC_GPIO_DEFAULT = 0xF,
}pmic_gpio_type;

extern void get_product_family(char *product,int count);
extern void get_speaker_type_audio(char *spk_type,int count);
extern audio_spk_num get_audio_num_spk(void);
extern tp_type get_touch_type(void);
extern tp_update_type is_need_update_fw(void);
extern void get_tp_fw_file_name(char *fw_file_name,int count);

extern char *get_touch_info(void);
extern char *get_synaptics_touch_info(void);
#ifdef CONFIG_HUAWEI_CYPRESS_TOUCHSCREEN
extern char * cyttsp_get_touch_info(void);
#endif

#ifdef CONFIG_TOUCHSCREEN_CYPRESS_CYTTSP4
extern char * get_cyttsp4_touch_info(void);
#endif


typedef enum
{
   LCD_IS_MIPI_CMD = 0, 
   LCD_IS_MIPI_VIDEO,
   LCD_IS_MDDI_TYPE1,
   LCD_IS_MDDI_TYPE2,
   LCD_IS_RGB, 
   LCD_IS_UNKNOW = 0xF,
}hw_lcd_interface_type;
typedef enum
{
	LCD_HW_ID0 = 0,    
	LCD_HW_ID1,
	LCD_HW_ID2,
	LCD_HW_ID3,
	LCD_HW_ID4,
	LCD_HW_ID5,
	LCD_HW_ID6,
	LCD_HW_ID7,
	LCD_HW_ID8,
	LCD_HW_ID9,
	LCD_HW_IDA,
	LCD_HW_ID_MAX = 0xFF,
} hw_lcd_id_index;
typedef enum
{
   LCD_PANEL_ALIGN_LSB,
   LCD_PANEL_ALIGN_MSB,
   LCD_PANEL_ALIGN_INVALID = 0xFF
}lcd_align_type;
typedef enum
{
	MIPI_NT35560_TOSHIBA_FWVGA,
	MIPI_RSP61408_CHIMEI_WVGA,
	MIPI_RSP61408_BYD_WVGA,
	MIPI_HX8369A_TIANMA_WVGA,
	MIPI_RSP61408_TRULY_WVGA,
	MIPI_NT35510_BOE_WVGA, 
	LCD_MAX_NUM,
	LCD_NONE =0xFF
}lcd_panel_type;
/********************************************
 *CTRL_BL_BY_LCD : control LCD backlight by lcd
 *CTRL_BL_BY_MSM : control LCD backlight by msm
 ********************************************/
typedef enum
{
	CTRL_BL_BY_LCD = 0,
	CTRL_BL_BY_MSM ,
	CTRL_BL_BY_UNKNOW = 0xF,
}hw_lcd_ctrl_bl_type;
/* Every buffer/layer/image width/length should be 32 pixel aligned. 
 * It is due to Qualcomm's GPU architecture. 
 * Also if using video decoder, some hw/sw decoder need 32 pixel aligned too. 
 * Now all software written based on this precondition. 
 */
typedef enum
{
	LCD_IS_QVGA     = 256 * 320,  /*240 * 320*/
	LCD_IS_HVGA     = 320 * 480,  /*320 * 480*/
	LCD_IS_WVGA     = 480 * 800,  /*480 * 800*/
	LCD_IS_FWVGA    = 480 * 864,  /*480 * 854*/
	LCD_IS_QHD      = 544 * 960,  /*540 * 960*/
	LCD_IS_HD       = 736 * 1280, /*720 * 1280*/
	LCD_IS_FHD      = 1088* 1920, /*1080* 1920*/
	LCD_IS_DEFAULT  = LCD_IS_FHD,
}lcd_resolution_type;
typedef enum
{
	CAMERA_NO_FLIP_OR_MIRROR = 0,
	CAMERA_FLIP,
	CAMERA_MIRROR,
	CAMERA_FLIP_AND_MIRROR,
	CAMERA_MOUNT_MAX = 0xFF,
}camera_mount_type;

typedef enum
{
	CAMERA_IS_MAIN = 0,
	CAMERA_IS_FRONT,
	CAMERA_ID_MAX = 0xFF,
}camera_id_type;

/*8930 sensor gpio Pin*/
#define MSM_8930_COMPASS_INT_PIN  70
#define MSM_8930_COMPASS_RST_PIN  145
#define MSM_8930_APS_INT_PIN 51
#define MSM_8930_GS_INT_1 46
#define MSM_8930_GS_INT_2 67
#define GS_SUSPEND  0
#define GS_RESUME   1
/*add new g-sensor*/
typedef enum
{
	GS_ADIX345 	= 0x01,
	GS_ST35DE	= 0x02,
	GS_ST303DLH = 0X03,
	GS_MMA8452  = 0x04,
	GS_BMA250   = 0x05,
	GS_STLIS3XH	= 0x06,
	GS_ADI346   = 0x07,
	GS_KXTIK1004= 0x08,
}hw_gs_type;
typedef enum
{
	COMPASS_TOP_GS_TOP 			=0,
	COMPASS_TOP_GS_BOTTOM 		=1,
	COMPASS_BOTTOM_GS_TOP 		=2,
	COMPASS_BOTTOM_GS_BOTTOM	=3,
	COMPASS_NONE_GS_BOTTOM		=4,
	COMPASS_NONE_GS_TOP			=5,
}compass_gs_position_type;
typedef enum
{
	NONE_SENSOR	= 0,
	G_SENSOR 	= 0x01,
	L_SENSOR	= 0x02,
	P_SENSOR 	= 0X04,
	M_SENSOR	= 0x08,
	GY_SENSOR   = 0x10,
}sensors_list_type;
typedef enum
{ 
    NO_SUPPORT = 0,
	SUPPORT ,
}nfc_support_type;

typedef enum
{ 
    NONE = 0,
    NXP,
	BCM,
}nfc_chipset_type;

#define NFC_NXP_DEV "/dev/pn544"
#define NFC_BCM_DEV "/dev/bcm2079x"
/*add compass platform data and the func of power_switch*/
struct gs_platform_data {
	int (*adapt_fn)(void);	/* fucntion is suported in some product */
	int slave_addr;     /*I2C slave address*/
	int dev_id;         /*who am I*/
	int *init_flag;     /*Init*/
	compass_gs_position_type (*get_compass_gs_position)(void);
	int (*gs_power)(int on);
};

struct compass_platform_data {
	int (*compass_power)(int on);
};

struct gyro_platform_data {
	u8 fs_range;
	u8 axis_map_x;     /*x map read data[axis_map_x] from i2c*/
	u8 axis_map_y;
	u8 axis_map_z;
	
	u8 negate_x;       /*negative x,y or z*/
	u8 negate_y;
	u8 negate_z;
	int dev_id;        /*who am I*/
	int slave_addr;
	int (*gyro_power)(int on);
};
compass_gs_position_type  get_compass_gs_position(void);

bool st303_gs_is_supported(void);
void set_st303_gs_support(bool status);
#ifdef CONFIG_HUAWEI_FEATURE_PROXIMITY_EVERLIGHT_APS_9900
struct aps9900_hw_platform_data {
    int (*aps9900_power)(int on);
    int (*aps9900_gpio_config_interrupt)(void);
};
#endif
/*add aps12d platform_data struct*/
#ifdef CONFIG_HUAWEI_FEATURE_PROXIMITY_EVERLIGHT_APS_12D
struct aps12d_hw_platform_data {
    int (*aps12d_power)(int on);
};
#endif

extern void get_audio_property(char *mic_type_value);
extern int get_product_type(char *pname, int length);
extern int get_lcd_name(char *pname,int length);
extern int get_battery_name(int batt_r_id,char *pname,int length);
extern int get_charge_standard(char *pname, int length);
extern bool get_mpp5_charge_limit(void);
extern int get_cutoff_v_mv(void);
extern int get_audio_fm_type(void);
extern int get_hw_sd_trigger_type(void);
extern int get_hw_sim_card_num(void);
extern void get_hw_wifi_pubfile_id(char *pubfile_id, int length);
extern int get_hw_wifi_clock_type(void);
extern int get_hw_wifi_device_type(void);
extern int get_bt_fm_device_type(void);
extern int get_bt_fm_fw_ver(char *bt_fm_fw_ver,unsigned int length);
extern int get_wlan_fw_ver(char *wlan_fw_ver,unsigned int length);
int get_bt_product_id(char *product_id,unsigned int length);
extern int get_fm_sinr(void);
extern int get_fm_rssi(void);
extern void set_sensors_list(int sensor);
extern char * get_sensors_list_name(void);
extern char * get_compass_gs_position_name(void);
extern audio_mic_type get_audio_mic_type(void);
extern handsfree_mic_type get_audio_handsfree_mic_type(void);
extern btsco_nrec_adapt_type get_btsco_nrec_adapt_status(void);
extern int get_audio_enhance_type(void);
extern int lcd_detect_panel(const char *pstring);
#ifdef CONFIG_HW_ESD_DETECT
extern bool can_use_sw_esd(void);
extern bool g_Can_Use_SW_Esd;
#endif
extern int get_camera_mount_type(int);
extern int get_camera_sensor_name( char*, int);
//extern int acdb_type_read(struct file *filp,char __user *buffer,size_t count, loff_t *ppos);  
//extern int acdb_debugfs(void);  

#ifdef CONFIG_HUAWEI_GPIO_UNITE
extern struct gpio_config_type *get_gpio_config_table(void);
extern int get_gpio_num_by_name(char *name);
extern struct pm_gpio_cfg_t *get_pm_gpio_config_table(void);
extern int get_pm_gpio_num_by_name(char *name);
#endif

#ifdef CONFIG_HW_POWER_TREE
extern struct hw_config_power_tree *get_power_config_table(void);
#endif
extern int get_I2C_devices(char *ptype);
extern int get_proximity_wave(void);
extern int get_proximity_window(void);
extern int get_proximity_sunlight(void);
extern void get_light_value(int *p);
extern int get_vibrator_voltage(void);
extern lcd_resolution_type  get_lcd_resolution(void);
extern bool qwerty_is_supported(void);
#endif
extern int get_nfc_support(void);
extern int get_nfc_chipset_type(void);

extern pmic_gpio_type get_pmic_gpio_type(int gpio);

#define KEYNAME_PTT "QCHAT_KEY"
#define KEYNAME_SUPPORT "support"
extern int get_key_custom_num(const char *name);
extern int get_wled_max_current(void);
