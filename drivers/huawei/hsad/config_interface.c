#include <linux/types.h>
#include <linux/gpio.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>

#include <hsad/configdata.h>   
#include "hwconfig_enum.h"
#include "config_mgr.h"
#include <hsad/config_general_struct.h>  
#include <hsad/config_interface.h>

#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/debugfs.h>


#define MAX_KEY_LENGTH 48

/*Add for 8930 audioparameters self adapt*/  
#define MAX_ACDB_PRO_NAME_LEN  20  

#define DEFAULT_PANEL_NAME	"mipi_rsp61408_chimei_wvga"

#define MAX_LCD_PANEL_NAME_LEN  40

/*This file is interfacer between modules/devices and config data manager.*/

void get_audio_property(char *audio_property_value)
{
    unsigned int property_value = 0;
    
    if(NULL == audio_property_value)
    {
        return;
    }
    
    property_value |= (get_audio_mic_type() & MIC_MASK);
    property_value |= (get_audio_handsfree_mic_type() & HANDSFREE_MIC_MASK);
    property_value |= (get_btsco_nrec_adapt_status() & BTSCO_NREC_ADAPT_MASK);
    property_value |= (get_audio_num_spk() & SPK_NUM_MASK);

    sprintf(audio_property_value, "%8x", property_value);
}

audio_mic_type get_audio_mic_type(void)
{
	  int ret = false;
	  audio_mic_type  mic_type = AUDIO_PROPERTY_INVALID;
	  ret = get_hw_config_enum("audio/mic_type", (char*)&mic_type, sizeof(audio_mic_type), NULL);
	  if(false == ret)
	  {
	  	printk(KERN_ERR "get audio mic type fail!\n");
		return -1;
	  }
	  return mic_type;
  
}

handsfree_mic_type get_audio_handsfree_mic_type(void)
{
    int ret = false;
    handsfree_mic_type  type = HANDSFREE_MIC_MASTER;
    
    ret = get_hw_config_enum("audio/handsfree_mic_type", (char*)&type, sizeof(type), NULL);
    if(false == ret)
    {
        printk(KERN_ERR "get handsfree_mic_type fail!\n");
        return HANDSFREE_MIC_MASTER;
    }
    return type;
}

btsco_nrec_adapt_type get_btsco_nrec_adapt_status(void)
{
    int ret = false;
    btsco_nrec_adapt_type  status = BTSCO_NREC_ADAPT_OFF;
    
    ret = get_hw_config_enum("audio/btsco_nrec_adapt", (char*)&status, sizeof(status), NULL);
    if(false == ret)
    {
        printk(KERN_ERR "get btsco_nrec_adapt fail!\n");
        return BTSCO_NREC_ADAPT_OFF;
    }
    return status;
}

int get_audio_enhance_type(void)
{
	  int ret = 0;
	  audio_enhance_type  enhance_type = AUDIO_ENHANCE_INVALID;
	  ret = get_hw_config_enum("audio/enhance_type", (char*)&enhance_type, sizeof(audio_enhance_type), NULL);
	  if(false == ret)
	  {
	  	printk(KERN_ERR "get enhance type  fail!\n");
		return -1;
	  }
	  return enhance_type;
  
}

int board_support_flash(void)
{

		int ret = false;
		bool support = false;
		ret = get_hw_config_bool("flash/support", &support, NULL);
		if(false == ret)
		{
			printk(KERN_ERR "get data fail!\n");
			return -1;
		}
		 return support;
}

static int sensors_list = 0;    /* record sensors which probe success */
/* set sensors_list, called by sensor driver */
void set_sensors_list(int sensor)
{
	sensors_list |= sensor;
}

static int get_sensors_list(void)
{
	return sensors_list;
}
/*8930 doesn't use these board ID*/
char *get_sensors_list_name(void)
{
	int sensors_list = G_SENSOR + L_SENSOR + P_SENSOR + M_SENSOR;
	char *list_name=NULL;

	sensors_list = get_sensors_list();

	switch(sensors_list)
	{
		case G_SENSOR + L_SENSOR + P_SENSOR + M_SENSOR + GY_SENSOR:
			 list_name = "G_L_P_M_GY_SENSORS";
			 break;
			 
		case G_SENSOR + L_SENSOR + P_SENSOR + M_SENSOR:
			 list_name = "G_L_P_M_SENSORS";
			 break;
			 
		case G_SENSOR + L_SENSOR + P_SENSOR:
			 list_name = "G_L_P_SENSORS";
			 break;
			 
		case G_SENSOR:
			 list_name = "G_SENSORS";
			 break;
			 
		case G_SENSOR + M_SENSOR + GY_SENSOR:
			 list_name = "G_M_GY_SENSORS";
			 break;
			 
		case G_SENSOR + M_SENSOR:
			 list_name = "G_M_SENSORS";
			 break;
			 
		case NONE_SENSOR:
			 list_name = "NONE_SENSORS";
			 break;
			 
		default:
			 list_name = "G_L_P_M_SENSORS";
			 break;
	}

	return list_name;
	
}
/*===========================================================================
FUNCTION     get_compass_gs_position

DESCRIPTION
  This function probe which G-sensor & Campass status type should be used

DEPENDENCIES
  This information can be demanded from app_info ,and used in G-sensor and Compass driver
RETURN VALUE
  Composs and G-sensor's layout

SIDE EFFECTS
  None
===========================================================================*/
compass_gs_position_type  get_compass_gs_position(void)
{
	compass_gs_position_type compass_gs_position=COMPASS_TOP_GS_TOP;
	bool ret = false;
	ret = get_hw_config_enum("sensors/compass_pos_name", (char*)&compass_gs_position, sizeof(compass_gs_position_type), NULL);
	if(false == ret)
	{
		compass_gs_position=COMPASS_TOP_GS_TOP;
		return compass_gs_position;
	}
	else
	{
		return compass_gs_position;
	}
}

char *get_compass_gs_position_name(void)
{
	compass_gs_position_type compass_gs_position=COMPASS_TOP_GS_TOP;
	char *position_name=NULL;
	compass_gs_position = get_compass_gs_position();
	switch(compass_gs_position)
	{
		case COMPASS_TOP_GS_TOP:
			position_name = "COMPASS_TOP_GS_TOP";
			break;
		case COMPASS_TOP_GS_BOTTOM:
			position_name = "COMPASS_TOP_GS_BOTTOM";
			break;
		case COMPASS_BOTTOM_GS_TOP:
			position_name = "COMPASS_BOTTOM_GS_TOP";
			break;
		case COMPASS_BOTTOM_GS_BOTTOM:
			position_name = "COMPASS_BOTTOM_GS_BOTTOM";
			break;
		case COMPASS_NONE_GS_BOTTOM:
			position_name = "COMPASS_NONE_GS_BOTTOM";
			break;
		case COMPASS_NONE_GS_TOP:
			position_name = "COMPASS_NONE_GS_TOP";
			break;
		default:
			position_name = "COMPASS_TOP_GS_TOP";
			break;
	}
	return position_name;
}
int get_hw_wifi_device_type(void)
{
    int ret = 0;
    hw_wifi_device_type  wifi_device_type = WIFI_TYPE_UNKNOWN;

    ret = get_hw_config_enum("wifi/device_type", (char*)&wifi_device_type, sizeof(hw_wifi_device_type), NULL);
    if(false == ret)
    {
        printk(KERN_ERR "get  wifi device type fail!\n");
        return -1;
    }

    return wifi_device_type;
}

int get_hw_wifi_clock_type(void)
{
    int ret = 0;
    hw_wifi_clock_type  wifi_clock_type = CLOCK_TYPE_UNKNOWN;
    ret = get_hw_config_enum("wifi/clock_type", (char*)&wifi_clock_type, sizeof(hw_wifi_clock_type), NULL);
    if(false == ret)
    {
        printk(KERN_ERR "get  wifi clock type fail!\n");
        return -1;
    }

    return wifi_clock_type;
}

void get_hw_wifi_pubfile_id(char *pubfile_id, int length)
{
    bool ret = false;

    if(NULL == pubfile_id || length <= 0)
    {
        printk(KERN_ERR "%s:para is error!\n",__func__);
        return;
    }

    ret = get_hw_config_string("wifi/pubfile_id", pubfile_id, length, NULL);

    if(ret == false)
    {
        printk(KERN_ERR"get_hw_wifi_pubfile_id failed!\n");
    }

    return;
}

int get_hw_sim_card_num(void)
{
	  int ret = 0;
	  int  sim_card_num = 0;
	  ret = get_hw_config_int("ds/sim_card_num", &sim_card_num, NULL);
	  if(false == ret)
	  {
	  	printk(KERN_ERR "get sim_card_num fail!\n");
		return -1;
	  }
	  return sim_card_num;
}

int get_hw_sd_trigger_type(void)
{
	  int ret = 0;
	  hw_sd_trigger_type  trigger_type = RAISE_TRIGGER;
	  ret = get_hw_config_enum("sdcard/trigger_type", (char*)&trigger_type, sizeof(hw_sd_trigger_type), NULL);
	  if(false == ret)
	  {
	  	printk(KERN_ERR "get sdcard trigger type fail!\n");
		return -1;
	  }
	  return trigger_type;

}


int get_audio_fm_type(void)
{
		int ret = 0;
		audio_fm_type	fm_type = FM_NVALID;
		ret = get_hw_config_enum("fm/fm_type", (char*)&fm_type, sizeof(audio_fm_type), NULL);
		if(false == ret)
		{
		  printk(KERN_ERR "get fm type fail!\n");
		  return -1;
		}
		return fm_type;

}
int get_bt_fm_device_type(void)
{
	  int ret = false;
	  bt_fm_device_type device_type = BT_FM_UNKNOWN_DEVICE;
	  
	  ret = get_hw_config_enum("bt/device_type", (char*)&device_type, sizeof(bt_fm_device_type), NULL);
	  if(false == ret)
	  {
	  	  printk(KERN_ERR "get bt chip type fail!\n");
		  return -1;
	  }
	  return device_type;  
}
/* interface to get product id */
int get_bt_product_id(char *product_id,unsigned int length)
{
    bool ret = false;

    if((NULL == product_id) || (0 >= length))
    {
        printk(KERN_ERR "%s:Invalid input!\n",__func__);
        return -1;
    }

    ret = get_hw_config_string("bt/product_id", product_id, length, NULL);

    if(false == ret)
    {
        printk(KERN_ERR"Get bt product id faild!\n");
        return -1;
    }

    return 0;
}
int get_fm_sinr(void)
{
	  int ret = false;
	  fm_sinr_value sinr = FM_SINR_MAX;
	  
	  ret = get_hw_config_enum("fm/fm_sinr", (char*)&sinr, sizeof(sinr), NULL);
	  if(false == ret)
	  {
	  	  printk(KERN_ERR "get fm sinr fail!\n");
		  return -1;
	  }
	  return sinr;  
}
int get_fm_rssi(void)
{
	  int ret = false;
	  fm_rssi_value rssi = FM_RSSI_MAX;
	  
	  ret = get_hw_config_enum("fm/fm_rssi", (char*)&rssi, sizeof(rssi), NULL);
	  if(false == ret)
	  {
	  	  printk(KERN_ERR "get fm rssi failed!\n");
		  return -1;
	  }
	  return rssi;  
}
int get_bt_fm_fw_ver(char *bt_fm_fw_ver,unsigned int length)
{
    bool ret = false;

    if((NULL == bt_fm_fw_ver) || (0 >= length))
    {
        printk(KERN_ERR "%s:Invalid input!\n",__func__);
        return -1;
    }

    ret = get_hw_config_string("bt/bt_fm_fw_ver", bt_fm_fw_ver, length, NULL);

    if(false == ret)
    {
        printk(KERN_ERR"Get bt_fm_fw_ver faild!\n");
        return -1;
    }

    return 0;
}

int get_wlan_fw_ver(char *wlan_fw_ver,unsigned int length)
{
    bool ret = false;

    if((NULL == wlan_fw_ver) || (0 >= length))
    {
        printk(KERN_ERR "%s:Invalid input!\n",__func__);
        return -1;
    }

    ret = get_hw_config_string("wifi/wlan_fw_ver", wlan_fw_ver, length, NULL);

    if(false == ret)
    {
        printk(KERN_ERR"Get wlan_fw_ver faild!\n");
        return -1;
    }

    return 0;
}
/*get_lcd_id function is implemented in devices-msm.c*/
extern int get_lcd_id(void);
int lcd_detect_panel(const char *pstring)
{
	/*the key is "lcd/idX, X may be 0, 1, 2 in hex format*/
    char *psKey = NULL;
    int id;
    int ret;
    static bool detected = false;
	char panel_name[MAX_LCD_PANEL_NAME_LEN];

    if(detected == true)
    {
        return -EBUSY;
    }
    if (NULL == (psKey = kmalloc(MAX_KEY_LENGTH, GFP_KERNEL)))  /*maybe we should use kmalloc instead?*/
    {
        ret = -ENOMEM;
        goto err_no_mem;
    }
    memset(psKey, 0, MAX_KEY_LENGTH);
    id = get_lcd_id();
	sprintf(psKey, "lcd/id%X", id);
    ret = get_hw_config_string(psKey, panel_name, MAX_LCD_PANEL_NAME_LEN, NULL);
    if(ret == false)
    {
		/*when there is no panel detected, given a default panel name,prevent kernel hang up*/
		strncpy(panel_name,DEFAULT_PANEL_NAME,MAX_LCD_PANEL_NAME_LEN);
		/* not goto err_get_config */
    }
    ret = strncmp(pstring,panel_name,MAX_LCD_PANEL_NAME_LEN);
    if(ret)
    {
        ret = -ENODEV;
        goto err_detect_panel;
    }

    detected = true;
	/*delete for default lcd panel*/

err_detect_panel:
/*delete for default lcd panel*/
    kfree(psKey);
err_no_mem:
    return ret;
}
int get_lcd_name(char *pname,int length)
{
    char *psKey = NULL;
    int id = 0;
    int ret = 0;
	psKey = kmalloc(MAX_KEY_LENGTH, GFP_KERNEL);
    if (NULL == psKey)  
    {
		kfree(psKey);
		return -1;
    }
    memset(psKey, 0, MAX_KEY_LENGTH);
    id = get_lcd_id();
    sprintf(psKey, "lcd/id%d", id);
    ret = get_hw_config_string(psKey, pname, length, NULL);
    kfree(psKey);
	if(false == ret)
	{
		printk(KERN_ERR "get LCD name fail!\n");
		return -1;
	}
   return ret;
}
#ifdef CONFIG_HW_ESD_DETECT
bool g_Can_Use_SW_Esd;
bool can_use_sw_esd(void)
{
    bool ret = false;
    static bool use_sw_esd = false;
    ret = get_hw_config_bool("lcd/use_sw_esd", &use_sw_esd, NULL);
    if(ret == false)
    {
        printk(KERN_ERR"%s: failed!\n",__func__);
        use_sw_esd = false;
    }
    return use_sw_esd;
}
#endif
/*===========================================================================
FUNCTION     get_lcd_Resolution

DESCRIPTION
  This function provide the basis of allocate FrameBuffer size.

DEPENDENCIES

RETURN VALUE
  LCD's Resolution type

SIDE EFFECTS
  None
===========================================================================*/
lcd_resolution_type  get_lcd_resolution(void)
{
	lcd_resolution_type lcd_Resolution = LCD_IS_WVGA;
	bool ret = false;
	ret = get_hw_config_enum("lcd/resolution", (char*)&lcd_Resolution, sizeof(lcd_resolution_type), NULL);
	if(false == ret)
	{
		lcd_Resolution = LCD_IS_FHD;
		return lcd_Resolution;
	}
	else
	{
		return lcd_Resolution;
	}
}
/*Get battery name from battery r_id*/
int get_battery_name(int batt_r_id,char *pname,int length)
{
    char *psKey = NULL;
    int ret = 0;
    psKey = kmalloc(MAX_KEY_LENGTH, GFP_KERNEL);
    if (NULL == psKey)  
    {
        kfree(psKey);
        return -1;
    }
    memset(psKey, 0, MAX_KEY_LENGTH);
    sprintf(psKey, "battery/R_%d", batt_r_id);
    ret = get_hw_config_string(psKey, pname, length, NULL);
    kfree(psKey);
    if(false == ret)
    {
        printk(KERN_ERR "get battt name fail!\n");
        return -1;
    }
    return ret;
}

int get_camera_mount_type( int cameraid)
{
	int ret = 0;
	camera_mount_type mtype = CAMERA_NO_FLIP_OR_MIRROR ;

	if(CAMERA_IS_MAIN == cameraid)
		ret = get_hw_config_enum("camera/main_mount", (char*)&mtype, sizeof(mtype), NULL);
	else if (CAMERA_IS_FRONT == cameraid)
		ret = get_hw_config_enum("camera/front_mount", (char*)&mtype, sizeof(mtype), NULL);
	else
	{
		printk(KERN_ERR "camera id error, return -1 !\n");
		return -1;
	}

	if(false == ret)
	{
		printk(KERN_ERR "get camera mount type fail !\n");
		return -1;
	}

	return mtype;  
}

int get_camera_sensor_name( char* sensor_name, int length)
{
	int ret = 0;

	ret = get_hw_config_enum("camera/board_name", sensor_name, length, NULL);
	if(false == ret)
	{
		printk(KERN_ERR "get sensor name fail !\n");
		return -1;
	}

	return ret;  
}

int get_charge_standard(char *pname, int length)
{
    int ret = 0;
    if (NULL == pname || length <= 0)
    {
        printk(KERN_ERR "para is error!\n");
        return -1;
    }
    ret = get_hw_config_string("charge/STD", pname, length, NULL);
    if(false == ret)
    {
        printk(KERN_ERR "get charge standard fail!\n");
        return -1;
    }	
    return ret;
}

bool get_mpp5_charge_limit(void)
{
	int ret = false;
	bool support = false;
	ret = get_hw_config_bool("charge/MPP5LMT", &support, NULL);
	if(false == ret)
	{
		printk(KERN_ERR "get data fail!\n");
		return false;
	}
	return support;
}

#define CUTOFF_V_MV 3460

int get_cutoff_v_mv(void)
{
    int cutoff_v_mv = 0;
    bool ret ;
    ret = get_hw_config_int("battery/cutoff_v_mv", &cutoff_v_mv, NULL);
    printk("cutoff_v_mv = %d\n", cutoff_v_mv);
    if ( true != ret) {
        printk("get cut_off_v_delta value wrong,return default value\n");
        return CUTOFF_V_MV;
    }
    return cutoff_v_mv;
}

int get_product_type(char *pname, int length)
{
    int ret = 0;
	if (NULL == pname || length <= 0)
	{
		printk(KERN_ERR "para is error!\n");
		return -1;
	}
    ret = get_hw_config_string("product/name", pname, length, NULL);
	if(false == ret)
	{
		printk(KERN_ERR "get product name fail!\n");
		return -1;
	}	
   return ret;
}

/* gpio get gpio struct */
#ifdef CONFIG_HUAWEI_GPIO_UNITE
struct gpio_config_type *get_gpio_config_table(void)
{
    
	struct board_id_general_struct *gpios_ptr = get_board_id_general_struct(GPIO_MODULE_NAME);
	struct gpio_config_type *gpio_ptr;
    
	if(NULL == gpios_ptr)
	{
		HW_CONFIG_DEBUG(" can not find  module:gpio\n");
		return NULL;
	}
	
	gpio_ptr =(struct gpio_config_type *) gpios_ptr->data_array.gpio_ptr;

    if(NULL != gpio_ptr)
	{
		return gpio_ptr;
	}
	else
	{
		HW_CONFIG_DEBUG(" return NULL\n");
		return NULL;
	}
}

/*gpio get number by name*/
int get_gpio_num_by_name(char *name)
{
    int min = 0;
    int max = NR_GPIO_IRQS - 1;
    int result;
    int new_cursor;
	struct gpio_config_type *gpio_ptr = get_gpio_config_table();

    if(NULL == gpio_ptr)
    {
       HW_CONFIG_DEBUG(" get gpio struct failed.\n");
		return -EFAULT; 
    }

    while(min <= max)
    {
        new_cursor = (min+max)/2;

        if(!(strcmp((gpio_ptr+new_cursor)->name,"")))
        {
            result = 1;
        }
        else
        {
            result = strcmp((gpio_ptr+new_cursor)->name,name);  
        }   
        
        if (0 == result)
        {
            /*found it, just return*/
    		return (gpio_ptr+new_cursor)->gpio_number;
        }
        else if (result > 0)
        {
            /* key is smaller, update max*/
            max = new_cursor-1;
        }
        else if (result < 0)
        {
            /* key is bigger, update min*/
            min = new_cursor+1;
        }        
    }

    return -EFAULT;
}

struct pm_gpio_cfg_t *get_pm_gpio_config_table(void)
{  
	struct board_id_general_struct *pm_gpios_ptr = get_board_id_general_struct(PM_GPIO_MODULE_NAME);
	struct pm_gpio_cfg_t *pm_gpio_ptr;
    
	if(NULL == pm_gpios_ptr)
	{
		HW_CONFIG_DEBUG(" can not find  module:pm gpio\n");
		return NULL;
	}
	
	pm_gpio_ptr =(struct pm_gpio_cfg_t *) pm_gpios_ptr->data_array.pm_gpio_ptr;

    if(NULL != pm_gpio_ptr)
	{
		return pm_gpio_ptr;
	}
	else
	{
		HW_CONFIG_DEBUG(" return NULL\n");
		return NULL;
	}
}

int get_pm_gpio_num_by_name(char *name)
{
   	int min = 0;
    int max = PM8038_GPIO_NUM -1;
    int result;
    int new_cursor;
    struct pm_gpio_cfg_t *pm_gpio_ptr = get_pm_gpio_config_table();

    if(NULL == pm_gpio_ptr)
    {
       HW_CONFIG_DEBUG(" get pm gpio config table failed.\n");
		return -EFAULT; 
    }

    while(min <= max)
    {
        new_cursor = (min+max)/2;

        if(!(strcmp((pm_gpio_ptr+new_cursor)->name,"")))
        {
            result = 1;
        }
        else
        {
            result = strcmp((pm_gpio_ptr+new_cursor)->name,name);  
        }   
        
        if (0 == result)
        {
            /*found it, just return*/
    		return (pm_gpio_ptr+new_cursor)->gpio_number;
        }
        else if (result > 0)
        {
            /* key is smaller, update max*/
            max = new_cursor-1;
        }
        else if (result < 0)
        {
            /* key is bigger, update min*/
            min = new_cursor+1;
        }        
    }

    return -EFAULT;
}
#endif
#ifdef CONFIG_HUAWEI_KERNEL
int get_tp_point_supported(void)
{
    bool ret = false;
    int tp_point_supported = 0;
    ret = get_hw_config_int("tp/finger_num",&tp_point_supported,NULL);
    if(ret == false)
    {
        printk(KERN_ERR"get_tp_point_supported failed!\n");
        return -1;
    }
    return tp_point_supported;
}
void get_tp_resolution(char *tp_resolution,int count)
{
    bool ret = false;
    if(NULL == tp_resolution || count <= 0)
    {
        printk(KERN_ERR "%s:para is error!\n",__func__);
        return;
    }
    ret = get_hw_config_string("tp/resolution", tp_resolution, count, NULL);
    if(ret == false)
    {
        printk(KERN_ERR"get_tp_resolution failed!\n");
    }
    return;
}
void get_virtualkeys_map(char *virtualkeys_map,int count)
{
    bool ret = false;
    if(NULL == virtualkeys_map || count <= 0)
    {
        printk(KERN_ERR "%s:para is error!\n",__func__);
        return;
    }
    ret = get_hw_config_string("tp/virtualkeys", virtualkeys_map, count, NULL);
    if(ret == false)
    {
        printk(KERN_ERR"get_virtualkeys_map failed!\n");
    }
    return;
}
#endif

void get_speaker_type_audio(char *spk_type,int count)
{
    bool ret = false;
    if(NULL == spk_type || count <= 0)
    {
        printk(KERN_ERR "%s:para is error!\n",__func__);
        return;
    }
    ret = get_hw_config_string("audio/spk_type", spk_type, count, NULL);
    if(ret == false)
    {
        printk(KERN_ERR"get_speaker_type_audio failed!\n");
    }
    return;
}

audio_spk_num get_audio_num_spk(void)
{
    int ret = false;
    audio_spk_num  status = SINGLE_SPEAKER;
    
    ret = get_hw_config_enum("audio/spk_num", (char*)&status, sizeof(status), NULL);
    if(false == ret)
    {
        printk(KERN_ERR "get audio/spk_type fail!\n");
        return SINGLE_SPEAKER;
    }

    return status;
}
void get_tp_fw_file_name(char *fw_file_name,int count)
{
    bool ret = false;
    /*any TP driver can all this so, verify the inputs*/
    if (NULL == fw_file_name || count <= 0)
    {
        pr_err("%s:parameters are incorrect!\n",__func__);
        return;
    }

    ret = get_hw_config_string("tp/fw_file", fw_file_name, count, NULL);
    if (ret == false)
    {
        /*as not all products need to configure this, its not an error case.*/
        HW_CONFIG_DEBUG("get_tp_fw_file can't get fw file name!\n");
    }
    return;
}
void get_product_family(char *product_family,int count)
{
    bool ret = false;
    if(NULL == product_family || count <= 0)
    {
        printk(KERN_ERR "%s:para is error!\n",__func__);
        return;
    }
    ret = get_hw_config_string("tp/product_family", product_family, count, NULL);
    if(ret == false)
    {
        printk(KERN_ERR"get_product_family failed!\n");
    }
    return;
}
/*  FUNCTION  get_touch_type
 *  DEPENDENCIES 
 *      get tp project type.
 *  RETURN VALUE
 *      TP_COB: use cob code auto upgreade FW.
 *      TP_COF: Not use cob code, use before code.
 */
tp_type get_touch_type(void)
{
    bool ret = false;
    char tp_tpye[MAX_TP_TYPE] = {0};

    ret = get_hw_config_string("tp/type", tp_tpye, sizeof(tp_tpye), NULL);
    if(ret == false)
    {
        printk(KERN_ERR"get_touch_type failed!\n");
    }
    
    if(0 == strncmp(tp_tpye,COB,sizeof(tp_tpye)))
    {
        return TP_COB;
    }
    else if(0 == strncmp(tp_tpye,COF,sizeof(tp_tpye)))
    {
        return TP_COF;
    }
    else
    {
        printk("default tp_type: COF\n");
        return TP_COF;
    }
}

/* Configuration upgrade mode */
/* default need to update fw */
tp_update_type is_need_update_fw(void)
{
    return NEED_UPDATE_FW;
}

char *get_touch_info(void)
{
    char *touch_info = NULL;

    touch_info = get_synaptics_touch_info();
    if (touch_info != NULL)
        return touch_info;

#ifdef CONFIG_HUAWEI_CYPRESS_TOUCHSCREEN
    touch_info = cyttsp_get_touch_info();
    if (touch_info != NULL)
        return touch_info;
#endif

#ifdef CONFIG_TOUCHSCREEN_CYPRESS_CYTTSP4
	touch_info = get_cyttsp4_touch_info();
	if (touch_info != NULL)
		return touch_info;
#endif

    return NULL;
}

#ifdef CONFIG_HW_POWER_TREE 
struct hw_config_power_tree *get_power_config_table(void)
{    
	struct board_id_general_struct *power_general_struct = get_board_id_general_struct(POWER_MODULE_NAME);
	struct hw_config_power_tree *power_ptr = NULL;
    
	if(NULL == power_general_struct)
	{
		HW_CONFIG_DEBUG("Can not find module:regulator\n");
		return NULL;
	}
	
	power_ptr =(struct hw_config_power_tree*)power_general_struct->data_array.power_tree_ptr;

  if(NULL == power_ptr)
	{
		HW_CONFIG_DEBUG("hw_config_power_tree return  NULL\n");
	}
	return power_ptr;
}
#endif

/* to delete the code to optimize  */
/*Add for 8930 audioparameters self adapt*/  
/* delete */
#define MAX_DEVICES_LENGTH 64
int get_I2C_devices(char *device_name)
{
    unsigned int det_value = 0;
    bool ret ;
    char I2C_devices[MAX_DEVICES_LENGTH] = {'\0'};

    snprintf(I2C_devices, sizeof(I2C_devices), "i2c/%s", device_name);
    ret = get_hw_config_int(I2C_devices, &det_value, NULL);
    printk("hsad: I2C_devices. \"%s\". %d\n", I2C_devices,det_value);
    if (ret == true) {
        return det_value;
    }
    return -1;
}

/*get proximity wave value*/
#define PX_WAVE_DEFAULT 100
#define PX_WINDOW_DEFAULT 200
/*this 7 should be align with aps-9900.h*/
#define LUX_NUMBER 7
int get_proximity_wave(void)
{
    int px_wave_value = 0;
    bool ret ;
    ret = get_hw_config_int("sensors/px_wave", &px_wave_value, NULL);
    printk("hsad: proximity px_wave = %d\n", px_wave_value);
    if ( true != ret) {
        printk("get px_wave value wrong,return default value\n");
        return PX_WAVE_DEFAULT;
    }
    return px_wave_value;
}
/*get proximity window value*/
int get_proximity_window(void)
{
    int px_window_value = 0;
    bool ret ;
    ret = get_hw_config_int("sensors/px_window", &px_window_value, NULL);
    printk("hsad: proximity px_window_value = %d\n", px_window_value);
    if ( true != ret) {
        printk("get px_window value wrong,return default value\n");
        return PX_WINDOW_DEFAULT;
    }
    return px_window_value;
}
/*this fuction is transform string to number*/
int atoi_light(const char *name)
{
    int val = 0;
    for (;; name++) {
        switch (*name) {
        case '0' ... '9':
            val = 10*val+(*name-'0');
            break;
        default:
            return val;
        }
    }
}
/*return light value from product xml to kernel*/
void get_light_value(int *lux)
{
    char lux_string[MAX_DEVICES_LENGTH] ;
    bool ret ;
    int i = 0;
    char *p;
    char *buf;
    ret = get_hw_config_string("sensors/light_lux", lux_string, sizeof(lux_string), NULL);
    if( true != ret)
    {
        printk("get light_lux value fail!\n");
        strcpy(lux_string,"9_25_110_400_750_1200_3200");
    }
    printk("hsad: light_sensor. \"%s\". \n", lux_string);
    buf = lux_string;
    for(i = 0; i < LUX_NUMBER ; i++)
    {
        p = strsep(&buf,"_");
        if(!p)
            printk("strsep error is hsad in get_light_value \n");
        lux[i] = atoi_light(p);
    }
}
/*get vibrator voltage level*/
#define VIBRATOR_VOLTAGE_DEFAULT 2700
int get_vibrator_voltage(void)
{
    int vibrator_voltage = 0;
    bool ret ;
    ret = get_hw_config_int("vibrator/level_mV", &vibrator_voltage, NULL);
    printk("hsad: vibrator voltage_level = %d\n", vibrator_voltage);
    if ( true != ret) {
        printk("get vibrator voltage_level wrong,return default value\n");
        return VIBRATOR_VOLTAGE_DEFAULT;
    }
    return vibrator_voltage;
}
int get_proximity_sunlight(void)
{
    int sunlight = 0;
    bool ret ;
    ret = get_hw_config_int("sensors/sunlight", &sunlight, NULL);
    printk("hsad: proximity sunlight = %d\n", sunlight);
    if ( true != ret) {
        printk("get px_wave value wrong,return default value\n");
        return 30000;//default value,75% of maximum range
    }
    return sunlight;
}
bool qwerty_is_supported(void)
{
    bool ret = false;
    bool support_qwerty = false;
    ret = get_hw_config_bool("keypad/qwerty", &support_qwerty, NULL);
    if(ret == false)
    {
        printk(KERN_ERR"%s: failed!\n",__func__);
        support_qwerty = false;
    }
    return support_qwerty;
}
/* get whether the current product support NFC feature */
int get_nfc_support(void)
{
	  int ret = false;
	  nfc_support_type nst = NO_SUPPORT;
	  
	  ret = get_hw_config_enum("nfc/nfc_support", (char*)&nst, sizeof(nst), NULL);
	  if(false == ret)
	  {
	  	  printk(KERN_ERR "get nfc property fail!\n");
		  return -1;
	  }
      printk("get_nfc_support: result is %d\n", nst);
	  return nst;  
}
/* get the type of the NFC chipset */
int get_nfc_chipset_type(void)
{
     int ret = false;
     nfc_chipset_type nct = NONE;

     ret = get_hw_config_enum("nfc/nfc_chip_type", (char*)&nct, sizeof(nct), NULL);
     if(false == ret)
     {
     	   printk(KERN_ERR "get nfc chip type fail!\n");
          return NONE;
     }
     printk("get_nfc_support: result is %d\n", nct);

     return nct;     
}

pmic_gpio_type get_pmic_gpio_type(int gpio)
{
    int ret = false;
    pmic_gpio_type type = PMIC_GPIO_DEFAULT;
    char *psKey = NULL;

    /*allocate memory to make the dynamic key*/
    psKey = kmalloc(MAX_KEY_LENGTH, GFP_KERNEL);
    if (NULL == psKey)  
    {
        printk(KERN_ERR "memory allocation failed for pmic gpio type!\n");
        /*in case of error do not change the default config*/
        return PMIC_GPIO_DEFAULT;
    }

    memset(psKey, 0, MAX_KEY_LENGTH);
    sprintf(psKey, "pmicgpio/%d", gpio);
    ret = get_hw_config_enum(psKey, (char*)&type, sizeof(type), NULL);
    kfree(psKey);

    if(false == ret)
    {
      return PMIC_GPIO_DEFAULT;
    }
    
    HW_CONFIG_DEBUG("get_pmic_gpio_type: result is %d\n", gpio);
    return type;
}

int get_key_custom_num(const char *name)
{
    int ret = 0;
    int  key_gpio_num = -1;
    char config_key_name[MAX_KEY_LENGTH] = {'\0'};

    snprintf(config_key_name, sizeof(config_key_name), "customkey/%s", name);

    ret = get_hw_config_int(config_key_name, &key_gpio_num, NULL);
    if (false == ret)
    {
        pr_err(KERN_ERR "get key_gpio_num fail for %s!\n", name);
        return -1;
    }
    return key_gpio_num;
}
/*
 *Despite the same backlight current, different LCD panels may differ in luminance,
 *Use this function to adapt the current to get good consistency in LCD luminance.
 */
int get_wled_max_current(void)
{
    bool ret = 0;
    int max_current = -1;
    char led_name[MAX_KEY_LENGTH] = {'\0'};
    int id = -1;

    id = get_lcd_id();
    snprintf(led_name, sizeof(led_name), "lcd/id_%d_wled_max_current", id);

    ret = get_hw_config_int(led_name, &max_current, NULL);
    if(true == ret)
    {
        pr_err("wled:id%d backlight max current is %d!\n", id, max_current);
        return max_current;
    }
    max_current = -1;
    memset(led_name, 0, MAX_KEY_LENGTH);

    snprintf(led_name, sizeof(led_name), "lcd/default_wled_max_current");

    ret = get_hw_config_int(led_name, &max_current, NULL);
    if(false == ret)
    {
        pr_err(KERN_ERR "get wled max current failed!\n");
        return -1;
    }
    return max_current;
}
