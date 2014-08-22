/* kernel\drivers\power\hw_bms_battery_debug.c
 * this file is used by bms to get battery data from  /data/battery_debug.txt
 * 
 * Copyright (C) 2012 HUAWEI Technology Co., BMS.
 * 
 * Date: 2012/08/09
 * By sunchenggang
 * 
 */

#include "hw_bms_battery_debug.h"

int g_buf_size = 0; /* record all read data size for get line */

/*============================================================================================
FUNCTION        fget_line

DESCRIPTION    This function is called for get one line from the buff and the line end with '\n'
CALLED BY       all config table

RETURN VALUE    success get line words count
DEPENDENCIES : none

SIDE EFFECTS : none

=============================================================================================*/
int  fget_line(char * buf,char * line,char ** ptp)
{
	char * src = buf;
	char * dest = line;
	unsigned int read_size = 0;
	if (NULL==buf)
	{
		printk("buf is null\n");
		return 0;
	}
	if (NULL==line)
	{
		printk("line is null\n");
		return 0;
	}

	/* the line must be end with '\n' */
	while('\0' != *src)
	{
		if(((src[0] == '\r')&&(src[1] == '\n'))||(src[0] == '\n'))
		{
			break;
		}
		*dest = *src;
		 dest++;
		  src++;
		  read_size++;
	}
	if ('\n' == *src)
	{
		*dest = '\0';
		read_size++;
		*ptp = ++src;
	}
	/* windows \r\n = linux \n  */
	else if ((src[0] == '\r')&&(src[1] == '\n'))
	{
		*dest = '\0';
		read_size = read_size + 2;
		*ptp = src+2;
		printk("there is online \\r\\n \n");
	}
	/* this is the last line  */
	else if ('\0'==*src)
	{
		*dest = '\0';
		read_size++;
		*ptp = NULL;
	}
	else
	{
		read_size = 0;
		*ptp = NULL;
		printk("the file format error ,there is no end \n");
	}
	g_buf_size  = g_buf_size + read_size;
	return read_size;
}
/*============================================================================================
FUNCTION        get_single_number

DESCRIPTION    This function is called for config one line that include one number
CALLED BY       all config table

RETURN VALUE    none
DEPENDENCIES : none

SIDE EFFECTS : none

=============================================================================================*/
bool get_single_number(int *x,char* line)
{
	char *p = line;
	int val = 0;

	/* if the first word is not digital ,the data is wrong */
	if((line[0] < '0')&&(line[0] > '9'))
	{
		printk("This single number no digital\n");
		return FALSE;
	}
	while(*p != '\0')
	{
		 switch (*p)
		 {
			case '0' ... '9':
				val = 10*val+(*p-'0');
				break;
			case ',':
				*x = val ;
				/* if , is not the last word ,the data is wrong */
				if(p[1]!='\0')
				{
					printk("This are more than one digital\n");
					return FALSE;
				}
				break;
			default:
				printk("This is illegal char\n");
				return FALSE;
		}
		p++;
	}
	*x = val ;
	return TRUE;
}

/*============================================================================================
FUNCTION        get_array_number

DESCRIPTION    This function is called for config one line that include more than one numbers
CALLED BY       all config table

RETURN VALUE    none
DEPENDENCIES : none

SIDE EFFECTS : none

=============================================================================================*/
bool get_array_number(int *x,char* line,int *num,int max_num)
{
	char *p = line;
	int val = 0;
	int array_num =0;
	bool plus = TRUE;

	while(*p != '\0')
	{
		 switch (*p)
		{
			case '{':
				break;
			/* if it is a negative digital ,set the plus flag FALSE */
			case '-':
				plus = FALSE;
				break;
			case '0' ... '9':
				val = 10*val+(*p-'0');
				break;
			case ',':
				if (plus == FALSE)
				{
					/*  negative digital = 0 -val */
					val = 0 - val;
					plus = TRUE;
				}
				if (array_num >= max_num)
				{
					printk(",:array_num=%d,max_num =%d\n",array_num,max_num);
					return FALSE;
				}
				x[array_num++] = val ;
				val = 0;
				break;
			case '}':
				if (plus == FALSE)
				{
					/*  negative digital = 0 -val */
					val = 0 - val;
					plus = TRUE;
				}
				if (array_num >= max_num)
				{
					printk("}:array_num=%d,max_num =%d\n",array_num,max_num);
					return FALSE;
				}
				x[array_num++] = val ;
				*num = array_num;
				plus = TRUE;
				return TRUE;
			default:
				break;
		}
		p++;
	}

	return TRUE;

}

/* for example
*static struct single_row_lut palladium_1500_fcc_temp = {
*	.x	= {-30, -20, -10, 0, 10, 25, 40, 60},
*	.y	= {1103, 1179, 1284, 1330, 1420, 1511, 1541, 1571},
*	.cols	= 8,
*};
**/
/*============================================================================================
FUNCTION        config_table_fcc_temp_or_sf_lut

DESCRIPTION     This function is called for configing the palladium_1500_fcc_temp table or palladium_1500_fcc_sf table
CALLED BY       set_config_data

RETURN VALUE    TRUE :config tabel success
                             FALSE:config table failed
DEPENDENCIES : none

SIDE EFFECTS : none

=============================================================================================*/

bool  config_table_fcc_temp_or_sf_lut(struct single_row_lut	*fcc_temp_or_sf_lut,char *buf,char **ptp,bool *nodata)
{
	char line[HW_READ_LINE_MAX_LENGTH];
	char * p = buf;
	int numx = 0;
	int numy = 0;
	int get_ret = 0;
	bool b_ret = FALSE;

	if (NULL == fcc_temp_or_sf_lut)
	{
		printk(KERN_ERR "%s: fcc_temp_lut  null\n",__func__);
		return FALSE;
	}
	if (NULL == buf)
	{
		printk(KERN_ERR "%s: buf  null\n",__func__);
		return FALSE;
	}
	
	/* one line at least two words */
	get_ret = fget_line(p,line,&p);
	if (get_ret < 1)
	{
		printk(KERN_ERR "%s: lut->x line error\n",__func__);
		return FALSE;
	}
	/*if no data,the first character is n*/
	if ('n' == line[0])
	{
		fcc_temp_or_sf_lut = NULL;
		*ptp = p;
		*nodata = TRUE;
		printk(KERN_ERR "no fcc_temp_or_sf_lut\n");
		return TRUE;
	}
	/* get  x */
	b_ret = get_array_number(fcc_temp_or_sf_lut->x,line,&numx,MAX_SINGLE_LUT_COLS);
	if (FALSE == b_ret)
	{
		printk("fcc_temp_or_sf_lut->x number wrong\n");
		return FALSE;
	}
	get_ret = fget_line(p,line,&p);
	if (get_ret < 1)
	{
		printk(KERN_ERR "%s: lut->y line error\n",__func__);
		return FALSE;
	}

	/* get  y */
	b_ret = get_array_number(fcc_temp_or_sf_lut->y,line,&numy,MAX_SINGLE_LUT_COLS);
	if (FALSE == b_ret)
	{
		printk("fcc_temp_or_sf_lut->y number wrong\n");
		return FALSE;
	}

	get_ret = fget_line(p,line,&p);
	if (get_ret < 1)
	{
		printk(KERN_ERR "%s: lut->cols line error\n",__func__);
		return FALSE;
	}
	/* get  cols */
	b_ret = get_single_number(&fcc_temp_or_sf_lut->cols,line);
	if (FALSE == b_ret)
	{
		printk("fcc_temp_or_sf_lut->cols number wrong\n");
		return FALSE;
	}
	/* if the x numbers not equal to cols or the y numbers not equal to rows ,the data is wrong */
	if ((numx != fcc_temp_or_sf_lut->cols)||(numy != fcc_temp_or_sf_lut->cols))
	{
		printk(KERN_ERR "%s: numx =%d,numy =%d,cols =%d\n",__func__,numx,numy, fcc_temp_or_sf_lut->cols);
		return FALSE;
	}

	/* set the next table start poniter */
	*ptp = p;
	
	return TRUE;
	
}

/* for example
*static struct sf_lut palladium_1500_pc_sf = {
*	.rows		= 10,
*	.cols		= 5,
*	.row_entries	= {100, 200, 300, 400, 500},
*	.percent	= {100, 90, 80, 70, 60, 50, 40, 30, 20, 10},
*	.sf		= {
*			{97, 93, 93, 90, 87},
*			{97, 93, 93, 90, 87},
*			{98, 94, 92, 89, 86},
*			{98, 94, 92, 89, 86},
*			{99, 94, 92, 88, 86},
*			{99, 95, 92, 88, 87},
*			{99, 95, 92, 88, 87},
*			{99, 95, 92, 88, 87},
*			{99, 95, 92, 88, 87},
*			{99, 95, 92, 88, 87}
*	},
*};
*/
/*============================================================================================
FUNCTION        config_table_pc_or_rabatt_sf_lut

DESCRIPTION     This function is called for configing the palladium_1500_rbatt_sf table or palladium_1500_pc_sf table
CALLED BY       set_config_data

RETURN VALUE    TRUE :config tabel success
                             FALSE:config table failed
DEPENDENCIES : none

SIDE EFFECTS : none

=============================================================================================*/
bool  config_table_pc_or_rabatt_sf_lut(struct sf_lut *pc_or_rabatt_sf_lut, char *buf, char **ptp,bool *nodata)
{
	char line[HW_READ_LINE_MAX_LENGTH];
	char * p = buf;
	int numx = 0;
	int numy = 0;
	int get_ret = 0;
	int i = 0;
	bool b_ret = FALSE;
	
	/* one line at least two words */
	get_ret = fget_line(p,line,&p);
	if (get_ret < 1)
	{
		printk(KERN_ERR "%s:sf_lut->rows line error\n",__func__);
		return FALSE;
	}
	if ('n' == line[0])
	{
		pc_or_rabatt_sf_lut = NULL;
		printk(KERN_ERR "no pc_or_rabatt_sf_lut2\n");
		*ptp = p;
		*nodata = TRUE;
		return TRUE;
	}
	/* get  rows */
	b_ret = get_single_number(&pc_or_rabatt_sf_lut->rows,line);
	if (FALSE == b_ret)
	{
		printk("pc_or_rabatt_sf_lut->rows number wrong\n");
		return FALSE;
	}
	get_ret = fget_line(p,line,&p);
	if (get_ret < 1)
	{
		printk(KERN_ERR "%s:sf_lut->cols line error\n",__func__);
		return FALSE;
	}

	/* get  cols */
	b_ret = get_single_number(&pc_or_rabatt_sf_lut->cols,line);
	if (FALSE == b_ret)
	{
		printk("pc_or_rabatt_sf_lut->cols number wrong\n");
		return FALSE;
	}
	get_ret = fget_line(p,line,&p);
	if (get_ret < 1)
	{
		printk(KERN_ERR "%s:sf_lut->row_entries line error\n",__func__);
		return FALSE;
	}

	/* get  row_entries */
	b_ret = get_array_number(pc_or_rabatt_sf_lut->row_entries,line,&numx,PC_CC_COLS);
	if (FALSE == b_ret)
	{
		printk("pc_or_rabatt_sf_lut->row_entries number wrong\n");
		return FALSE;
	}
	get_ret = fget_line(p,line,&p);
	if (get_ret < 1)
	{
		printk(KERN_ERR "%s:sf_lut->percent line error\n",__func__);
		return FALSE;
	}

	/* get  percent */
	b_ret = get_array_number(pc_or_rabatt_sf_lut->percent,line,&numy,PC_CC_ROWS);
	if (FALSE == b_ret)
	{
		printk("pc_or_rabatt_sf_lut->percent number wrong\n");
		return FALSE;
	}
	/* if the row_entries not equal to cols or the percent numbers not equal to rows ,the data is wrong */
	if ((numx != pc_or_rabatt_sf_lut->cols)||(numy != pc_or_rabatt_sf_lut->rows))
	{
		printk(KERN_ERR "%s: numx =%d,numy =%d,cols =%d,rows =%d\n",__func__,numx,numy, pc_or_rabatt_sf_lut->cols,pc_or_rabatt_sf_lut->rows);
		return FALSE;
	}

	/* there must be a { before multi array */
	get_ret = fget_line(p,line,&p);
	if ((get_ret < 1)||(line[0]!='{'))
	{
		printk(KERN_ERR "%s:the array first line must be {\n",__func__);
		return FALSE;
	}

	for (i = 0;i  <= pc_or_rabatt_sf_lut->rows -1;i++)
	{
		get_ret = fget_line(p,line,&p);
		if (get_ret < 1)
		{
			printk(KERN_ERR "%s:sf_lut->sf line %d error\n",__func__,i);
			return FALSE;
		}
		
		/* ervery lines number must equal to cols */
		b_ret = get_array_number(&pc_or_rabatt_sf_lut->sf[i][0],line,&numx,PC_CC_COLS);
		if (FALSE == b_ret)
		{
			printk("sf_lut->sf[%d] number wrong\n",i);
			return FALSE;
		}
		if (numx != pc_or_rabatt_sf_lut->cols)
		{
			printk(KERN_ERR "%s:sf_lut->sf[%d] line,numx =%d,cols =%d\n",__func__,i,numx, pc_or_rabatt_sf_lut->cols);
			return FALSE;
		}
	}

	/* there must be a } after multi array */
	get_ret = fget_line(p,line,&p);
	if ((get_ret < 1)||(line[0]!='}'))
	{
		printk(KERN_ERR "%s:the array end line must be {\n",__func__);
		return FALSE;
	}

	/* set the next table start poniter */
	*ptp = p;

	return TRUE;
	
}

/* for example
static struct pc_temp_ocv_lut palladium_1500_pc_temp_ocv = {
	.rows		= 29,
	.cols		= 8,
	.temp		= {-30, -20, -10, 0, 10, 25, 40, 60},
	.percent	= {100, 95, 90, 85, 80, 75, 70, 65, 60, 55,50, 45, 40, 35, 30, 25, 20, 15, 10, 9,8, 7, 6, 5, 4, 3, 2, 1, 0},
	.ocv		= {
			{3673, 3814, 3945, 4025, 4106, 4176, 4218, 4260},
			{3613, 3751, 3880, 3959, 4038, 4107, 4149, 4190},
			{3573, 3710, 3837, 3916, 3994, 4062, 4103, 4144},
			{3534, 3670, 3796, 3873, 3951, 4019, 4059, 4099},
			{3491, 3625, 3749, 3826, 3902, 3969, 4009, 4049},
			{3464, 3597, 3721, 3796, 3872, 3939, 3978, 4018},
			{3436, 3568, 3691, 3766, 3841, 3907, 3946, 3985},
			{3407, 3537, 3659, 3733, 3808, 3873, 3912, 3951},
			{3377, 3507, 3627, 3701, 3775, 3840, 3878, 3917},
			{3355, 3484, 3604, 3677, 3751, 3815, 3853, 3891},
			{3339, 3467, 3586, 3659, 3732, 3796, 3834, 3872},
			{3324, 3452, 3570, 3643, 3716, 3780, 3818, 3855},
			{3312, 3440, 3558, 3630, 3703, 3766, 3804, 3842},
			{3303, 3430, 3548, 3620, 3692, 3756, 3793, 3831},
			{3297, 3424, 3541, 3614, 3686, 3749, 3787, 3824},
			{3288, 3414, 3531, 3603, 3675, 3738, 3776, 3813},
			{3272, 3398, 3514, 3586, 3658, 3720, 3757, 3795},
			{3240, 3365, 3480, 3551, 3622, 3684, 3721, 3758},
			{3224, 3348, 3463, 3533, 3604, 3666, 3702, 3739},
			{3221, 3344, 3459, 3530, 3600, 3662, 3695, 3728},
			{3216, 3340, 3454, 3525, 3595, 3657, 3686, 3715},
			{3212, 3335, 3449, 3520, 3590, 3652, 3677, 3703},
			{3203, 3326, 3440, 3510, 3580, 3642, 3664, 3686},
			{3185, 3307, 3420, 3490, 3560, 3621, 3639, 3657},
			{3176, 3298, 3411, 3481, 3550, 3611, 3626, 3640},
			{3151, 3272, 3384, 3453, 3522, 3583, 3593, 3604},
			{3106, 3225, 3335, 3446, 3472, 3531, 3538, 3545},
			{3021, 3217, 3245, 3417, 3429, 3435, 3439, 3442},
			{3000, 3000, 3000, 3000, 3000, 3000, 3000, 3000}
	},
};
*/
/*============================================================================================
FUNCTION        config_table_pc_temp_ocv_lut

DESCRIPTION     This function is called for configing the pc_temp_ocv_lut table
CALLED BY       set_config_data

RETURN VALUE    TRUE :config tabel success
                FALSE:config table failed
DEPENDENCIES : none

SIDE EFFECTS : none

=============================================================================================*/
bool  config_table_pc_temp_ocv_lut(	struct pc_temp_ocv_lut * pc_temp_ocv_lut,char *buf,char **ptp,bool *nodata)
{
	char line[HW_READ_LINE_MAX_LENGTH];
	char * p = buf;
	int numx = 0;
	int numy = 0;
	int get_ret = 0;
	int i = 0;
	bool b_ret = FALSE;
	
	/* one line at least two words */
	get_ret = fget_line(p,line,&p);
	if (get_ret < 1)
	{
		printk(KERN_ERR "%s:pc_temp_ocv_lut->rows line error\n",__func__);
		return FALSE;
	}
	if ('n' == line[0])
	{
		pc_temp_ocv_lut = NULL;
		*ptp = p;
		*nodata = TRUE;
		printk(KERN_ERR "no pc_temp_ocv_lut\n");
		return TRUE;
	}
	/* get  rows*/
	b_ret = get_single_number(&pc_temp_ocv_lut->rows,line);
	if (FALSE == b_ret)
	{
		printk("pc_temp_ocv_lut->rows number wrong\n");
		return FALSE;
	}
	get_ret = fget_line(p,line,&p);
	if (get_ret < 1)
	{
		printk(KERN_ERR "%s:pc_temp_ocv_lut->cols line error\n",__func__);
		return FALSE;
	}
	
	/* get  cols*/
	b_ret = get_single_number(&pc_temp_ocv_lut->cols,line);
	if (FALSE == b_ret)
	{
		printk("pc_temp_ocv_lut->cols number wrong\n");
		return FALSE;
	}
	get_ret = fget_line(p,line,&p);
	if (get_ret < 1)
	{
		printk(KERN_ERR "%s:pc_temp_ocv_lut->temp line error\n",__func__);
		return FALSE;
	}
	
	/* get  temp*/
	b_ret = get_array_number(pc_temp_ocv_lut->temp,line,&numx,PC_TEMP_COLS);
	if (FALSE == b_ret)
	{
		printk("pc_temp_ocv_lut->temp number wrong\n");
		return FALSE;
	}
	get_ret = fget_line(p,line,&p);
	if (get_ret < 1)
	{
		printk(KERN_ERR "%s:pc_temp_ocv_lut->percent line error\n",__func__);
		return FALSE;
	}

	/* get  percent*/
	b_ret = get_array_number(pc_temp_ocv_lut->percent,line,&numy,PC_TEMP_ROWS);
	if (FALSE == b_ret)
	{
		printk("pc_temp_ocv_lut->percent number wrong\n");
		return FALSE;
	}
	/* if the temp numbers not equal to cols or the percent numbers not equal to rows ,the data is wrong */
	if ((numx != pc_temp_ocv_lut->cols)||(numy != pc_temp_ocv_lut->rows))
	{
		printk(KERN_ERR "%s: numx =%d,numy =%d,cols =%d,rows =%d\n",__func__,numx,numy, pc_temp_ocv_lut->cols,pc_temp_ocv_lut->rows);
		return FALSE;
	}
	
	/* there must be a { before multi array */
	get_ret = fget_line(p,line,&p);
	if ((get_ret < 1)||(line[0]!='{'))
	{
		printk(KERN_ERR "%s:the array first line must be {\n",__func__);
		return FALSE;
	}

	/* the multi array has rows lines */
	for (i = 0;i  <= pc_temp_ocv_lut->rows -1;i++)
	{
		get_ret = fget_line(p,line,&p);
		if (get_ret < 1)
		{
			printk(KERN_ERR "%s:pc_temp_ocv_lut->ocv line %d error\n",__func__,i);
			return FALSE;
		}

		b_ret = get_array_number(&pc_temp_ocv_lut->ocv[i][0],line,&numx,PC_TEMP_COLS);
		if (FALSE == b_ret)
		{
			printk("pc_temp_ocv_lut->ocv[%d] number wrong\n",i);
			return FALSE;
		}
		/* ervery lines number must equal to cols */
		if (numx != pc_temp_ocv_lut->cols)
		{
			printk(KERN_ERR "%s:pc_temp_ocv_lut->ocv[%d] line,numx =%d,cols =%d\n",__func__,i,numx, pc_temp_ocv_lut->cols);
			return FALSE;
		}
	}

	/* there must be a } after multi array */
	get_ret = fget_line(p,line,&p);
	if ((get_ret < 1)||(line[0]!='}'))
	{
		printk(KERN_ERR "%s:the array end line must be {\n",__func__);
		return FALSE;
	}

	/* set the next table start poniter */
	*ptp = p;
	
	return TRUE;
	
}
/*============================================================================================
FUNCTION        config_table_other

DESCRIPTION     This function is called for configing the last three lines fcc /
                           default_rbatt_mohm/delta_rbatt_mohm
CALLED BY       set_config_data

RETURN VALUE    TRUE :config tabel success
                FALSE:config table failed
DEPENDENCIES : none

SIDE EFFECTS : none

=============================================================================================*/
/* modify for 1.7232 baseline upgrade */
bool  config_table_other(struct bms_battery_data* battery_table,char *buf,char **ptp)
{
	char line[HW_READ_LINE_MAX_LENGTH];
	char * p = buf;
	int get_ret = 0;
	bool b_ret = FALSE;
	get_ret = fget_line(p,line,&p);
	if (get_ret < 1)
	{
		printk(KERN_ERR "%s:battery_table->fcc line error\n",__func__);
		return FALSE;
	}
	if ('n' == line[0])
	{
		battery_table->fcc = 0;
		printk(KERN_ERR "no fcc\n");
	}
	else
	{
		/* get fcc */
		b_ret = get_single_number(&battery_table->fcc,line);
		if (FALSE == b_ret)
		{
			printk("battery_table->fcc number wrong\n");
			return FALSE;
		}
	}
	get_ret = fget_line(p,line,&p);
	if (get_ret < 1)
	{
		printk(KERN_ERR "%s:battery_table->default_rbatt_mohm line error\n",__func__);
		return FALSE;
	}
	if ('n' == line[0])
	{
		battery_table->default_rbatt_mohm = 0;
		printk(KERN_ERR "no debugdefault_rbatt_mohm\n");
	}
	else
	{
		/* get default_rbatt_mohm */
		b_ret = get_single_number(&battery_table->default_rbatt_mohm,line);
		if (FALSE == b_ret)
		{
			printk("battery_table->default_rbatt_mohm number wrong\n");
			return FALSE;
		}
	}

	get_ret = fget_line(p,line,&p);
	if (get_ret < 1)
	{
		printk(KERN_ERR "%s:battery_table->delta_rbatt_mohm line error\n",__func__);
		return FALSE;
	}
	if ('n' == line[0])
	{
		battery_table->delta_rbatt_mohm = 0;
		printk(KERN_ERR "no delta_rbatt_mohm\n");
	}
	else
	{
		/* get delta_rbatt_mohm */
		b_ret = get_single_number(&battery_table->delta_rbatt_mohm,line);
		if (FALSE == b_ret)
		{
			printk("battery_table->delta_rbatt_mohm number wrong\n");
			return FALSE;
		}
	}
	/* the last line must be end at \n */
       get_ret = fget_line(p,line,&p);
	if ((get_ret != 1)||(line[0]!='\0'))
	{
		printk(KERN_ERR "%s:there must be \\n a  at end line and the last word is 0,get_ret=%d\n",__func__,get_ret);
		return FALSE;
	}
	
	/* set the next table start poniter */
	*ptp = NULL;
	return TRUE;
	
}
/*============================================================================================
FUNCTION        set_config_data

DESCRIPTION     This function is called for configing five talbes and all success return ture.
CALLED BY       bms_battery_debug_malloc_get_para

RETURN VALUE    TRUE :config tabel success
                FALSE:config table failed
DEPENDENCIES : none

SIDE EFFECTS : none

=============================================================================================*/
bool set_config_data(struct bms_battery_data* debug_battery_table,char *read_buf,int max_size)
{

	char * p_current = read_buf;
	bool ret = FALSE;
	bool nodata = FALSE;

	/* config the fcc temp table */
	printk("%s: start config fcc_temp_lut table \n",__func__);
	ret = config_table_fcc_temp_or_sf_lut(debug_battery_table->fcc_temp_lut,p_current,&p_current,&nodata);
	if (FALSE == ret)
	{
		printk(KERN_ERR "%s: config_table_fcc_temp_lut  failed\n",__func__);
		return FALSE;
	}
	else if(TRUE == nodata)
	{
		debug_battery_table->fcc_temp_lut = NULL;
	}

	/* config the fcc sf table */
	printk("%s: start config fcc_sf_lut table \n",__func__);
	nodata = FALSE; 
	ret = config_table_fcc_temp_or_sf_lut(debug_battery_table->fcc_sf_lut,p_current,&p_current,&nodata);

	if (FALSE == ret)
	{
		printk(KERN_ERR "%s: config_table_fcc_sf_lut  failed\n",__func__);
		return FALSE;
	}
	else if(TRUE == nodata)
	{
		debug_battery_table->fcc_sf_lut = NULL;
	}

	/* config the pc sf table */
	printk("%s: start config pc_sf_lut table \n",__func__);
	nodata = FALSE; 
	ret = config_table_pc_or_rabatt_sf_lut(debug_battery_table->pc_sf_lut,p_current,&p_current,&nodata);

	if (FALSE == ret)
	{
		printk(KERN_ERR "%s: config_table_pc_sf_lut  failed\n",__func__);
		return FALSE;
	}
	else if(TRUE == nodata)
	{
		debug_battery_table->pc_sf_lut = NULL;
	}

	/* config the rbatt sf table */
	printk("%s: start config rbatt_sf_lut table \n",__func__);
	nodata = FALSE; 
	ret = config_table_pc_or_rabatt_sf_lut(debug_battery_table->rbatt_sf_lut,p_current,&p_current,&nodata);
	if (FALSE == ret)
	{
		printk(KERN_ERR "%s: config_table_rbatt_sf_lut  failed\n",__func__);
		return FALSE;
	}
	else if(TRUE == nodata)
	{
		debug_battery_table->rbatt_sf_lut = NULL;
	}

	/* config the pc temp ocv  table */
	printk("%s: start config pc_temp_ocv_lut table \n",__func__);
	nodata = FALSE; 
	ret = config_table_pc_temp_ocv_lut(debug_battery_table->pc_temp_ocv_lut,p_current,&p_current,&nodata);

	if (FALSE == ret)
	{
		printk(KERN_ERR "%s: config_table_pc_temp_ocv_lut  failed\n",__func__);
		return FALSE;
	}
	else if(TRUE == nodata)
	{
		debug_battery_table->pc_temp_ocv_lut = NULL;
	}

	/* config the fcc / default rabtt /delta rabtt  */
	printk("%s: start config other table \n",__func__);
	ret = config_table_other(debug_battery_table,p_current,&p_current);
	if (FALSE == ret)
	{
		printk(KERN_ERR "%s: config_table_other  failed\n",__func__);
		return FALSE;
	}
	return ret ;
	
}
/*============================================================================================
FUNCTION        bms_battery_debug_print

DESCRIPTION    if get battery data success ,print all table data 
CALLED BY       bms_battery_debug_get_para

RETURN VALUE    TRUE :get data success
                FALSE:get data failed
DEPENDENCIES : none

SIDE EFFECTS : none

=============================================================================================*/
void bms_battery_debug_print(struct bms_battery_data* debug_battery_table)
{
	int i = 0;
	int j = 0;
	printk("After config data result is \n");
	/* print fcc_temp table */
	if((debug_battery_table->fcc_temp_lut) != NULL)
	{
		printk("static struct single_row_lut palladium_1500_fcc_temp = {\n");
		printk(".x = {");
		for(i = 0; i < debug_battery_table->fcc_temp_lut->cols ;i++)
		{
			printk("%d,",debug_battery_table->fcc_temp_lut->x[i]);
		}
		printk("}\n");
		printk(".y = {");
		for(i = 0; i < debug_battery_table->fcc_temp_lut->cols ;i++)
		{
			printk("%d,",debug_battery_table->fcc_temp_lut->y[i]);
		}
		printk("}\n");
		printk(".cols = %d\n",debug_battery_table->fcc_temp_lut->cols);
		printk("}\n");
	}
	/* print fcc_sf  table */
	if((debug_battery_table->fcc_sf_lut) != NULL)
	{
		printk("static struct single_row_lut palladium_1500_fcc_sf = {\n");
		printk(".x = {");
		for(i = 0; i < debug_battery_table->fcc_sf_lut->cols ;i++)
		{
			printk("%d,",debug_battery_table->fcc_sf_lut->x[i]);
		}
		printk("}\n");
		printk(".y = {");
		for(i = 0; i < debug_battery_table->fcc_sf_lut->cols; i++)
		{
			printk("%d,",debug_battery_table->fcc_sf_lut->y[i]);
		}
		printk("}\n");
		printk(".cols = %d,\n",debug_battery_table->fcc_sf_lut->cols);
		printk("}\n");
	}
	/* print pc_sf table */
	if((debug_battery_table->pc_sf_lut) != NULL)
	{
		printk("static struct sf_lut palladium_1500_pc_sf = {\n");
		printk(".rows = %d,\n",debug_battery_table->pc_sf_lut->rows);
		printk(".cols = %d,\n",debug_battery_table->pc_sf_lut->cols);
		printk(".row_entries = {");
		for(i = 0; i < debug_battery_table->pc_sf_lut->cols ; i++)
		{
			printk("%d,",debug_battery_table->pc_sf_lut->row_entries[i]);
		}
		printk("}\n");
		printk(".percent = {");
		for(i = 0; i < debug_battery_table->pc_sf_lut->rows; i++)
		{
			printk("%d,",debug_battery_table->pc_sf_lut->percent[i]);
		}
		printk("}\n");
		printk(".sf		= {\n");
		for(i = 0; i < debug_battery_table->pc_sf_lut->rows ; i++)
		{
			printk("{");
			for(j = 0; j < debug_battery_table->pc_sf_lut->cols ;j++)
			{
				printk("%d,",debug_battery_table->pc_sf_lut->sf[i][j]);
			}
			printk("}\n");
		}
		printk("}\n");
		printk("}\n");
	}
	/* print rbatt_sf table */
	if((debug_battery_table->rbatt_sf_lut) != NULL)
	{
		printk("static struct sf_lut palladium_1500_rbatt_sf = {\n");
		printk(".rows = %d,\n",debug_battery_table->rbatt_sf_lut->rows);
		printk(".cols = %d,\n",debug_battery_table->rbatt_sf_lut->cols);
		printk(".row_entries = {");
		for(i = 0; i < debug_battery_table->rbatt_sf_lut->cols ; i++)
		{
			printk("%d,",debug_battery_table->rbatt_sf_lut->row_entries[i]);
		}
		printk("}\n");
		printk(".percent = {");
		for(i = 0; i < debug_battery_table->rbatt_sf_lut->rows; i++)
		{
			printk("%d,",debug_battery_table->rbatt_sf_lut->percent[i]);
		}
		printk("}\n");
		printk(".sf		= {\n");
		for(i = 0; i < debug_battery_table->rbatt_sf_lut->rows; i++)
		{
			printk("{");
			for(j = 0; j < debug_battery_table->rbatt_sf_lut->cols ; j++)
			{
				printk("%d,",debug_battery_table->rbatt_sf_lut->sf[i][j]);
			}
			printk("}\n");
		}
		printk("}\n");
		printk("}\n");
	}
	/* print pc_temp_ocvtable */
	if((debug_battery_table->pc_temp_ocv_lut)!= NULL)
	{
		printk("static struct pc_temp_ocv_lut palladium_1500_pc_temp_ocv = {\n");
		printk(".rows = %d,\n",debug_battery_table->pc_temp_ocv_lut->rows);
		printk(".cols = %d,\n",debug_battery_table->pc_temp_ocv_lut->cols);
		printk(".temp = {");
		for(i = 0; i < debug_battery_table->pc_temp_ocv_lut->cols ; i++)
		{
			printk("%d,",debug_battery_table->pc_temp_ocv_lut->temp[i]);
		}
		printk("}\n");
		printk(".percent = {");
		for(i = 0; i < debug_battery_table->pc_temp_ocv_lut->rows ; i++)
		{
			printk("%d,",debug_battery_table->pc_temp_ocv_lut->percent[i]);
		}
		printk("}\n");
		printk(".ocv		= {\n");
		for(i = 0; i < debug_battery_table->pc_temp_ocv_lut->rows ; i++)
		{
			printk("{");
			for(j = 0; j < debug_battery_table->pc_temp_ocv_lut->cols ; j++)
			{
				printk("%d,",debug_battery_table->pc_temp_ocv_lut->ocv[i][j]);
			}
			printk("}\n");
		}
		printk("}\n");
		printk("}\n");
	}
	
	printk("struct pm8921_bms_battery_data palladium_1500_data = {\n");
	printk(".fcc = %d,\n",debug_battery_table->fcc);
	printk(".default_rbatt_mohm = %d,\n",debug_battery_table->default_rbatt_mohm);
	printk(".delta_rbatt_mohm = %d,\n",debug_battery_table->delta_rbatt_mohm);
	
}
/*============================================================================================
FUNCTION        bms_battery_debug_get_para

DESCRIPTION    This function is called by bms,try to get debug battery data from /data/battery_debug.txt
CALLED BY       set_battery_debug_data

RETURN VALUE    TRUE :get data success
                FALSE:get data failed
DEPENDENCIES : none

SIDE EFFECTS : none

=============================================================================================*/
bool bms_battery_debug_get_para(struct bms_battery_data* debug_battery_table)
{
	struct file * filp = NULL;
	struct inode * inode = NULL;
	mm_segment_t fs = 0;
	off_t fsize = 0;
	char * file_read_buf = NULL;
	unsigned long magic = 0;
	bool ret = FALSE;
	
	if(NULL==debug_battery_table){
		return FALSE;	
	}

	printk("start....open file %s\n",HW_BATTERY_DEBUG_DATA);

	/* open the file HW_BATTERY_DEBUG_DATA */
	filp=filp_open(HW_BATTERY_DEBUG_DATA,O_RDONLY,0);
	if (IS_ERR(filp))
	{
		printk(KERN_ERR "%s: Can not open %s\n",__func__, HW_BATTERY_DEBUG_DATA);
		return FALSE;
	}
	inode=filp->f_dentry->d_inode;  

	magic=inode->i_sb->s_magic;

	printk("file system magic:%li \n",magic);

	printk("super blocksize:%li \n",inode->i_sb->s_blocksize);

	printk("inode %li \n",inode->i_ino);

	fsize=inode->i_size;

	printk("file size:%i \n",(int)fsize);

	/* read the file content */
	file_read_buf = (char *) kmalloc(fsize+1,GFP_ATOMIC);
	if (NULL == file_read_buf)
	{
		printk(KERN_ERR "%s: malloc %d size failed\n",__func__,(int)fsize+1);
		filp_close(filp,NULL);
		return FALSE;
	}

	/* get the old fs */
	fs=get_fs();

	/* set kernel  fs */
	set_fs(KERNEL_DS);

	filp->f_op->read(filp,file_read_buf,fsize,&(filp->f_pos));
	file_read_buf[fsize] = '\0';
	/* set the old fs */
	set_fs(fs);
	
	/* config five tables function */
	ret = set_config_data(debug_battery_table,file_read_buf,fsize);
	if (FALSE == ret)
	{
		printk(KERN_ERR "%s: config data  failed\n",__func__);
		printk("has config data g_buf_size = %d\n",g_buf_size);
		filp_close(filp,NULL);
		kfree(file_read_buf);
		return FALSE;
	}
	
	/* print the table result */
	bms_battery_debug_print(debug_battery_table);
	printk("The File Content is config success,g_buf_size = %d\n",g_buf_size);
	filp_close(filp,NULL);
	kfree(file_read_buf);

	return TRUE;
}
