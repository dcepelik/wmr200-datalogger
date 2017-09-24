/*
 * loggers/server.h:
 * Make readings available to the server
 *
 * This software may be freely used and distributed according to the terms
 * of the GNU GPL version 2 or 3. See LICENSE for more information.
 *
 * Copyright (c) 2015 David Čepelík <cepelik@gymlit.cz>
 */

#ifndef SERVER_H
#define	SERVER_H

#include "wmrdata.h"


void server_push_reading(struct wmr200 *wmr, wmr_reading *reading, void *arg);


#endif