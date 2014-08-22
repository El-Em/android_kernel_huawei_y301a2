/* Copyright (c) 2010-2012, The Linux Foundation. All rights reserved.
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

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/reboot.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/cpu.h>
#include <linux/interrupt.h>
#include <linux/mfd/pmic8058.h>
#include <linux/mfd/pmic8901.h>
#include <linux/mfd/pm8xxx/misc.h>

#include <asm/mach-types.h>

#include <mach/msm_iomap.h>
#include <mach/restart.h>
#include <mach/socinfo.h>
#include <mach/irqs.h>
#include <mach/scm.h>
#include "msm_watchdog.h"
#include "timer.h"

#define WDT0_RST	0x38
#define WDT0_EN		0x40
#define WDT0_BARK_TIME	0x4C
#define WDT0_BITE_TIME	0x5C

#define PSHOLD_CTL_SU (MSM_TLMM_BASE + 0x820)

#define RESTART_REASON_ADDR 0x65C
#define DLOAD_MODE_ADDR     0x0

#define SCM_IO_DISABLE_PMIC_ARBITER	1

#ifdef CONFIG_MSM_RESTART_V2
#define use_restart_v2()	1
#else
#define use_restart_v2()	0
#endif

static int restart_mode;
void *restart_reason;

#ifdef CONFIG_HUAWEI_KERNEL
#define RESTART_FLAG_ADDR  0x800
#define RESTART_FLAG_MAGIC_NUM  0x25866220
#define RESETFACTORY_MAGIC_NUM  0x77665520
#define RESETUSER_MAGIC_NUM  0x77665522
#define OEMRTC_FLAG_MAGIC_NUM  0x77665524
#define SDUPDATE_FLAG_MAGIC_NUM  0x77665528
#define USBUPDATE_FLAG_MAGIC_NUM  0x77665523
#define QFUSE_MAGIC_NUM  0xF4C3D2C1
#define QFUSE_MAGIC_OFFSET  0x30
void *restart_flag_addr;
#endif
int pmic_reset_irq;
static void __iomem *msm_tmr0_base;

#ifdef CONFIG_MSM_DLOAD_MODE
static int in_panic;
static void *dload_mode_addr;

/* Download mode master kill-switch */
static int dload_set(const char *val, struct kernel_param *kp);
#ifdef CONFIG_HUAWEI_KERNEL
static int nv_dload_set(const char *val, struct kernel_param *kp);
#endif

static int download_mode = 1;
#ifdef CONFIG_HUAWEI_KERNEL
static int nv_download_mode = 0;
#endif

module_param_call(download_mode, dload_set, param_get_int,
			&download_mode, 0644);
#ifdef CONFIG_HUAWEI_KERNEL
module_param_call(nv_download_mode, nv_dload_set, param_get_int,
			&nv_download_mode, 0644);
#endif

static int panic_prep_restart(struct notifier_block *this,
			      unsigned long event, void *ptr)
{
	in_panic = 1;
	return NOTIFY_DONE;
}

static struct notifier_block panic_blk = {
	.notifier_call	= panic_prep_restart,
};

#ifdef CONFIG_HUAWEI_KERNEL
/*****************************************************************************
 Function    : set_dload_mode
 Description  : set dload mode, Only if the nv_download_mode = 1(nv905 = 0), can go to download mode
 Input        :  int on  
 Output       : 
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/28/9
    Author       : liuting
    Modification : Created function

*****************************************************************************/
static void set_dload_mode(int on)
{
       /*Only if the nv_download_mode = 1(nv905 = 0), can go to download mode*/
	if (dload_mode_addr){
		__raw_writel((on && nv_download_mode) ? 0xE47B337D : 0, dload_mode_addr);
		__raw_writel((on && nv_download_mode) ? 0xCE14091A : 0,
		       dload_mode_addr + sizeof(unsigned int));
		if (on && nv_download_mode)
		{
			printk("nv_download_mode = 1, set to error dump mode.\n");
		}
		mb();
	}
}

/*****************************************************************************
 Function    : nv_dload_set
 Description  : set nv_download_mode
 Input        :  onst char *val, struct kernel_param *kp  
 Output       : 
 Return Value : FAIL or SUCCESS
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/28/9
    Author       : liuting
    Modification : Created function

*****************************************************************************/
static int nv_dload_set(const char *val, struct kernel_param *kp)
{
	int ret;
	int old_val = nv_download_mode;

	ret = param_set_int(val, kp);

	if (ret)
		return ret;

	/* If nv_download_mode is not zero or one, ignore. */
	if (nv_download_mode >> 1) {
		nv_download_mode = old_val;
		return -EINVAL;
	}

	set_dload_mode(download_mode);

	return 0;
}
#else
static void set_dload_mode(int on)
{
	if (dload_mode_addr) {
		__raw_writel(on ? 0xE47B337D : 0, dload_mode_addr);
		__raw_writel(on ? 0xCE14091A : 0,
		       dload_mode_addr + sizeof(unsigned int));
		mb();
	}
}
#endif

static int dload_set(const char *val, struct kernel_param *kp)
{
	int ret;
	int old_val = download_mode;

	ret = param_set_int(val, kp);

	if (ret)
		return ret;

	/* If download_mode is not zero or one, ignore. */
	if (download_mode >> 1) {
		download_mode = old_val;
		return -EINVAL;
	}

	set_dload_mode(download_mode);

	return 0;
}
#else
#define set_dload_mode(x) do {} while (0)
#endif

void msm_set_restart_mode(int mode)
{
	restart_mode = mode;
}
EXPORT_SYMBOL(msm_set_restart_mode);

static void __msm_power_off(int lower_pshold)
{
	printk(KERN_CRIT "Powering off the SoC\n");
#ifdef CONFIG_MSM_DLOAD_MODE
	set_dload_mode(0);
#endif
	pm8xxx_reset_pwr_off(0);

	if (lower_pshold) {
		if (!use_restart_v2())
			__raw_writel(0, PSHOLD_CTL_SU);
		else
			__raw_writel(0, MSM_MPM2_PSHOLD_BASE);

		mdelay(10000);
		printk(KERN_ERR "Powering off has failed\n");
	}
	return;
}

static void msm_power_off(void)
{
	/* MSM initiated power off, lower ps_hold */
	__msm_power_off(1);
}

static void cpu_power_off(void *data)
{
	int rc;

	pr_err("PMIC Initiated shutdown %s cpu=%d\n", __func__,
						smp_processor_id());
	if (smp_processor_id() == 0) {
		/*
		 * PMIC initiated power off, do not lower ps_hold, pmic will
		 * shut msm down
		 */
		__msm_power_off(0);

		pet_watchdog();
		pr_err("Calling scm to disable arbiter\n");
		/* call secure manager to disable arbiter and never return */
		rc = scm_call_atomic1(SCM_SVC_PWR,
						SCM_IO_DISABLE_PMIC_ARBITER, 1);

		pr_err("SCM returned even when asked to busy loop rc=%d\n", rc);
		pr_err("waiting on pmic to shut msm down\n");
	}

	preempt_disable();
	while (1)
		;
}

static irqreturn_t resout_irq_handler(int irq, void *dev_id)
{
	pr_warn("%s PMIC Initiated shutdown\n", __func__);
	oops_in_progress = 1;
	smp_call_function_many(cpu_online_mask, cpu_power_off, NULL, 0);
	if (smp_processor_id() == 0)
		cpu_power_off(NULL);
	preempt_disable();
	while (1)
		;
	return IRQ_HANDLED;
}

#ifdef CONFIG_HUAWEI_KERNEL
static void qfuse_handle(const char *cmd)
{
    char *fuse_data_p = NULL;
    unsigned int rdata = 0;

    if(NULL == cmd)
    {
        return;
    }
    
    //write qfuse magic
    __raw_writel(QFUSE_MAGIC_NUM, (dload_mode_addr+QFUSE_MAGIC_OFFSET)); /*addr 0x2A03F000 */

    //get r0
    fuse_data_p = strstr(cmd, "r0=");
    rdata = 0;
    if(NULL != fuse_data_p)
    {
        fuse_data_p = fuse_data_p+3;
        if(NULL != fuse_data_p)
        {
            rdata = simple_strtoul(fuse_data_p, NULL, 16);
        }
    }
    //write r0
    pr_err("qfuse_handle r0 = 0x%x\n", rdata);
    __raw_writel(rdata, (dload_mode_addr+QFUSE_MAGIC_OFFSET+4));
    pr_err("qfuse_handle dload_mode_addr r0 = 0x%x\n", *((unsigned int*)(dload_mode_addr+QFUSE_MAGIC_OFFSET+4)));

    //get r1
    fuse_data_p = strstr(cmd, "r1=");
    rdata = 0;
    if(NULL != fuse_data_p)
    {
        fuse_data_p = fuse_data_p+3;
        if(NULL != fuse_data_p)
        {
            rdata = simple_strtoul(fuse_data_p, NULL, 16);
        }
    }
    //write r1
    pr_err("qfuse_handle r1 = 0x%x\n", rdata);
    __raw_writel(rdata, (dload_mode_addr+QFUSE_MAGIC_OFFSET+8));
    pr_err("qfuse_handle dload_mode_addr r1 = 0x%x\n", *((unsigned int*)(dload_mode_addr+QFUSE_MAGIC_OFFSET+8)));

    //get r2
    fuse_data_p = strstr(cmd, "r2=");
    rdata = 0;
    if(NULL != fuse_data_p)
    {
        fuse_data_p = fuse_data_p+3;
        if(NULL != fuse_data_p)
        {
            rdata = simple_strtoul(fuse_data_p, NULL, 16);
        }
    }

    //write r2
    pr_err("qfuse_handle r2 = 0x%x\n", rdata);
    __raw_writel(rdata, (dload_mode_addr+QFUSE_MAGIC_OFFSET+12));
    pr_err("qfuse_handle dload_mode_addr r2 = 0x%x\n", *((unsigned int*)(dload_mode_addr+QFUSE_MAGIC_OFFSET+12)));

    //get r3
    fuse_data_p = strstr(cmd, "r3=");
    rdata = 0;
    if(NULL != fuse_data_p)
    {
        fuse_data_p = fuse_data_p+3;
        if(NULL != fuse_data_p)
        {
            rdata = simple_strtoul(fuse_data_p, NULL, 16);
        }
    }
    //write r3
    pr_err("qfuse_handle r3 = 0x%x\n", rdata);
    __raw_writel(rdata, (dload_mode_addr+QFUSE_MAGIC_OFFSET+16));
    pr_err("qfuse_handle dload_mode_addr r3 = 0x%x\n", *((unsigned int*)(dload_mode_addr+QFUSE_MAGIC_OFFSET+16)));

    pr_err("qfuse_handle dload_mode_addr magic = 0x%x\n", *((unsigned int*)(dload_mode_addr+QFUSE_MAGIC_OFFSET)));

    //enter recovery mode
    //__raw_writel(0x77665502, restart_reason); 

    mb();
}
#endif

static void msm_restart_prepare(const char *cmd)
{
#ifdef CONFIG_MSM_DLOAD_MODE

	/* This looks like a normal reboot at this point. */
	set_dload_mode(0);

	/* Write download mode flags if we're panic'ing */
	set_dload_mode(in_panic);

	/* Write download mode flags if restart_mode says so */
	if (restart_mode == RESTART_DLOAD)
		set_dload_mode(1);

	/* Kill download mode if master-kill switch is set */
	if (!download_mode)
		set_dload_mode(0);
#endif

	printk(KERN_NOTICE "Going down for restart now\n");

#ifdef CONFIG_HUAWEI_KERNEL
       /* write the flag for reboot action */
       __raw_writel(RESTART_FLAG_MAGIC_NUM,  restart_flag_addr);
#endif
	pm8xxx_reset_pwr_off(1);

	if (cmd != NULL) {
		if (!strncmp(cmd, "bootloader", 10)) {
			__raw_writel(0x77665500, restart_reason);
		} else if (!strncmp(cmd, "recovery", 8)) {
			__raw_writel(0x77665502, restart_reason);
#ifdef CONFIG_HUAWEI_KERNEL
		} else if(!strncmp(cmd, "resetfactory", 12)){
			__raw_writel(RESETFACTORY_MAGIC_NUM, restart_reason);
		} else if(!strncmp(cmd, "resetuser", 9)){
			__raw_writel(RESETUSER_MAGIC_NUM, restart_reason);
		} else if(!strncmp(cmd, "usbdload", 8)){
			set_dload_mode(1);
		/* power off alarm */
		} else if(!strncmp(cmd, "oem-rtc", 7)) {
			__raw_writel(OEMRTC_FLAG_MAGIC_NUM, restart_reason);
		} else if(!strncmp(cmd, "sdupdate", 8)) {
			__raw_writel(SDUPDATE_FLAG_MAGIC_NUM, restart_reason);
		} else if(!strncmp(cmd, "usbupdate", 9)){
		__raw_writel(USBUPDATE_FLAG_MAGIC_NUM, restart_reason);
		} else if(!strncmp(cmd, "qfuse", 5)) {
		     qfuse_handle(cmd);
#endif
		} else if (!strncmp(cmd, "oem-", 4)) {
			unsigned long code;
			code = simple_strtoul(cmd + 4, NULL, 16) & 0xff;
			__raw_writel(0x6f656d00 | code, restart_reason);
		} else {
			__raw_writel(0x77665501, restart_reason);
		}
	}
}

void msm_restart(char mode, const char *cmd)
{
	printk(KERN_NOTICE "Going down for restart now\n");

	msm_restart_prepare(cmd);

	if (!use_restart_v2()) {
		__raw_writel(0, msm_tmr0_base + WDT0_EN);
		if (!(machine_is_msm8x60_fusion() ||
		      machine_is_msm8x60_fusn_ffa())) {
			mb();
			 /* Actually reset the chip */
			__raw_writel(0, PSHOLD_CTL_SU);
			mdelay(5000);
			pr_notice("PS_HOLD didn't work, falling back to watchdog\n");
		}

		__raw_writel(1, msm_tmr0_base + WDT0_RST);
		__raw_writel(5*0x31F3, msm_tmr0_base + WDT0_BARK_TIME);
		__raw_writel(0x31F3, msm_tmr0_base + WDT0_BITE_TIME);
		__raw_writel(1, msm_tmr0_base + WDT0_EN);
	} else
		__raw_writel(0, MSM_MPM2_PSHOLD_BASE);

	mdelay(10000);
	printk(KERN_ERR "Restarting has failed\n");
}

static int __init msm_pmic_restart_init(void)
{
	int rc;

#ifdef CONFIG_HUAWEI_KERNEL
	restart_flag_addr = MSM_IMEM_BASE  + RESTART_FLAG_ADDR;
#endif
	if (pmic_reset_irq != 0) {
		rc = request_any_context_irq(pmic_reset_irq,
					resout_irq_handler, IRQF_TRIGGER_HIGH,
					"restart_from_pmic", NULL);
		if (rc < 0)
			pr_err("pmic restart irq fail rc = %d\n", rc);
	} else {
		pr_warn("no pmic restart interrupt specified\n");
	}

	return 0;
}

late_initcall(msm_pmic_restart_init);

static int __init msm_restart_init(void)
{
#ifdef CONFIG_MSM_DLOAD_MODE
	atomic_notifier_chain_register(&panic_notifier_list, &panic_blk);
	dload_mode_addr = MSM_IMEM_BASE + DLOAD_MODE_ADDR;
	set_dload_mode(download_mode);
#endif
	msm_tmr0_base = msm_timer_get_timer0_base();
	restart_reason = MSM_IMEM_BASE + RESTART_REASON_ADDR;
	pm_power_off = msm_power_off;

	return 0;
}
early_initcall(msm_restart_init);
