#include "julian.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

/* helper function declarations */
static bool is_julian_leap(long year);
static bool is_gregorian_leap(long year);
static bool is_pre_gregorian(long year, int month, float day);
static bool is_missing_gregorian_date(long year, int month, float day);
static enum day_of_week day_of_week_jd(double jd);

static int DAYS_IN_MONTH[]   = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static char *MONTH_NAMES_3LETTER[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                       "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static char *MONTH_NAMES[] = { "January", "February", "March",
                               "April", "May", "June",
                               "July", "August", "September",
                               "October", "November", "December" };

/**
 * Returns true if given date is before the Gregorian calendar transition
 */
static bool is_pre_gregorian(long year, int month, float day) {
	if (year > 1582)      return false;
	else if (year < 1582) return true;
	else {
		meeus_err err;
		int oct15_doy, doy;
		err = cal_to_day_of_year(1582, 10, 15, &oct15_doy);
		err = cal_to_day_of_year(1582, month, day, &doy);
		return doy < oct15_doy;
	}
}

/**
 * Returns true if given date is in interval [5 Oct 1582, 14 Oct 1582]
 *
 * These dates _do not exist_ due to the transition from Julian to
 * Gregorian calendar. They have no corresponding Julian Date.
 */
static bool is_missing_gregorian_date(long year, int month, float day) {
	if (year == 1582 && month == 10 && (day >= 5 && day <= 14))
		return true;
	return false;
}

meeus_err cal_to_julian_date(long year, int month, float day, double *jd) {
	/* results invalid before JD 0 (1 Jan 4713 BC) */
	assert(year >= -4712);
	if (year < -4712 || is_missing_gregorian_date(year, month, day))
		return MeeusError_InvalidDate;

	if (month < 3) {
		year = year - 1;
		month = month + 12;
	}

	/* pg. 60 */
	long a, b;
	if (is_pre_gregorian(year, month, day)) {
		b = 0;		/* assume Julian calendar */
	} else {
		a = floor(year/100);
		b = 2 - a + floor(a/4);
	}

	/* eq. 7-1 */
	*jd = floor(365.25 * (year + 4716)) +
		floor(30.6001 * (month + 1)) +
		day + b - 1524.5;
	return MeeusError_Success;
}

meeus_err julian_date_to_cal(double jd, long *year, int *month, float *day) {
	long z;
	double a, alpha, b, c, d, e, f;

	/* pg. 63 */
	jd = jd + 0.5;
	z = (long) jd;		/* integer part of jd */
	f = jd - z;		/* fractional part of jd */

	if (z < 2299161)
		a = z;
	else {
		alpha = floor((z - 1867216.25)/36524.25);
		a = z + 1 + alpha - floor(alpha/4.0);
	}

	b = a + 1524;
	c = floor((b - 122.1)/365.25);
	d = floor(365.25 * c);
	e = floor((b - d)/30.6001);

	*day = b - d - floor(30.6001*e) + f;

	if (e < 14) *month = e - 1;
	else        *month = e - 13;

	if (*month > 2) *year = c - 4716;
	else            *year = c - 4715;

	return MeeusError_Success;
}

char *short_month_name(enum month_index m) {
	if (m < JAN || m > DEC)
		return "UNK";
	return MONTH_NAMES_3LETTER[m-1];
}

meeus_err month_name(enum month_index m, char* name) {
	if (m < JAN || m > DEC)
		return MeeusError_InvalidInput;
	name = MONTH_NAMES[m-1];
	return MeeusError_Success;
}

bool is_leap(long year) {
	return year < 1582 ? is_julian_leap(year) : is_gregorian_leap(year);
}

static bool is_julian_leap(long year) {
	return year % 4 == 0;
}

static bool is_gregorian_leap(long year) {
	return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

meeus_err cal_to_day_of_year(long year, int month, float day, int *day_of_year) {
	int k;
	if (is_leap(year)) k = 1;
	else               k = 2;
	*day_of_year = floor((275 * month)/9) -
		k * floor((month + 9)/12) + day - 30;
	return MeeusError_Success;
}

meeus_err day_of_year_to_cal(long year, int day_of_year, int *month, float *day) {
	int m, d, k;
	int n = day_of_year;
	bool leap = is_leap(year);

	if (n < 1 || (leap && n > 366) || n > 365)
		return MeeusError_InvalidDate;

	/* pg. 66 */
	if (leap) k = 1;
	else      k = 2;

	if (n < 32) m = 1;
	else        m = floor(((9*(k + n)) / 275.0) + 0.98);

	d = n - floor((275 * m) / 9.0) +
		(k * floor((m + 9) / 12.0)) + 30;

	*month = m;
	*day = d;
	return MeeusError_Success;
}

meeus_err days_in_month(long year, int month, int *days) {
	if (month < 1 || month > 12)
		return MeeusError_InvalidDate;

	*days = DAYS_IN_MONTH[month-1];
	if (month == 2 && is_leap(year))
		*days += 1;
	return MeeusError_Success;
}

meeus_err day_of_week(long year, int month, float day, enum day_of_week *dow) {
	day = (int) day;	/* set 0 hours on day */
	double jd;
	meeus_err err = cal_to_julian_date(year, month, day, &jd);
	if (MeeusError_Success != err)
		return err;
	*dow = day_of_week_jd(jd);
	return MeeusError_Success;
}

static enum day_of_week day_of_week_jd(double jd) {
	int z = (int)jd + 2;
	return z % 7;
}
