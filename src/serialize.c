/*
 * serialize.c:
 * Super-simple WMR200 arr serialization/deserialization routines
 *
 * This software may be freely used and distributed according to the terms
 * of the GNU GPL version 2 or 3. See LICENSE for more information.
 *
 * Copyright (c) 2015 David Čepelík <cepelik@gymlit.cz>
 */


#include "serialize.h"
#include "wmrdata.h"
#include "strbuf.h"
#include "common.h"

#include <endian.h>		/* TODO this ain't compatible */


#define ARRAY_ELEM		unsigned char
#define ARRAY_PREFIX(x)		byte_##x
#include "array.h"


/********************** (de)serialization of primitives  **********************/


static void
serialize_long(struct byte_array *arr, long l) {
	long nl = htobe64(l);
	byte_array_push_n(arr, (unsigned char *)&nl, sizeof(nl));
}


static long
deserialize_long(struct byte_array *arr) {
	long be64 = 0;
	int i;

	for (i = 0; i < sizeof(be64); i++) {
		((char *)&be64)[i] = byte_array_shift(arr);
	}

	return be64toh(be64);
}


static void
serialize_int(struct byte_array *arr, int i) {
	serialize_long(arr, (long)i);
}


static int
deserialize_int(struct byte_array *arr) {
	return (int)deserialize_long(arr);
}


static void
serialize_char(struct byte_array *arr, char c) {
	byte_array_push(arr, c);
}


static char
deserialize_char(struct byte_array *arr) {
	return byte_array_pop(arr);
}


static void
serialize_float(struct byte_array *arr, float f) {
}


static float
deserialize_float(struct byte_array *arr) {
	return 0;
}


static void
serialize_string(struct byte_array *arr, const char *str) {
	int i = 0;
	while (str[i] != '\0') {
		byte_array_push(arr, str[i]);
		i++;
	}
	byte_array_push(arr, '\0');
}


static char *
deserialize_string(struct byte_array *arr) {
	strbuf str;

	strbuf_init(&str);
	while (byte_array_first(arr) != '\0') {
		strbuf_append(&str, "%c", byte_array_shift(arr));
	}

	byte_array_shift(arr); /* dump the '\0' */

	char *out_str = strbuf_copy(&str);
	strbuf_free(&str);

	return out_str;
}


/********************** (de)serialization of WMR data **********************/


static void
serialize_wind(struct byte_array *arr, wmr_wind *wind) {
	serialize_long(arr, wind->time);
	serialize_string(arr, wind->dir);
	serialize_float(arr, wind->gust_speed);
	serialize_float(arr, wind->avg_speed);
	serialize_float(arr, wind->chill);
}


static void
deserialize_wind(struct byte_array *arr, wmr_wind *wind) {
	wind->time = deserialize_long(arr);
	wind->dir = deserialize_string(arr);
	wind->gust_speed = deserialize_float(arr);
	wind->avg_speed = deserialize_float(arr);
	wind->chill = deserialize_float(arr);
}


static void
serialize_rain(struct byte_array *arr, wmr_rain *rain) {
	serialize_long(arr, rain->time);
	serialize_float(arr, rain->rate);
	serialize_float(arr, rain->accum_hour);
	serialize_float(arr, rain->accum_24h);
	serialize_float(arr, rain->accum_2007);
}


static void
deserialize_rain(struct byte_array *arr, wmr_rain *rain) {
	rain->time = deserialize_long(arr);
	rain->rate = deserialize_float(arr);
	rain->accum_hour = deserialize_float(arr);
	rain->accum_24h = deserialize_float(arr);
	rain->accum_2007 = deserialize_float(arr);
}


static void
serialize_uvi(struct byte_array *arr, wmr_uvi *uvi) {
	serialize_long(arr, uvi->time);
	serialize_int(arr, uvi->index);
}


static void
deserialize_uvi(struct byte_array *arr, wmr_uvi *uvi) {
	uvi->time = deserialize_long(arr);
	uvi->index = deserialize_int(arr);
}


static void
serialize_baro(struct byte_array *arr, wmr_baro *baro) {
	serialize_long(arr, baro->time);
	serialize_int(arr, baro->pressure);
	serialize_int(arr, baro->alt_pressure);
	serialize_string(arr, baro->forecast);
}


static void
deserialize_baro(struct byte_array *arr, wmr_baro *baro) {
	baro->time = deserialize_long(arr);
	baro->pressure = deserialize_int(arr);
	baro->alt_pressure = deserialize_int(arr);
	baro->forecast = deserialize_string(arr);
}


static void
serialize_temp(struct byte_array *arr, wmr_temp *temp) {
	serialize_long(arr, temp->time);
	serialize_int(arr, temp->sensor_id);
	serialize_int(arr, temp->humidity);
	serialize_int(arr, temp->heat_index);
	serialize_float(arr, temp->temp);
	serialize_float(arr, temp->dew_point);
}


static void
deserialize_temp(struct byte_array *arr, wmr_temp *temp) {
	temp->time = deserialize_long(arr);
	temp->sensor_id = deserialize_int(arr);
	temp->humidity = deserialize_int(arr);
	temp->heat_index = deserialize_int(arr);
	temp->temp = deserialize_float(arr);
	temp->dew_point = deserialize_float(arr);
}


static void
serialize_status(struct byte_array *arr, wmr_status *status) {
	serialize_long(arr, status->time);
	serialize_string(arr, status->wind_bat);
	serialize_string(arr, status->temp_bat);
	serialize_string(arr, status->rain_bat);
	serialize_string(arr, status->uv_bat);
	serialize_string(arr, status->wind_sensor);
	serialize_string(arr, status->temp_sensor);
	serialize_string(arr, status->rain_sensor);
	serialize_string(arr, status->uv_sensor);
	serialize_string(arr, status->rtc_signal_level);
}


static void
deserialize_status(struct byte_array *arr, wmr_status *status) {
	status->time = deserialize_long(arr);

	status->wind_bat = deserialize_string(arr);
	status->temp_bat = deserialize_string(arr);
	status->rain_bat = deserialize_string(arr);
	status->uv_bat = deserialize_string(arr);

	status->wind_sensor = deserialize_string(arr);
	status->temp_sensor = deserialize_string(arr);
	status->rain_sensor = deserialize_string(arr);
	status->uv_sensor = deserialize_string(arr);

	status->rtc_signal_level = deserialize_string(arr);
}


static void
serialize_meta(struct byte_array *arr, wmr_meta *meta) {
	serialize_long(arr, meta->time);
	serialize_int(arr, meta->num_packets);
	serialize_int(arr, meta->num_failed);
	serialize_int(arr, meta->num_frames);
	serialize_float(arr, meta->error_rate);
	serialize_long(arr, meta->num_bytes);
	serialize_long(arr, meta->latest_packet);
}


static void
deserialize_meta(struct byte_array *arr, wmr_meta *meta) {
	meta->time = deserialize_long(arr);
	meta->num_packets = deserialize_int(arr);
	meta->num_failed = deserialize_int(arr);
	meta->num_frames = deserialize_int(arr);
	meta->error_rate = deserialize_float(arr);
	meta->num_bytes = deserialize_long(arr);
	meta->latest_packet = deserialize_long(arr);
}


/********************** public interface **********************/


void
serialize_reading(struct byte_array *arr, wmr_reading *reading) {
	serialize_int(arr, reading->type);

	switch (reading->type) {
	case WMR_WIND:
		serialize_wind(arr, &reading->wind);
		break;

	case WMR_RAIN:
		serialize_rain(arr, &reading->rain);
		break;

	case WMR_UVI:
		serialize_uvi(arr, &reading->uvi);
		break;

	case WMR_BARO:
		serialize_baro(arr, &reading->baro);
		break;

	case WMR_TEMP:
		serialize_temp(arr, &reading->temp);
		break;

	case WMR_STATUS:
		serialize_status(arr, &reading->status);
		break;

	case WMR_META:
		serialize_meta(arr, &reading->meta);
		break;

	default:
		die("Cannot serialize reading of type %02x\n", reading->type);
	}
}


void
deserialize_reading(struct byte_array *arr, wmr_reading *reading) {
	reading->type = deserialize_int(arr);

	switch (reading->type) {
	case WMR_WIND:
		deserialize_wind(arr, &reading->wind);
		break;

	case WMR_RAIN:
		deserialize_rain(arr, &reading->rain);
		break;

	case WMR_UVI:
		deserialize_uvi(arr, &reading->uvi);
		break;

	case WMR_BARO:
		deserialize_baro(arr, &reading->baro);
		break;

	case WMR_TEMP:
		deserialize_temp(arr, &reading->temp);
		break;

	case WMR_STATUS:
		deserialize_status(arr, &reading->status);
		break;

	case WMR_META:
		deserialize_meta(arr, &reading->meta);
		break;

	default:
		die("Cannot serialize reading of type %02x\n", reading->type);
	}
}


