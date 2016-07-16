/**
 * Julian day, Chapter 7
 *
 * Meeus, Astronomical Algorithms, 2nd edition
 */
#include "meeus_error.h"

#define bool int
#define true 1
#define false 0

enum month_index { JAN =  1, FEB =  2, MAR =  3, APR =  4, MAY =  5, JUN =  6,
                   JUL =  7, AUG =  8, SEP =  9, OCT = 10, NOV = 11, DEC = 12 };
enum day_of_week { SUN = 0, MON = 1, TUE = 2, WED = 3,
                   THU = 4, FRI = 5, SAT = 6 };

/**
 * Convert calendar date to Julian Date
 *
 * Calendar dates are assumed to follow Gregorian calendar if they
 * occur on or after 15 Oct 1582. Before this date, Julian calendar is
 * assumed.
 *
 * Julian Date may have fractional part, indicating fraction of day
 * since preceding noon. Numeric year should be given by astronomical
 * year counting---that is, "A.D." time is normal (2015 A.D. is 2015),
 * but year before 1 A.D. is year 0. Negative years indicate
 * B.C. time, with magnitude offset by +1 to account for counting year
 * 0 (e.g. 100 B.C. is -100 + 1 = -99; 1 B.C. is -1 + 1 = 0).
 *
 * Result is not valid for Julian days before JD 0 (Nov 24, 4714 BC,
 * Gregorian). "Missing" dates from Julian to Gregorian transition
 * (5--14 Oct 1582) are invalid inputs.
 *
 * @param year   full four-digit (or more) integer (e.g. 2015)
 * @param month  month index (1 - Jan, 2 - Feb, ..., 12 - Dec)
 * @param day    day of month. may be fractional (1-31, generally)
 * @param jd     returned Julian Date, may be fractional
 *
 * @ref Meeus, Chap 7, eq 7-1, pp 60-61.
 */
meeus_err cal_to_julian_date(long year, int month, float day, double *jd);

/**
 * Converts a Julian Date (JD) to a calendar date
 *
 * Not valid for days before JD 0.
 *
 * @see cal_to_julian_date()
 * @ref Meeus, Chap 7, pg. 63.
 */
meeus_err julian_date_to_cal(double jd, long *year, int *month, float *day);

/**
 * Return true if the year is a leap year
 *
 * valid for both Julian and Gregorian calendar years. before 1582,
 * years are treated as Julian. otherwise, years are Gregorian.
 *
 * @ref Meeus, Chap 7, pg. 62
 */
bool is_leap(long year);

/**
 * Return day of year index in range [1,365] (leap: [1,366])
 *
 * @ref Meeus, Chap 7, pg. 65
 */
meeus_err cal_to_day_of_year(long year, int month, float day, int *doy);

/**
 * Return calendar month and day corresponding to the given day of year
 *
 * Inverse of day_of_year()
 *
 * @param year  numerical year of given day of year
 * @param doy   day of year in interval [1,365] (leap: [1,366])
 *
 * @ref Meeus, Chap 7, pg. 66
 */
meeus_err day_of_year_to_cal(long year, int doy, int *month, float *day);

/**
 * Return day of week for given year
 *
 * @ref Meeus, Chap 7, pg. 65
 */
meeus_err day_of_week(long year, int month, float day, enum day_of_week *dow);


/**
 * Return number of days in the calendar month
 *
 * valid for both Gregorian and Julian calendars
 */
meeus_err days_in_month(long year, int month, int *days);

/**
 * Return three-letter month abbreviation for given month (1 --> Jan, 12 --> Dec)
 *
 * If month index is out of range, returns the string "UNK" for unknown.
 */
char *short_month_name(enum month_index m);
