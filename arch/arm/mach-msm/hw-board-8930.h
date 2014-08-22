#ifndef HW_BOARD_8930_H
#define HW_BOARD_8930_H
#include <hsad/config_interface.h>
#include <hsad/config_debugfs.h>
#ifdef CONFIG_HUAWEI_KERNEL
#include <linux/touch_platform_config.h>
#include <linux/regulator/consumer.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <mach/gpio.h>
#include <linux/input.h>
#endif
#include <mach/msm_xo.h>
#ifdef CONFIG_HUAWEI_NFC_PN544
#include <linux/nfc/pn544.h>
#endif

#ifdef CONFIG_HUAWEI_KERNEL
#include <linux/nfc/bcm2079x.h>
#endif

#ifdef CONFIG_HUAWEI_KERNEL
#define MSM_8930_TOUCH_INT_PIN 11
#define MSM_8930_TOUCH_RESET_PIN 52

int power_switch(int pm);
int set_touch_interrupt_gpio(void);
int touch_reset(void);
int get_touch_reset_gpio(void);
void set_touch_probe_flag(int detected);
int read_touch_probe_flag(void);
int get_touch_resolution(struct tp_resolution_conversion *tp_resolution_type);
int read_button_flag(void);
int get_touch_button_map(struct tp_button_map *tp_button_map);

/*delete some lines, the function is moved to power_switch() in hw-board-8930.c*/
void __init virtualkeys_init(void);
#endif


#ifdef CONFIG_HUAWEI_CYPRESS_TOUCHSCREEN
/* default is to build for Txx3xx */
#if !defined(CY_USE_GEN2) && !defined(CY_USE_GEN3)
#define CY_USE_GEN3
#elif defined(CY_USE_GEN2) && defined(CY_USE_GEN3)
#undef CY_USE_GEN2
#endif

/* Use the following define if including an autoload firmware image
#define CY_USE_AUTOLOAD_FW
 */
#ifndef CONFIG_HUAWEI_KERNEL /*Use bin file to update FW*/
#define CY_USE_AUTOLOAD_FW
#endif /*added by huawei*/

#define CY_I2C_NAME     "cyttsp3-i2c"

#ifndef CONFIG_HUAWEI_KERNEL /*added by huawei*/
#define CY_I2C_TCH_ADR	0x24
#define CY_I2C_LDR_ADR	0x24
#else/*add by huawei*/
#define CY_I2C_TCH_ADR	0x1B
#define CY_I2C_LDR_ADR	0x1B
#endif /*add by huawei*/

#ifdef CY_USE_GEN2
#define CY_MAXX 170
#define CY_MAXY 310
#endif /* --CY_USE_GEN2 */

#ifdef CY_USE_GEN3
#ifndef CONFIG_HUAWEI_KERNEL /*add by huawei*/
#define CY_MAXX 880
#define CY_MAXY 1280
#else/*add by huawei*/
/*When the LCD size 480x800*/
#define CY_MAXX 480
#define CY_MAXY 800
#endif /*add by huawei*/
#endif /* --CY_USE_GEN3 */

#define CY_WAKE_DFLT                99	/* causes wake strobe on INT line
					 * in sample board configuration
					 * platform data->hw_recov() function
					 */

#define CY_ABS_MIN_X 0
#define CY_ABS_MIN_Y 0
#define CY_ABS_MIN_P 0
#define CY_ABS_MIN_W 0

#ifdef CY_USE_GEN2
#define CY_ABS_MIN_T 0
#endif /* --CY_USE_GEN2 */

#ifdef CY_USE_GEN3
#define CY_ABS_MIN_T 1
#endif /* --CY_USE_GEN3 */

#define CY_ABS_MAX_X CY_MAXX
#define CY_ABS_MAX_Y CY_MAXY
#define CY_ABS_MAX_P 255
#define CY_ABS_MAX_W 255

#ifdef CY_USE_GEN2
#define CY_ABS_MAX_T 1
#endif /* --CY_USE_GEN2 */

#ifdef CY_USE_GEN3
#define CY_ABS_MAX_T 14
#endif /* --CY_USE_GEN3 */


#define CY_IGNORE_VALUE 0xFFFF

int cyttsp3_hw_reset(void);
int cyttsp3_hw_recov(int on);
int cyttsp3_irq_stat(void);
#endif

/*------------------------huawei nfc-------------------------*/
#ifdef CONFIG_HUAWEI_NFC_PN544
int pn544_ven_reset(void);

int pn544_interrupt_gpio_config(void);

int pn544_fw_download_pull_down(void);

int pn544_fw_download_pull_high(void);

int pn544_clock_output_ctrl(int vote);

// expand func function: add close PMU output function
// mode = 0 : close for clock pmu request mode,  mode = 1 : Set for clock pmu request mode
int pn544_clock_output_mode_ctrl(int mode);
#endif

/* -------------------- huawei sensors -------------------- */

#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_ACCELEROMETER_ADI_ADXL346
int gsensor_support_dummyaddr_adi346(void);
#endif

#ifdef CONFIG_HUAWEI_FEATURE_SENSORS_ACCELEROMETER_KXTIK1004
int gsensor_support_dummyaddr_kxtik(void);
#endif

#ifdef CONFIG_HUAWEI_FEATURE_PROXIMITY_EVERLIGHT_APS_9900
int aps9900_gpio_config_interrupt(void);
#endif
#endif
