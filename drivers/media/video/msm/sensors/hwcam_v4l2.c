#include "msm_sensor.h"
#define SENSOR_NAME "hwcam"
#define PLATFORM_DRIVER_NAME "msm_camera_hwcam"


static struct task_struct *  g_cam_worker_thread;
extern int sensor_post_init_thread(void * data);

static int __init hw_sensor_init_module(void)
{
	g_cam_worker_thread=kthread_create(sensor_post_init_thread,NULL,"cam_post"); 
	if(!IS_ERR(g_cam_worker_thread)){
				wake_up_process(g_cam_worker_thread);
		}
       return 0;
}

module_init(hw_sensor_init_module);
MODULE_DESCRIPTION("huawei Fake sensor driver");
MODULE_LICENSE("GPL v2");
