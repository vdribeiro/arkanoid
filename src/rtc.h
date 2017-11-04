#ifndef _RTC_H_
#define _RTC_H_

#include <pc.h>
#include <dos.h>

#include "utypes.h"

/** @defgroup RealTimeController RealTimeController
 * @{
 *
 * Real Time Controller related functions
 */

/** Time structure. Atributes in plain decimal
 */
typedef struct {
  Byte hour, ///< The hour of the day
    min,    ///< The minute  of the day
    sec;    ///< The second  of the day
} RTC_TIME;

/** Data structure. Atributes in plain decimal
 */
typedef struct {
  Byte day, ///< The day of the month
    month, ///< The day of the year
    year; ///< The year
} RTC_DATE;

/** @name The RTC registers */
/*@{*/ 
enum rtc_regs {SEC, SEC_ALARM, 
	       MIN, MIN_ALARM,
	       HOUR, HOUR_ALARM,
	       WEEK_DAY, MONTH_DAY, MONTH, YEAR,
	       RTC_STAT_A, RTC_STAT_B, RTC_STAT_C, RTC_STAT_D};
/*@}*/ 

/** @name PC I/O register address used to comunicate with the RTC */
/*@{*/ 
#define RTC_ADDR_REG 0x70 ///< I/O address that contains the RTC address to communicate with 
#define RTC_DATA_REG 0x71 ///< I/O address that contains the data read from or written to the RTC
/*@}*/ 

/** @name Bit Meaning in STAT-A register */
/*@{*/ 
#define RTC_UIP (1 << 7) ///<  1-update in progress, 0-data valid for next 244 usec
#define RTC_DV2 (1 << 6) ///< Divider
#define RTC_DV1 (1 << 5) ///< Divider
#define RTC_DV0 (1 << 4) ///< Divider
#define RTC_RS3 (1 << 3) ///< Rate selector
#define RTC_RS2 (1 << 2) ///< Rate selector
#define RTC_RS1 (1 << 1) ///< Rate selector
#define RTC_RS0 (1 << 0) ///< Rate selector
/*@}*/ 

/** @name Bit Meaning in STAT-B register */
/*@{*/ 
#define RTC_SET  (1 << 7) ///< 1-Stop updating, 0-update normally 
#define RTC_PIE  (1 << 6) ///< 1-Periodic interrupt enable
#define RTC_AIE  (1 << 5) ///< 1-Alarm interrupt enable
#define RTC_UIE  (1 << 4) ///< 1-Update-ended interrupt enable
#define RTC_SQWE (1 << 3) ///< 1-Square Wave Enable
#define RTC_DM   (1 << 2) ///< Data Mode, 1-Binary, 0-BCD 
#define RTC_12_24 (1 << 1) ///< 1-24 hours, 0-12 hours
#define RTC_DSE  (1 << 0) ///< 1-Day-light saving
/*@}*/ 

/** @name Bit Meaning in STAT-C register */
/*@{*/
#define RTC_IRQF (1 << 7) ///< 1-An interrupt occurred, if PF=PIE=1 || AF=AIE=1 || UF=UIE=1 
#define RTC_PF   (1 << 6) ///< 1-Periodic interrupt occurred
#define RTC_AF   (1 << 5) ///< 1-Alarm interrupt occurred
#define RTC_UF   (1 << 4) ///< 1-Update interrupt occurred
/*@}*/
 
/** @name Bit Meaning in STAT-D register */
/*@{*/
#define RTC_VRT (1 << 7) ///< 1-Valid RAM and time
/*@}*/

/** converts BCD (Binary Coded Decimal) to decimal
 */
Byte bcd2dec(Byte i);

/** converts decimal to BCD (Binary Coded Decimal)
 */
Byte dec2bcd(Byte i);

/** Wait until data in rtc is valid.
 * Data is valid until +/- 240 usec after this function returns,
 * thus a read/write can be done to the RTC within this time period
 */
void rtc_valid();

/** Returns rtc data from I/O address add. Doesn't check valid bit
 */
Byte read_rtc(Byte add);

/** Returns rtc data from I/O address add. Check valid bit
 */
Byte read_rtcv(Byte add);

/** Write value to I/O address add. Doesn't check valid bit
 */
void write_rtc(Byte add, Byte value);

/** Write value to I/O address add. Check valid bit
 */
void write_rtcv(Byte add, Byte value);

/** Read RTC stored time
 *
 * Uses read_rtcv() and bcd2dec()
 */
void rtc_read_time(RTC_TIME *);

/** Read RTC stored date
 *
 * Uses read_rtcv() and bcd2dec()
 */
void rtc_read_date(RTC_DATE *);

/** Read RTC stored alarm
 *
 * Uses read_rtcv() and bcd2dec()
 */
void rtc_read_alarm(RTC_TIME *);

/** Write alarm to RTC
 *
 * Uses write_rtcv() and dec2bcd()
 */
void rtc_write_alarm(RTC_TIME *);

/** @} end of RealTimeController */

#endif
