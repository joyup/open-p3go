/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4740  mobile_tv  Project  V1.0.0
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright (c) Ingenic Semiconductor Co. Ltd 2005. All rights reserved.
 * 
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * http://www.ingenic.cn 
 *
 ********************** END LICENSE BLOCK **************************************
 *
 *  Author:  <dsqiu@ingenic.cn>  <jgao@ingenic.cn> 
 *
 *  Create:   2008-06-26, by dsqiu
 *            
 *  Maintain: 2008-06-26, by jgao
 *            
 *
 *******************************************************************************
 */

#include <jz4755.h>
#include <rtc.h>

#ifdef USE_MIDWARE
#include "os_api.h"
#include "midware.h"
#endif

#define RTC_ALM_READ 0
#define RTC_ALM_SET 1
#define RTC_RD_TIME 2
#define RTC_SET_TIME 3
#define RTC_EPOCH_READ 4
#define RTC_EPOCH_SET 5
  
#define EINVAL 1
#define EACCES 2
#define EFAULT 3

//extern spinlock_t rtc_lock;
static __uint64_t st_time_atom;//get time base from RTC(unit second) 
static __uint64_t st_time_atom_base;//get time base from RTC(unit second) 
static ST_datetime st_EU_time;
static ST_datetime st_EU_base={1900,1,0,1,0,0,0};//2008.02.13
static ST_datetime st_EU_start={1921,2,0,8,0,0,0};//2021.02.8
static ST_datetime st_EU_sample={2009,6,0,29,0,0,0};

static unsigned int u32_RTC_time_l;
static unsigned int u32_RTC_time_h;


//#define AHB_MONITOR_PERIOD	1	// unit: second

#ifdef AHB_MONITOR_PERIOD
	#undef printf
#endif


const unsigned int sum_monthday[13] = {
	0,
	31,
	31+28,
	31+28+31,
	31+28+31+30,
	31+28+31+30+31,
	31+28+31+30+31+30,
	31+28+31+30+31+30+31,
	31+28+31+30+31+30+31+31,
	31+28+31+30+31+30+31+31+30,
	31+28+31+30+31+30+31+31+30+31,
	31+28+31+30+31+30+31+31+30+31+30,
	365
}; //the total day of the last month

void rtc_alarm_stop(void);

const unsigned char monthday[12] = 
{31,28,31,30,31,30,31,31,30,31,30,31};
//1 2  3  4  5  6  7  8  9  10 11 12
//=====================================================================
//function script get the month from yeardays
//input yeardays y(years)
//output return ST_monthday(month and day)
//create date 2008.03.13
//=====================================================================
static void rtc_get_month_from_yeardays(unsigned int yeardays,unsigned short int y,ST_monthday *st_md)
{
	unsigned char m_ret=0,m_f,m_i;
	unsigned int tmpd;
	st_md->u8_month=1;
	st_md->u8_day=1;
	m_f=rtc_is_double_year(y);
	for(m_i=1;m_i<13;m_i++)
	{
		tmpd=sum_monthday[m_i];
		if(m_i>1&&m_f) tmpd+=1;
		if(yeardays<=tmpd) 
		{
			st_md->u8_month=m_i;
			tmpd=sum_monthday[m_i-1];  
			if(m_i>2&&m_f) tmpd+=1;
			st_md->u8_day=yeardays-tmpd;
			break;
		}  
	}//for
	if(13==m_i)
	{
		st_md->u8_month=12;
		tmpd=sum_monthday[12];  
		if(m_f) tmpd+=1;
		st_md->u8_day=yeardays-tmpd;
	}
}
//=====================================================================
//function script get atom time from RTC
//input
//output return a format time
//create date 2008.03.13
//=====================================================================
void atom_time_to_ymd(__uint64_t atom_time ,ST_datetime *st_time)
{
	ST_datetime tmpt;
	unsigned char tmp_i;  
	__uint64_t tmpday,tmphour,tmpminute,tmpd,tmpdd,tmpy;
	unsigned short int tmpdt[4] = {0};
	ST_monthday tmpmdd;

	tmpday = atom_time / 86400;//24*60*60
	tmphour = atom_time % 86400;
	tmpminute = tmphour % 3600;//60*60
	tmpt.u8_hour = (unsigned char)(tmphour/3600);
	tmpt.u8_minute = (unsigned char)(tmpminute/60);
	tmpt.u8_second = (unsigned char)(tmpminute%60);

	tmpd = tmpday / 1461; //n 4 year //365*4+1
	tmpdd = tmpday % 1461;   //double year counter
	unsigned char tmpycnt = (unsigned char)(tmpdd/365);
	tmpy = st_EU_base.u16_year + tmpd*4;
	if(0 == tmpdd) 
	{
		tmpy -= 1;
		tmpt.u8_month = 12;
		tmpt.u8_day = 31;
	}
	else
	{
		for(tmp_i=0; tmp_i<(tmpycnt+1); tmp_i++) 
		{
			tmpdt[tmp_i] = 365 * (tmp_i+1);
			if(rtc_is_double_year(tmpy+tmp_i))
			{
				for(; tmp_i<(tmpycnt+1); tmp_i++)
					tmpdt[tmp_i] = 365*(tmp_i+1) + 1;
				break;
			}
		}
		for(tmp_i=0; tmp_i<(tmpycnt+1); tmp_i++)
		{
			if(tmpdd <= tmpdt[tmp_i]) 
			{
 				tmpy += tmp_i;
				if(tmp_i) 
					tmpdd -= tmpdt[tmp_i-1];
				break;
			}
		}
		if(4 == tmp_i)
		{
			tmpy += 3;
			tmpdd = tmpdt[3] - tmpdt[2];
		}
		rtc_get_month_from_yeardays(tmpdd,tmpy,&tmpmdd);
		tmpt.u8_month = tmpmdd.u8_month;
		if(tmpmdd.u8_day)
			tmpt.u8_day = tmpmdd.u8_day;
		else 
			tmpt.u8_day = 1;
	}
	tmpt.u16_year =(unsigned short int)tmpy;
	tmpt.u8_week = rtc_get_week_from_date(tmpt.u16_year,tmpt.u8_month,tmpt.u8_day);
	st_time->u16_year = tmpt.u16_year;
	st_time->u8_month = tmpt.u8_month;
	st_time->u8_week = tmpt.u8_week;
	st_time->u8_day = tmpt.u8_day;
	st_time->u8_hour = tmpt.u8_hour;
	st_time->u8_minute = tmpt.u8_minute;
	st_time->u8_second = tmpt.u8_second;
}

//=====================================================================
//function script current time convert to ABS atom time
//input current time
//output return a atom time
//create date 2008.03.13
//=====================================================================
__uint64_t ymd_time_to_atom(ST_datetime *currentime)
{
	__uint64_t tempam_ret = 0;
	unsigned char f_d,tmp_i;
	unsigned short int tmpdt[4] = {0},temp_p;
	unsigned int tempam;
	tempam = currentime->u16_year;
	f_d = rtc_is_double_year(tempam);
	if(tempam < st_EU_base.u16_year) 
		return;
	tempam -= st_EU_base.u16_year;
	temp_p = tempam % 4;
	tempam = tempam / 4;
	tempam_ret = (__uint64_t)(tempam*1461); //365*4+1 day
	tempam = tempam*4 + st_EU_base.u16_year;

	for(tmp_i=0; tmp_i<(temp_p+1); tmp_i++) 
	{
		tmpdt[tmp_i]=365 * tmp_i;
		if(rtc_is_double_year(tempam+tmp_i))
		{
			for(; tmp_i<(temp_p+1); tmp_i++)
				tmpdt[tmp_i] = 365*tmp_i + 1;
			tmp_i --;//恢复有效值
			break;
		}
	}
	tempam_ret += (__uint64_t)tmpdt[temp_p]; 

	tempam_ret += (__uint64_t)sum_monthday[currentime->u8_month-1];
	tempam_ret += (__uint64_t)currentime->u8_day;
	//当前年份为闰年且小于2月,减一天
	if(f_d && currentime->u8_month<=2 && 0==tmp_i)
		tempam_ret -= 1;

	tempam_ret *= (__uint64_t)86400;//24*60*60
	tempam_ret += (__uint64_t)(currentime->u8_hour*3600) ;//60*60
	tempam_ret += (__uint64_t)(currentime->u8_minute*60) ;//60
	tempam_ret += (__uint64_t)(currentime->u8_second) ;//
			
	return tempam_ret;
}
//=====================================================================
//function script double year
//input year
//output return yes or no
//create date 2008.03.12
//=====================================================================
unsigned char rtc_is_double_year(unsigned short int year)
{
	unsigned char is_ret=0;

	if(0==(year%400)||((year%100)!=0&&0==(year%4)))
		is_ret=1;
	return is_ret;
}
//=====================================================================
//function script get week from date
//input year month day
//output return week
//create date 2008.03.12
//=====================================================================
unsigned char rtc_get_week_from_date(unsigned short int year,unsigned char month,unsigned char day)
{
	unsigned char w=1; //because 1901-01-01 is monday
	unsigned short int y,d;
	y=year-1+(year-1)/4-(year-1)/100+(year-1)/400;
	d=day+sum_monthday[month-1];
	if(month>2&&rtc_is_double_year(year))
	{
		d+=1;
	}
	y+=d;
	w=y%7;
	return w;
}
//=====================================================================
//function script Get urtime
//input
//output current urtime
//create date 2008.03.15
//=====================================================================
void rtc_get_cur_urtime(ST_datetime *st_sd)
{
	unsigned long flags;
	ST_datetime *tmp_st=&st_EU_time;
	flags = spin_lock_irqsave();
	__uint64_t tempam_cur=RTC_INREG32(A_RTC_RTCSR);
	tempam_cur = tempam_cur +st_time_atom_base;
	st_time_atom=tempam_cur;
	atom_time_to_ymd(tempam_cur,st_sd);
	memcpy(tmp_st,st_sd,sizeof(ST_datetime));
//REG_RTC_RSAR = lval;
	spin_unlock_irqrestore(flags);
//return tmp_st;
}
//=====================================================================
//function script Get abs time
//input
//output current urtime
//create date 2008.03.15
//=====================================================================
__uint64_t rtc_get_atom_time(void)
{
	unsigned long flags;
	flags = spin_lock_irqsave();
	__uint64_t tempam_cur = RTC_INREG32(A_RTC_RTCSR);
	tempam_cur+=st_time_atom_base;
	st_time_atom=tempam_cur;
	spin_unlock_irqrestore(flags);
	return tempam_cur;
}
//=====================================================================
//function script Get urtime
//input
//output current urtime
//create date 2008.03.15
//=====================================================================
void rtc_set_rtc_datetime(ST_datetime * st_time)
{
//st_time_atom_base
	unsigned long flags;
	flags = spin_lock_irqsave();
	__uint64_t tempam_cur=ymd_time_to_atom(st_time);
	st_time_atom=tempam_cur;
	RTC_OUTREG32(A_RTC_RTCSR, tempam_cur-st_time_atom_base);
	spin_unlock_irqrestore(flags);
}
//=====================================================================
//function script Set RTC from abs time
//input
//output current urtime
//create date 2008.03.19
//=====================================================================
void rtc_set_rtc_abs(__uint64_t u64_time)
{
//st_time_atom_base
	unsigned long flags;
	flags = spin_lock_irqsave();
	st_time_atom=u64_time;
	RTC_OUTREG32(A_RTC_RTCSR, u64_time-st_time_atom_base);
	spin_unlock_irqrestore(flags);
}
//=====================================================================
//function script Get base urtime
//input
//output current urtime
//create date 2008.03.16
//=====================================================================
static ST_datetime *rtc_get_base_urtime(void)
{
	return &st_EU_base;
}
//=====================================================================
//function script Get base urtime
//input
//output current urtime
//create date 2008.03.16
//=====================================================================
static ST_datetime *get_base_start_urtime(void)
{
	return &st_EU_start;
}


#ifdef USE_MIDWARE

//#if 0
volatile static MIDSRC rtcsrc;
static unsigned int rtcid;

static void GetRequest(MIDSRCDTA *dat)
{
//	printf("Up layer get :%d \n",res.Val);
}

static void Response(MIDSRCDTA *dat)
{
	printf("dat->Val == %d\n",dat->Val);
	if (dat->Val == 1)
	{
		cli();
		jz_pm_hibernate();
	}
}

static void info_low_power()
{
	printf("Send info low power!\n");
	rtcsrc.Src = SRC_POWER_LOW;
	rtcsrc.Event =  EVENT_BAT_CRITICAL_LOW;
	os_QueuePost(rtcsrc.CurEvent1 , (void *)&rtcid);
	os_SemaphorePost(rtcsrc.CurEvent);
}

static void info_set_alarm()
{
	//printf("Send info set rtc alarm!\n");
	rtcsrc.Src = SRC_SET_ALM;
	rtcsrc.Event =  EVENT_SET_ALM;
	os_QueuePost(rtcsrc.CurEvent1 , (void *)&rtcid);
	os_SemaphorePost(rtcsrc.CurEvent);
}

void rtc_power_check_init()
{
	rtcsrc.GetRequest = GetRequest;
	rtcsrc.Response = Response;
	rtcsrc.Name = "RTC";
	printf("Register Midware SRC RTC! \n");
	RegisterMidSrc((PMIDSRC)&rtcsrc);
	rtcid = rtcsrc.ID;
	printf("rtc ID %d \n",rtcsrc.ID);

}
#endif

void do_bat_low(void)
{
	printf("Now bat capacity too low, power off at once! \n");
#ifdef USE_MIDWARE
	info_low_power();
#endif
//	sys_shutdown(2);
//	ShutLcd(0);
}

static void rtc_alarm_handler(void)
{
#ifdef USE_MIDWARE	
	info_set_alarm();
#endif	
}

static void jz_rtc_interrupt(unsigned int arg)
{
	if (RTC_INREG32(A_RTC_RTCCR) & RTCCR_AF) 	/* rtc alarm interrupt */
	{
		RTC_CLRREG32(A_RTC_RTCCR, RTCCR_AF);

#ifdef AHB_MONITOR_PERIOD
		// Stop and log the data, and the 1st of the data is INVALID.
		MONITOR_LOG();
		// Start the monitor.
		MONITOR_START(MASTER_IPU, MEVENT_BUS_TRANS_CYCLE, MASTER_ALL, MEVENT_BUS_TRANS_CYCLE);

		// Config the RTC alarm for the next time
		RTC_OUTREG32(A_RTC_RTCSAR, RTC_INREG32(A_RTC_RTCSR) + AHB_MONITOR_PERIOD);
#else
		printf("RTC alarm interrupt clean!\n");
		rtc_alarm_handler();
#endif
	}
	else
	{
		RTC_CLRREG32(A_RTC_RTCCR, RTCCR_1HZ);
	}
}

void rtc_alarm_bat_start()
{
	printf("Rtc alarm for auto power off start! \n");
	RTC_CLRREG32(A_RTC_RTCCR, RTCCR_AF);
	request_irq(IRQ_RTC, jz_rtc_interrupt, 0);
}


void rtc_alarm_stop()
{
#ifdef AHB_MONITOR_PERIOD
	return;
#endif

	RTC_CLRREG32(A_RTC_RTCCR, RTCCR_AF | RTCCR_AIE);
	RTC_CLRREG32(A_RTC_HWCR, HWCR_EALM);

	printf("Rtc alarm for auto power off stop! \n");
}

//=====================================================================
//function script RTC initial
//input
//output current urtime
//create date 2008.03.19
//=====================================================================
void jz_rtc_init(void) 
{
	unsigned int tmpx = RTC_INREG32(A_RTC_HSPR), flag;

	printf("+jz_rtc_init\n");

	st_time_atom=ymd_time_to_atom(&st_EU_start); //set base time
	st_time_atom_base=st_time_atom;

	flag = RTC_INREG32(A_RTC_RTCGR) & RTCGR_DIV_MASK;
	if((tmpx != COLD_BOOT_SIG) || (flag != RTCGR_DIV_1HZ))
	{
		printf("tmpx=0x%x, rtcgr=0x%x\n", tmpx, RTC_INREG32(A_RTC_RTCGR));
		u32_RTC_time_l=0;
		u32_RTC_time_h=0;
 		atom_time_to_ymd(st_time_atom,&st_EU_time);  //get base time info

		RTC_OUTREG32(A_RTC_RTCGR, RTCGR_DIV_1HZ);
		RTC_OUTREG32(A_RTC_HSPR, COLD_BOOT_SIG);
		RTC_OUTREG32(A_RTC_RTCSR, 0x00000000);

		rtc_set_rtc_abs(st_time_atom_base);
		rtc_set_rtc_datetime(&st_EU_sample);

		RTC_SETREG32(A_RTC_RTCCR, RTCCR_RTCEN);
	}
	else
	{
		rtc_get_cur_urtime(&st_EU_time);
	}	

	RTC_OUTREG32(A_RTC_RTCSAR, RTC_INREG32(A_RTC_RTCSR) - 1);

#define RTCID (('R'<<24)|('T'<<16)|('C'<<8)|(':'<<0))
	rtc_alarm_bat_start();
	PM_AddPowerCtrl(RTCID,rtc_alarm_stop,0);

#ifdef AHB_MONITOR_PERIOD
	SetRtcAlmTime(AHB_MONITOR_PERIOD);
#endif
}

int SetRtcAlmTime(unsigned int alarm_time)
{
#ifdef AHB_MONITOR_PERIOD
	alarm_time = AHB_MONITOR_PERIOD;
#endif

	unsigned int rtccr = RTC_INREG32(A_RTC_RTCCR);
	unsigned int tick = alarm_time + RTC_INREG32(A_RTC_RTCSR);

	printf("rtc alarm will be %d second late!\n", alarm_time);
	RTC_OUTREG32(A_RTC_RTCSAR, tick);
	
	if (!(rtccr & RTCCR_AE)) {
		RTC_OUTREG32(A_RTC_RTCCR, rtccr | RTCCR_AE);
	}

	if ( !(rtccr & RTCCR_AIE) ) { /* Enable alarm irq */
		RTC_OUTREG32(A_RTC_RTCCR, rtccr | RTCCR_AIE);
	}
	return 0;
}

unsigned int GetRtcAlmTime(void)
{ 
	return (RTC_INREG32(A_RTC_RTCSAR));
}
