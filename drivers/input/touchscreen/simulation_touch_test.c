/* \kernel\drivers\input\touchscreen\simulation_touch_test.c
 *
 * Copyright (C) 2013 HUAWEI, Inc.
 *
 */

#include <linux/module.h>
#include <linux/input.h>
#include <linux/hrtimer.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/hw_tp_config.h>
#include <linux/ktime.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/namei.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>
#include <linux/touch_platform_config.h>

#define REPORT_DATA_BUFFER_SIZE  (1024*25)
#define TOUCH_DEFAULT_REPORT_FREQ   80         /* the default freq for touch report */

static struct input_dev *simulation_touch_dev;
static struct kobject *kobject_ts;
static char* test_touch_buffer;                              /* Use to save data from touch_data.txt */
static char report_time_buffer[REPORT_DATA_BUFFER_SIZE];     /* Use to save the report data */
static int x_coordinate = -1;         /* X Coordinate */
static int y_coordinate = -1;         /* Y Coordinate */
static char *dest_data[1024];

static ssize_t touch_test_show(struct kobject *kobj,struct kobj_attribute *attr,char *buf);
static ssize_t touch_test_store(struct kobject *kobj,struct kobj_attribute *attr,const char *buf,size_t count);

static struct kobj_attribute touch_test_attribute = {
      .attr = {.name = "touch_test_flag",.mode = 0664},
      .show = touch_test_show,
      .store = touch_test_store,
};


extern void  display_set_record_start(int flag);
extern int get_touch_resolution(struct tp_resolution_conversion *tp_resolution_type);

/*****************************************
  @func_name:  hextodec
  @para: char s[]  hex string bo be transferred
  @func: Transfer hex string to Dec number
  @return  int  the result
******************************************/
int hextodec(char s[])
{
    int i,m,temp=0,n;
    m=strlen(s);
    for(i=0;i<m;i++)
    {
        if(s[i]>='A'&&s[i]<='F')
         n=s[i]-'A'+10;
        else if(s[i]>='a'&&s[i]<='f')
         n=s[i]-'a'+10;
         else n=s[i]-'0';
        temp=temp*16+n;
    }
    return temp;
}

/*****************************************
  @func_name:  atoi
  @para: char *name  string bo be transferred
  @func: Transfer string to integer
  @return  int  the result
******************************************/
static int atoi(const char *name)
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

/*****************************************
  @func_name:  split
  @para:  src: source string
          separator:  the separator string
		  dest: store the split result strings
		  num: split count
  @func:   According to separator string to Split string
  @return  void
******************************************/
void split(char *src, const char *separator, char **dest, int *num)
{
    char *pNext;
    int count = 0;
    
    if (src == NULL || strlen(src) == 0) return;
    if (separator == NULL || strlen(separator) == 0) return; 

    while((pNext = strsep(&src,separator)) != NULL)
    {
        *dest++ = pNext;
        count++;
    }

    *num = count;
}

/*****************************************
  @func_name:  write_data_to_file
  @para: char* filename  
  @func: write the test data to the file
  @return  int  success:0,fail:-1
******************************************/
static int write_data_to_file(char* filename)
{
    mm_segment_t oldfs;
    struct file	*filp;
    uint16_t	length;

    length = strlen(report_time_buffer) +1 ;
    printk("%s: write_data_to_file: length %d\n", __FUNCTION__,length);

    oldfs = get_fs();
    set_fs(KERNEL_DS);
    filp = filp_open(filename, O_RDWR | O_CREAT |O_APPEND, 0);
    if (IS_ERR(filp))
    {
        printk("%s: file %s filp_open error\n", __FUNCTION__,filename);
        set_fs(oldfs);
        return -1;
    }


    if (!filp->f_op)
    {
        printk("%s: File Operation Method Error\n", __FUNCTION__);
        filp_close(filp, NULL);
        set_fs(oldfs);
        return -1;
    }

    /* write data */
    if (filp->f_op->write(filp, report_time_buffer, length, &filp->f_pos) != length)
    {
        printk("%s: file write error\n", __FUNCTION__);
        filp_close(filp, NULL);
        set_fs(oldfs);
        return -1;
    }
    
    filp_close(filp, NULL);
    set_fs(oldfs);
    return 0;
}

/*****************************************
  @func_name:  read_data_from_file
  @para:  filename: write efs  path
  @func:   Read data from file
  @return  char*   sucess:return the data read from file,faled:NULL
******************************************/
static char* read_data_from_file(char* filename)
{
    mm_segment_t oldfs;
    struct file	*filp;
    uint32_t	length;
    struct inode *inode = NULL;
	char* buffer = NULL;
    
    oldfs = get_fs();
    set_fs(KERNEL_DS);
    filp = filp_open(filename, O_RDONLY, 0);
    if (IS_ERR(filp))
    {
        printk("%s: file %s filp_open error\n", __FUNCTION__,filename);
        set_fs(oldfs);
        return NULL;
    }


    if (!filp->f_op)
    {
        printk("%s: File Operation Method Error\n", __FUNCTION__);
        filp_close(filp, NULL);
        set_fs(oldfs);
        return NULL;
    }

    inode = filp->f_path.dentry->d_inode;
    if (!inode) 
    {
        printk("%s: Get inode from filp failed\n", __FUNCTION__);
        filp_close(filp, NULL);
        set_fs(oldfs);
        return NULL;
    }
    
    /* file's size */
    length = i_size_read(inode->i_mapping->host);
    

	
    /* allocation buff size */
    buffer = vmalloc(length+1);
    if (!buffer) 
    {
    	printk("alloctation memory failed\n");
    	filp_close(filp, NULL);
    	set_fs(oldfs);
    	return NULL;
    }

    memset((void*)buffer,0,length+1);

    /* read data */
    if (filp->f_op->read(filp, buffer, length, &filp->f_pos) != length)
    {
        printk("%s: file read error\n", __FUNCTION__);
        filp_close(filp, NULL);
        set_fs(oldfs);
        vfree(test_touch_buffer);
        return NULL;
    }

    filp_close(filp, NULL);
    set_fs(oldfs);
    return buffer;
}

/*****************************************
  @func_name:  synaptics_rmi4_work_touch_simulation
  @para: 
  @func:   parse the file touch_data.txt from the efs and 
            simulate some touch events report,and save the
			touch event and report time to file
  @return  void
******************************************/
static void synaptics_rmi4_work_touch_simulation(void)
{
    char *filename_freq = "/data/performancetest/testdata/tp_freq.dat";
    char *filename = "/data/performancetest/testdata/touch_data.txt";
    char *write_filename = "/data/performancetest/1_time_kenel_report.txt";
    int i = 0;
    int num = 0;

    char *subdest[20];
    char *subsubdest[10];
    int subnum = 0;
    int subsubnum = 0;
    u8 report_type = EV_CNT;
    u32 report_code = 0;
    int report_val = 0;
    int j = 0;

    long report_pointer_time_ms = 0;
    struct timespec ts;
    static unsigned int buffer_point_count = 0;
    char* freq_data = NULL;
    int freq_val = 0;
    int sleep_time = 0;

    /* Read the TP freq from file tp_freq.dat */
    freq_data = read_data_from_file(filename_freq);
    if(NULL != freq_data)
    {
        freq_val = atoi(freq_data);
        vfree(freq_data);
    }
    else
    {
        /* read file failed ,set the default value is 80 HZ*/
        freq_val = TOUCH_DEFAULT_REPORT_FREQ;
        printk("CZL_TEST read tp_freq.dat is failed\n");
    }

    /* caculate sleep time */
    sleep_time = 1000*1000/freq_val;

    /* Read the test data from the file touch_data.txt */
    test_touch_buffer = read_data_from_file(filename);
    if(test_touch_buffer == NULL)
    {
        return;
    }
    
    x_coordinate = -1;
    y_coordinate = -1;
    memset(dest_data,0,sizeof(dest_data));
    memset(subdest,0,sizeof(subdest));
    memset(subsubdest,0,sizeof(subsubdest));
	
	/* Split each "#" ,each item represent a pointer,include pressure,x,y,... */
    split(test_touch_buffer, "#", dest_data, &num);

    memset(report_time_buffer,0,REPORT_DATA_BUFFER_SIZE);
    buffer_point_count = 0;
    
	/* parse each pointer */
    for(i = 0 ; i < num; i++)
    {
         ktime_get_ts(&ts);
         report_pointer_time_ms = ts.tv_sec*1000 + ts.tv_nsec/1000000;
		 
		 /* split each "\n",each item represent an event,include event type ,code ,value */
         split(dest_data[i], "\n",subdest,&subnum);
		 
		 /* parse each event and report */
	     for(j = 1; j  < subnum; j++)
	     {
	         int subsubdest_len = 0;
			 
			 /* Split each " " */
             split(subdest[j]," ",subsubdest,&subsubnum);
			 
             subsubdest_len = strlen(subsubdest[2]);
             subsubdest[2][subsubdest_len-1] = '\0';
		     report_type = hextodec(subsubdest[0]);
             report_code = hextodec(subsubdest[1]);
             report_val = hextodec(subsubdest[2]);
			 
             /* save X and Y to report */
              if((EV_ABS == report_type) && (ABS_MT_POSITION_X == report_code))
              {
                 x_coordinate = report_val;
              }

              if((EV_ABS == report_type) && (ABS_MT_POSITION_Y== report_code))
              {
                 y_coordinate = report_val;
              }
			  
              /* report event  */ 
              input_event(simulation_touch_dev,report_type,report_code,report_val);
	     }

	     sprintf((report_time_buffer+buffer_point_count),"%d%s%d%s%d%s%ld%s",\
		          i+1," ",x_coordinate," ",y_coordinate," ",report_pointer_time_ms,"\n");
	     buffer_point_count = strlen(report_time_buffer);

	     usleep(sleep_time);
    }

    /*  Save the result data to file /data/performancetest/1_time_kenel_report.txt */
    (void)write_data_to_file(write_filename);
    vfree(test_touch_buffer);
}

static ssize_t touch_test_show(struct kobject *kobj,struct kobj_attribute *attr,char *buf)
{
    return 1;
}

static ssize_t touch_test_store(struct kobject *kobj,struct kobj_attribute *attr,const char *buf,size_t count)
{
	printk("#################touch_test_store######################\n");
	if((buf[0] == '1') && (buf[1] == '\0'))
	{
	  printk(KERN_ERR "Start touch test \n");
	  display_set_record_start(1);

	  synaptics_rmi4_work_touch_simulation();
	}
	else
	{
		display_set_record_start(0);
	}
	return 1;
}

/*****************************************
  @func_name:  simulation_touch_test_init
  @para: 
  @func:   initialize simulation_touch_test driver  
  @return  int 
******************************************/
static int __init simulation_touch_test_init(void)
{
	int rc=0;
	int lcd_x = 0;
    int lcd_y = 0;
	struct tp_resolution_conversion tp_type_self_check = {0};

	/* Create an input device  */
	simulation_touch_dev = input_allocate_device();
	if (!simulation_touch_dev)
	{
		printk(KERN_ERR "failed to allocate input device.\n");
		return -1;
	}

    /* Set the parameters of the input device */
	simulation_touch_dev->name = "simulation_touch_test";

	set_bit(EV_ABS, simulation_touch_dev->evbit);
	set_bit(EV_SYN, simulation_touch_dev->evbit);
	set_bit(EV_KEY, simulation_touch_dev->evbit);
	set_bit(BTN_TOUCH, simulation_touch_dev->keybit);
	set_bit(ABS_X, simulation_touch_dev->absbit);
	set_bit(ABS_Y, simulation_touch_dev->absbit);
	set_bit(KEY_NUMLOCK, simulation_touch_dev->keybit);
	set_bit(INPUT_PROP_DIRECT,simulation_touch_dev->propbit);

	/* register a input device */
	rc = input_register_device(simulation_touch_dev);
	if (rc) 
	{
		printk(KERN_ERR "simulation_touch_test_init Unable to register %s \
				input device\n", simulation_touch_dev->name);
		return rc;
	} 
	else 
	{
		printk(KERN_ERR"simulation_touch_test input device registered\n");
	}

    /* get the touch resolution */
	rc = get_touch_resolution(&tp_type_self_check);
	if(rc < 0)
	{
		printk(KERN_ERR "get_touch_resolution failed \n");
		return rc;
	}
	else
	{
		lcd_x = tp_type_self_check.lcd_x;
		lcd_y = tp_type_self_check.lcd_y;
		printk(KERN_ERR "lcd_x = %d,lcd_y=%d \n",lcd_x,lcd_y);
	}
	
	/* Set what event the input device supports */
	input_set_abs_params(simulation_touch_dev, ABS_MT_POSITION_X, 0, lcd_x - 1, 0, 0);
	input_set_abs_params(simulation_touch_dev, ABS_MT_POSITION_Y, 0, lcd_y - 1, 0, 0);
	input_set_abs_params(simulation_touch_dev, ABS_MT_TOUCH_MAJOR, 0, 0xF, 0, 0);
	input_set_abs_params(simulation_touch_dev, ABS_MT_TOUCH_MINOR, 0, 0xF, 0, 0);
	input_set_abs_params(simulation_touch_dev, ABS_MT_ORIENTATION, 0, 1, 0, 0);
	input_set_abs_params(simulation_touch_dev, ABS_MT_PRESSURE, 0,	255, 0, 0);
	input_set_abs_params(simulation_touch_dev, ABS_X, 0, lcd_x-1, 0, 0);
	input_set_abs_params(simulation_touch_dev, ABS_Y, 0, lcd_y-1, 0, 0);
	input_set_abs_params(simulation_touch_dev, ABS_PRESSURE, 0, 255, 0, 0);

	/* Create test attribute note*/
	kobject_ts = kobject_create_and_add("touch_screen_test", NULL);
	if (!kobject_ts)
	{
		printk(KERN_ERR"create kobjetct error!\n");
		return -1;
	}
	rc = sysfs_create_file(kobject_ts,&touch_test_attribute.attr);
	if(rc)
	{
		kobject_put(kobject_ts);
		printk(KERN_ERR"create touch test flag file error\n");
		return -1;
	}
	return rc;
}

/*****************************************
  @func_name:  simulation_touch_test_exit
  @para: 
  @func:   uninstall the  simulation_touch_test driver  
  @return  int 
******************************************/
static void __exit simulation_touch_test_exit(void)
{
    input_unregister_device(simulation_touch_dev);
    input_free_device(simulation_touch_dev);
    sysfs_remove_file(kobject_ts,&touch_test_attribute.attr);
    kobject_put(kobject_ts);
    return;
}

module_init(simulation_touch_test_init);
module_exit(simulation_touch_test_exit);
