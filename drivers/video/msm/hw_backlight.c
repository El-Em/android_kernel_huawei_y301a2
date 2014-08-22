/* drivers\video\msm\hw_backlight.c
 * backlight driver for 8x30 platform
 *
 * Copyright (C) 2012 HUAWEI Technology Co., ltd.
 * 
 * Date: 2012/10/10
 * By lijianzhao
 * 
 */

#include "msm_fb.h"
#include <linux/mfd/pmic8058.h>
#include <mach/vreg.h>
#include <linux/kernel.h>
#include <mach/pmic.h>
#include <linux/earlysuspend.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/hw_backlight.h>


static struct msm_fb_data_type *mfd_local;
static boolean backlight_set = FALSE;
static atomic_t suspend_flag = ATOMIC_INIT(0);

void pwm_set_backlight(struct msm_fb_data_type *mfd)
{
	struct msm_fb_panel_data *pdata = NULL;
	/*When all the device are resume that can turn the light*/
	if(atomic_read(&suspend_flag)) 
	{
		mfd_local = mfd;
		backlight_set = TRUE;
		return;
	}
	pdata = (struct msm_fb_panel_data *)mfd->pdev->dev.platform_data; 
	if ((pdata) && (pdata->set_brightness))
	{
		pdata->set_brightness(mfd);
	}
	return;
}
#ifdef CONFIG_HAS_EARLYSUSPEND
static void pwm_backlight_suspend( struct early_suspend *h)
{
	atomic_set(&suspend_flag,1);
}

static void pwm_backlight_resume( struct early_suspend *h)
{
	atomic_set(&suspend_flag,0);
	if (backlight_set == TRUE)
	{
		down(&mfd_local->sem);
		pwm_set_backlight(mfd_local);
		up(&mfd_local->sem);
	}
}
/*add early suspend*/
static struct early_suspend pwm_backlight_early_suspend = {
	.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN - 1,
	.suspend = pwm_backlight_suspend,
	.resume = pwm_backlight_resume,
};
#endif

static int __init pwm_backlight_init(void)
{
	
#ifdef CONFIG_HAS_EARLYSUSPEND
	register_early_suspend(&pwm_backlight_early_suspend);
#endif
	return 0;
}
module_init(pwm_backlight_init);
