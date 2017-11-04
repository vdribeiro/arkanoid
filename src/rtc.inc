%define RTC_ADD_REG 70h
%define RTC_DATA_REG 71h

%define RTC_SEC 0
%define RTC_MIN 2
%define RTC_HOUR 4

%define RTC_STAT_A 10
%define RTC_STAT_B 11
%define RTC_STAT_C 12
%define RTC_STAT_D 13

%define RTC_SET 10000000b

%define EOI 0x20
%define PIC1_CMD 0x20 
%define PIC2_CMD 0xA0