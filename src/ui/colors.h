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

#ifndef VIFM__UI__COLORS_H__
#define VIFM__UI__COLORS_H__

/* Color information structure. */
typedef struct
{
	int fg;   /* Foreground color. */
	int bg;   /* Background color. */
	int attr; /* Attributes (bold, inverse, etc.). */
}
col_attr_t;

/* Elements of a color scheme.  Each one is an offset of a color pair inside of
 * color scheme. */
enum
{
	WIN_COLOR,          /* Pane background and default file highlight. */
	DIRECTORY_COLOR,    /* Directory. */
	LINK_COLOR,         /* Symbolic link. */
	BROKEN_LINK_COLOR,  /* Dangling symbolic link. */
	SOCKET_COLOR,       /* Socket. */
	DEVICE_COLOR,       /* Device file. */
	FIFO_COLOR,         /* Named pipe. */
	EXECUTABLE_COLOR,   /* Executable. */
	SELECTED_COLOR,     /* Selected item. */
	CURR_LINE_COLOR,    /* Line under the cursor in the selected pane. */
	TOP_LINE_COLOR,     /* Top line of the other pane. */
	TOP_LINE_SEL_COLOR, /* Top line of the selected pane. */
	STATUS_LINE_COLOR,  /* Status line. */
	WILD_MENU_COLOR,    /* Wild menu. */
	CMD_LINE_COLOR,     /* Command line. */
	ERROR_MSG_COLOR,    /* Error of the command line. */
	BORDER_COLOR,       /* Vertical border lines. */
	OTHER_LINE_COLOR,   /* Line under the cursor in the other pane. */
	JOB_LINE_COLOR,     /* Line that displays status of background jobs. */
	SUGGEST_BOX_COLOR,  /* Style of suggestion box. */
	MISMATCH_COLOR,     /* File entries that don't match each other in diff. */
	AUX_WIN_COLOR,      /* Auxiliary part of window. */
	TAB_LINE_COLOR,     /* Tab line. */
	TAB_LINE_SEL_COLOR, /* Tip of selected tab. */
	USER1_COLOR,        /* User color #1. */
	USER2_COLOR,        /* User color #2. */
	USER3_COLOR,        /* User color #3. */
	USER4_COLOR,        /* User color #4. */
	USER5_COLOR,        /* User color #5. */
	USER6_COLOR,        /* User color #6. */
	USER7_COLOR,        /* User color #7. */
	USER8_COLOR,        /* User color #8. */
	USER9_COLOR,        /* User color #9. */
	OTHER_WIN_COLOR,    /* Background and highlighting for inactive pane. */
	LINE_NUM_COLOR,     /* Selected item. */
	OTHER_LINE_NUM_COLOR, /* Selected item. */
	SEL_LINE_NUM_COLOR,    /* Selected item. */
	MAXNUM_COLOR,       /* Number of elements of a color scheme. */
};

#endif /* VIFM__UI__COLORS_H__ */

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 filetype=c : */
