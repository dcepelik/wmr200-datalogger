/*
 * loggers/yaml.h:
 * Log readings serialized in YAML to a file or stream
 *
 * This software may be freely used and distributed according to the terms
 * of the GNU GPL version 2 or 3. See LICENSE for more information.
 *
 * Copyright (c) 2015 David Čepelík <cepelik@gymlit.cz>
 */


#ifndef LOGGERS_FILE_H
#define	LOGGERS_FILE_H

#include "wmrdata.h"


void yaml_push_reading(struct wmr200 *wmr, wmr_reading *reading, void *arg);


#endif