/*
 * wmrdata.h:
 * WMR data structures (encapsulation of readings) and utils
 *
 * This software may be freely used and distributed according to the terms
 * of the GNU GPL version 2 or 3. See LICENSE for more information.
 *
 * Copyright (c) 2015 David Čepelík <cepelik@gymlit.cz>
 */

#ifndef WMRDATA_H
#define	WMRDATA_H

#include "common.h"
#include "strbuf.h"


enum packet_type {
	WMR_WIND = 0xD3,
	WMR_RAIN = 0xD4,
	WMR_UVI	 = 0xD5,
	WMR_BARO = 0xD6,
	WMR_TEMP = 0xD7,
	WMR_STATUS = 0xD9,
	WMR_META = 0xFF		/* system meta-packet */
};


typedef struct {
	const char *dir;	/* wind direction, see `wmr200.c' */
	float gust_speed;	/* gust speed, m/s */
	float avg_speed;	/* average speed, m/s */
	float chill;		/* TODO what's this? */
} wmr_wind;


typedef struct {
	float rate;		/* immediate rain rate, mm/m^2 */
	float accum_hour;	/* rain last hour, mm/m^2 */
	float accum_24h;	/* rain 24 hours (without rain_hour), mm/m^2 */
	float accum_2007;	/* accum rain since 2007-01-01 12:00, mm/m^2 */
} wmr_rain;


typedef struct {
	uint_t index;		/* "UV index", value in range 0..15 */
} wmr_uvi;


typedef struct {
	uint_t pressure;	/* immediate pressure, hPa */
	uint_t alt_pressure;	/* TODO */
	const char *forecast;	/* name of "forecast icon", see `wmr200.c' */
} wmr_baro;


typedef struct {
	uint_t sensor_id;	/* ID in range 0..MAX_EXT_SENSORS, 0 = console */
	uint_t humidity;	/* relative humidity, percent */
	uint_t heat_index;	/* value 0..4, 0 = undefined (temp too low) */
	float temp;		/* temperature, deg C */
	float dew_point;	/* dew point, deg C */
} wmr_temp;


typedef struct {
	const char *wind_bat;		/* battery levels, see `wmr200.c' */
	const char *temp_bat;
	const char *rain_bat;
	const char *uv_bat;

	const char *wind_sensor;	/* sensor states, see `wmr200.c' */
	const char *temp_sensor;
	const char *rain_sensor;
	const char *uv_sensor;

	const char *rtc_signal_level;	/* signal level of the RTC */
} wmr_status;


typedef struct {
	uint_t num_packets;
	uint_t num_failed;
	uint_t num_frames;
	float error_rate;
	ulong_t num_bytes;
	time_t latest_packet;
	time_t uptime;
} wmr_meta;


typedef struct {
	uint_t type;
	time_t time;
	union {
		wmr_wind wind;
		wmr_rain rain;
		wmr_uvi uvi;
		wmr_baro baro;
		wmr_temp temp;
		wmr_status status;
		wmr_meta meta;
	};
} wmr_reading;


typedef struct {
	wmr_reading wind;
	wmr_reading rain;
	wmr_reading uvi;
	wmr_reading baro;
	wmr_reading temp[10]; /* TODO */
	wmr_reading status;
	wmr_reading meta;
} wmr_latest_data;


char *wmr_sensor_name(wmr_reading *reading);


#endif
