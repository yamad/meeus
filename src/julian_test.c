#include "unity.h"
#include "unity_internals.h"

#include "julian.h"
#include <stdio.h>

struct test_date {
	long year;
	int month;
	float day;
	double expected_jd;
};

#define NDATES 20
struct test_date dates[NDATES] = {{ 1957, 10, 4.81, 2436116.31},
                                  {  333,  1, 27.5, 1842713.0 },
                                  { 2000,  1,  1.5, 2451545.0 },
                                  { 1999,  1,  1.0, 2451179.5 },
                                  { 1987,  1, 27.0, 2446822.5 },
                                  { 1987,  6, 19.5, 2446966.0 },
                                  { 1988,  1, 27.0, 2447187.5 },
                                  { 1988,  6, 19.5, 2447332.0 },
                                  { 1900,  1,  1.0, 2415020.5 },
                                  { 1600,  1,  1.0, 2305447.5 },
                                  { 1600, 12, 31.0, 2305812.5 },
                                  {  837,  4, 10.3, 2026871.8 },
                                  { -123, 12, 31.0, 1676496.5 },
                                  { -122,  1,  1.0, 1676497.5 },
                                  {-1000,  7, 12.5, 1356001.0 },
                                  {-1000,  2, 29.0, 1355866.5 },
                                  {-1001,  8, 17.9, 1355671.4 },
                                  {-4712,  1,  1.5,       0.0 },
                                  { 1582, 10,    4, 2299159.5 },
                                  { 1582, 10,   15, 2299160.5 } };

void test_cal_to_julian_date(void) {
	double jd_res;
	meeus_err err;
	char msg_buf[255];

	for (int i = 0; i < NDATES; i++) {
		struct test_date *d = &dates[i];
		err = cal_to_julian_date(d->year, d->month, d->day, &jd_res);
		if (MeeusError_Success == err) {
			snprintf(msg_buf, 255,
				"%ld %.3s %4f, expected %f, actual %f",
				d->year, short_month_name(d->month), d->day,
				d->expected_jd, jd_res);
			TEST_ASSERT_EQUAL_FLOAT_MESSAGE(d->expected_jd,
				jd_res, msg_buf);
		} else {
			snprintf(msg_buf, 255,
				"%ld %.3s %4f produced an error",
				d->year, short_month_name(d->month), d->day);
			TEST_FAIL_MESSAGE(msg_buf);
		}
	}
}

void test_julian_date_to_cal(void) {
	double jd;
	long year; int month; float day;
	meeus_err err;
	char msg[255];

	for (int i = 0; i < NDATES; i++) {
		struct test_date *d = &dates[i];
		err = julian_date_to_cal(d->expected_jd, &year, &month, &day);
		if (MeeusError_Success == err) {
			snprintf(msg, 255,
				"%f --> %ld %.3s %4f, expected %ld %.3s %4f",
				d->expected_jd, year, short_month_name(month), day,
				d->year, short_month_name(d->month), d->day);
			TEST_ASSERT_EQUAL_INT_MESSAGE(d->year, year, msg);
			TEST_ASSERT_EQUAL_INT_MESSAGE(d->month, month, msg);
			TEST_ASSERT_EQUAL_FLOAT_MESSAGE(d->day, day, msg);
		} else {
			snprintf(msg, 255,
				"%ld %.3s %4f produced an error",
				d->year, short_month_name(d->month), d->day);
			TEST_FAIL_MESSAGE(msg);
		}
	}
}

void test_cal_to_julian_date_missing_gregorian(void) {
	meeus_err err;
	char msg[255];
	double jd;

	/* all dates from 5 Oct 1582 to 14 Oct 1582 should fail */
	for (int day = 5; day <= 14; day++) {
		err = cal_to_julian_date(1582, 10, (double)day, &jd);
		if (MeeusError_Success == err) {
			snprintf(msg, 255,
				"Invalid date should not succeed: 1582 Oct %d", day);
			TEST_FAIL_MESSAGE(msg);
		}
	}

	err = cal_to_julian_date(1582, 10, 4.9, &jd);
	if (MeeusError_Success != err) TEST_FAIL_MESSAGE(meeus_perror(err));
	snprintf(msg, 255, "actual: %f", jd);
	TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(2299160.4, jd, msg);

	err = cal_to_julian_date(1582, 10, 15.1, &jd);
	if (MeeusError_Success != err) TEST_FAIL_MESSAGE("15 Oct 1582 failed");
	snprintf(msg, 255, "actual: %f", jd);
	TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(2299160.6, jd, msg);
}

void test_day_of_week(void) {
	meeus_err err;
	enum day_of_week dow;

	/* example from Meeus */
	err = day_of_week(1954, JUN, 30, &dow);
	TEST_ASSERT_EQUAL_INT(WED, dow);

	/* full week of date when this test was written :-P */
	enum day_of_week expected = SUN;
	for (int day = 25; day <= 31; day++) {
		err = day_of_week(2015, OCT, day, &dow);
		TEST_ASSERT_EQUAL_INT(expected++, dow);
	}
}

struct date_doy {
	long year;
	int month;
	float day;
	int day_of_year;
};
#define NDOYDATES 9
struct date_doy doy_tests[] = { { 1978, NOV, 14, 318 },
                                { 1988, APR, 22, 113 },
                                { 1582, JAN, 01,   1 },
                                { 1582, DEC, 31, 365 },
                                /* Gregorian leap years */
                                { 2000, DEC, 31, 366 },
                                {    0, DEC, 31, 366 },
                                /* Gregorian common years */
                                { 1700, DEC, 31, 365 },
                                { 1800, DEC, 31, 365 },
                                /* Julian (only) leap years */
                                { 1000, DEC, 31, 366 } };

void test_cal_to_day_of_year(void) {
	meeus_err err;
	struct date_doy d;
	int doy;
	for (int i = 0; i < NDOYDATES; i++) {
		d = doy_tests[i];
		err = cal_to_day_of_year(d.year, d.month, d.day, &doy);
		TEST_ASSERT_EQUAL_INT(d.day_of_year, doy);
	}
}

void test_day_of_year_to_cal(void) {
	meeus_err err;
	struct date_doy d;
	int month;
	float day;
	for (int i = 0; i < NDOYDATES; i++) {
		d = doy_tests[i];
		err = day_of_year_to_cal(d.year, d.day_of_year, &month, &day);
		TEST_ASSERT_EQUAL_INT(d.month, month);
		TEST_ASSERT_EQUAL_FLOAT(d.day, day);
	}
}

void test_days_in_month(void) {
	meeus_err err;
	int dim;

	/* leap years */
	err = days_in_month(1000, FEB, &dim);
	TEST_ASSERT_EQUAL_INT(29, dim);
	err = days_in_month(2000, FEB, &dim);
	TEST_ASSERT_EQUAL_INT(29, dim);
	err = days_in_month(0, FEB, &dim);
	TEST_ASSERT_EQUAL_INT(29, dim);

	/* common years */
	err = days_in_month(1700, FEB, &dim);
	TEST_ASSERT_EQUAL_INT(28, dim);
}
