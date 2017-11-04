#include "rtc.h"

Byte bcd2dec(Byte i)
{
	//o segundo digito nos 4 + significativos(dezenas)
	//o primeiro nos 4 -significativos
	return ((i>>4)*10) + (i & 0x0F);
}

Byte dec2bcd(Byte i)
{
	return ((i/10) << 4) + (i%10);
}

void rtc_valid()
{
	int ints_en = disable();
	outportb(RTC_ADDR_REG, RTC_STAT_A);
	while(inportb(RTC_DATA_REG) & RTC_UIP)
	;
	if(ints_en)
		enable();
}

Byte read_rtc(Byte add)
{
	Byte data;
	int ints_en = disable();
	outportb(RTC_ADDR_REG,add);
	data=inportb(RTC_DATA_REG);
	if(ints_en)
		enable();
	return data;
}

Byte read_rtcv(Byte add)
{
	Byte data;
	int ints_en = disable();
	rtc_valid();
	outportb(RTC_ADDR_REG,add);
	data=inportb(RTC_DATA_REG);
	if(ints_en)
		enable();
	return data;
}

void write_rtc(Byte add, Byte value)
{
	int ints_en = disable();
	outportb(RTC_ADDR_REG,add);
	outportb(RTC_DATA_REG,value);
	if(ints_en)
		enable();
}

void write_rtcv(Byte add, Byte value)
{
	int ints_en = disable();
	rtc_valid();
	outportb(RTC_ADDR_REG,add);
	outportb(RTC_DATA_REG,value);
	if(ints_en)
		enable();
}

void rtc_read_time(RTC_TIME *t)
{
	t->sec = bcd2dec(read_rtcv(SEC));
	t->min = bcd2dec(read_rtcv(MIN));
	t->hour = bcd2dec(read_rtcv(HOUR));
}

void rtc_read_date(RTC_DATE *d)
{
	d->day = bcd2dec(read_rtcv(MONTH_DAY));
	d->month = bcd2dec(read_rtcv(MONTH));
	d->year = bcd2dec(read_rtcv(YEAR));	
}

void rtc_read_alarm(RTC_TIME *a)
{
	a->sec = bcd2dec(read_rtcv(SEC_ALARM));
	a->min = bcd2dec(read_rtcv(MIN_ALARM));
	a->hour = bcd2dec(read_rtcv(HOUR_ALARM));
}

void rtc_write_alarm(RTC_TIME *a)
{
	write_rtcv(HOUR_ALARM, dec2bcd(a->hour));
	write_rtcv(MIN_ALARM, dec2bcd(a->min));
	write_rtcv(SEC_ALARM, dec2bcd(a->sec));
}
