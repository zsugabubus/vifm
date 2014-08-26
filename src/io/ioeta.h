/* vifm
 * Copyright (C) 2014 xaizek.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef VIFM__IO__IOETA_H__
#define VIFM__IO__IOETA_H__

#include <stddef.h> /* size_t */
#include <stdint.h> /* uint64_t */

/* ioeta - Input/Output estimation */

typedef struct
{
	/* Total number of items to process (T). */
	size_t total_items;

	/* Number of already processed items and index of the current item at the same
	 * time (0..T). */
	size_t current_item;

	/* Total number of bytes to process (T).  Size of directories is counted as
	 * 0. */
	uint64_t total_bytes;

	/* Number of already processed bytes. */
	uint64_t current_byte;

	/* Relative path to currently processed file relative to its source
	 * directory. */
	char *item;
}
eta_estimate_t;

eta_estimate_t * ioeta_alloc(void);

void ioeta_free(eta_estimate_t *estimate);

#endif /* VIFM__IO__IOETA_H__ */

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 : */
