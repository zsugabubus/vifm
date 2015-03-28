/* vifm
 * Copyright (C) 2001 Ken Steen.
 * Copyright (C) 2011 xaizek.
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

#ifndef VIFM__ARGS_H__
#define VIFM__ARGS_H__

#include <stddef.h> /* size_t */

#include "utils/fs_limits.h"

/* Parsed command-line arguments. */
typedef struct
{
	int help;    /* Display help information and quit. */
	int version; /* Display version information and quit. */

	int logging;    /* Enable logging. */
	int no_configs; /* Skip reading configuration files. */

	char chosen_files_out[PATH_MAX]; /* Output for file picking. */
	char chosen_dir_out[PATH_MAX];   /* Output for directory picking. */
	const char *delimiter;           /* Delimiter for list of picked files. */
	const char *on_choose;           /* Action to perform on chosen files. */

	char **remote_cmds; /* Arguments to pass to server instance. */

	char lwin_path[PATH_MAX]; /* Chosen path of the left pane. */
	char rwin_path[PATH_MAX]; /* Chosen path of the right pane. */

	int lwin_handle; /* Whether to open file in the left pane (else select). */
	int rwin_handle; /* Whether to open file in the right pane (else select). */

	char **cmds;  /* List of startup commands. */
	size_t ncmds; /* Number of startup commands. */
}
args_t;

/* Parses command-line arguments into fields of the *args structure. */
void args_parse(args_t *args, int argc, char *argv[], const char dir[]);

/* Processes command-line arguments from fields of the *args structure.  General
 * args are --help and --version. */
void args_process(args_t *args, int general);

/* Frees memory allocated for the structure.  args can be NULL. */
void args_free(args_t *args);

#endif /* VIFM__ARGS_H__ */

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 : */