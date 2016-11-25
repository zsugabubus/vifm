#include <stic.h>

#include <sys/stat.h> /* chmod() */
#include <unistd.h> /* rmdir() symlink() */

#include <stdio.h> /* EOF FILE fclose() fgetc() fopen() remove() */
#include <string.h> /* strcpy() */

#include "../../src/cfg/config.h"
#include "../../src/compat/os.h"
#include "../../src/engine/mode.h"
#include "../../src/modes/cmdline.h"
#include "../../src/modes/modes.h"
#include "../../src/ui/ui.h"
#include "../../src/utils/filter.h"
#include "../../src/utils/fs.h"
#include "../../src/cmd_core.h"
#include "../../src/compare.h"
#include "../../src/filelist.h"
#include "../../src/filtering.h"
#include "../../src/flist_pos.h"
#include "../../src/running.h"

#include "utils.h"

static void basic_panes_check(int expected_len);
static int files_are_identical(const char a[], const char b[]);

SETUP()
{
	curr_view = &lwin;
	other_view = &rwin;

	view_setup(&lwin);
	view_setup(&rwin);

	opt_handlers_setup();

	cfg.delete_prg = strdup("");
}

TEARDOWN()
{
	view_teardown(&lwin);
	view_teardown(&rwin);

	opt_handlers_teardown();

	free(cfg.delete_prg);
}

TEST(files_are_compared_by_name)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/a");
	compare_one_pane(&lwin, CT_NAME, LT_ALL, 0);

	assert_int_equal(CV_COMPARE, lwin.custom.type);
	assert_int_equal(3, lwin.list_rows);
	assert_int_equal(1, lwin.dir_entry[0].id);
	assert_int_equal(2, lwin.dir_entry[1].id);
	assert_int_equal(3, lwin.dir_entry[2].id);
}

TEST(files_are_compared_by_size)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/a");
	compare_one_pane(&lwin, CT_SIZE, LT_ALL, 0);

	assert_int_equal(CV_COMPARE, lwin.custom.type);
	assert_int_equal(3, lwin.list_rows);
	assert_int_equal(1, lwin.dir_entry[0].id);
	assert_int_equal(2, lwin.dir_entry[1].id);
	assert_int_equal(3, lwin.dir_entry[2].id);
}

TEST(files_are_compared_by_contents)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/b");
	compare_one_pane(&lwin, CT_CONTENTS, LT_ALL, 0);

	assert_int_equal(CV_COMPARE, lwin.custom.type);
	assert_int_equal(4, lwin.list_rows);
	assert_int_equal(1, lwin.dir_entry[0].id);
	assert_int_equal(1, lwin.dir_entry[1].id);
	assert_int_equal(2, lwin.dir_entry[2].id);
	assert_int_equal(3, lwin.dir_entry[3].id);
}

TEST(two_panes_all_group_ids)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/a");
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/b");
	compare_two_panes(CT_NAME, LT_ALL, 0, 0);

	basic_panes_check(4);

	assert_int_equal(1, lwin.dir_entry[0].id);
	assert_int_equal(2, lwin.dir_entry[1].id);
	assert_int_equal(3, lwin.dir_entry[2].id);
	assert_int_equal(4, lwin.dir_entry[3].id);

	assert_string_equal("same-content-different-name-1", lwin.dir_entry[0].name);
	assert_string_equal("same-content-different-name-1", rwin.dir_entry[0].name);
	assert_string_equal("same-name-different-content", lwin.dir_entry[1].name);
	assert_string_equal("same-name-different-content", rwin.dir_entry[1].name);
	assert_string_equal("same-name-same-content", lwin.dir_entry[2].name);
	assert_string_equal("same-name-same-content", rwin.dir_entry[2].name);
	assert_string_equal("", lwin.dir_entry[3].name);
	assert_string_equal("same-content-different-name-2", rwin.dir_entry[3].name);
}

TEST(two_panes_all_group_paths)
{
	curr_view = &rwin;
	other_view = &lwin;
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/a");
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/b");
	compare_two_panes(CT_NAME, LT_ALL, 1, 0);

	basic_panes_check(4);

	assert_int_equal(1, lwin.dir_entry[0].id);
	assert_int_equal(2, lwin.dir_entry[1].id);
	assert_int_equal(3, lwin.dir_entry[2].id);
	assert_int_equal(4, lwin.dir_entry[3].id);

	assert_string_equal("same-content-different-name-1", lwin.dir_entry[0].name);
	assert_string_equal("same-content-different-name-1", rwin.dir_entry[0].name);
	assert_string_equal("", lwin.dir_entry[1].name);
	assert_string_equal("same-content-different-name-2", rwin.dir_entry[1].name);
	assert_string_equal("same-name-different-content", lwin.dir_entry[2].name);
	assert_string_equal("same-name-different-content", rwin.dir_entry[2].name);
	assert_string_equal("same-name-same-content", lwin.dir_entry[3].name);
	assert_string_equal("same-name-same-content", rwin.dir_entry[3].name);
}

TEST(two_panes_dups_one_is_empty)
{
	curr_view = &rwin;
	other_view = &lwin;
	strcpy(lwin.curr_dir, SANDBOX_PATH);
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/b");
	compare_two_panes(CT_CONTENTS, LT_ALL, 1, 0);

	basic_panes_check(4);

	assert_int_equal(1, lwin.dir_entry[0].id);
	assert_int_equal(1, lwin.dir_entry[1].id);
	assert_int_equal(2, lwin.dir_entry[2].id);
	assert_int_equal(3, lwin.dir_entry[3].id);

	assert_string_equal("", lwin.dir_entry[0].name);
	assert_string_equal("same-content-different-name-1", rwin.dir_entry[0].name);
	assert_string_equal("", lwin.dir_entry[1].name);
	assert_string_equal("same-content-different-name-2", rwin.dir_entry[1].name);
	assert_string_equal("", lwin.dir_entry[2].name);
	assert_string_equal("same-name-different-content", rwin.dir_entry[2].name);
	assert_string_equal("", lwin.dir_entry[3].name);
	assert_string_equal("same-name-same-content", rwin.dir_entry[3].name);
}

TEST(two_panes_dups)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/a");
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/b");
	compare_two_panes(CT_CONTENTS, LT_DUPS, 1, 0);

	basic_panes_check(3);

	assert_int_equal(1, lwin.dir_entry[0].id);
	assert_int_equal(1, lwin.dir_entry[1].id);
	assert_int_equal(2, lwin.dir_entry[2].id);

	assert_string_equal("same-content-different-name-1", lwin.dir_entry[0].name);
	assert_string_equal("same-content-different-name-1", rwin.dir_entry[0].name);
	assert_string_equal("", lwin.dir_entry[1].name);
	assert_string_equal("same-content-different-name-2", rwin.dir_entry[1].name);
	assert_string_equal("same-name-same-content", lwin.dir_entry[2].name);
	assert_string_equal("same-name-same-content", rwin.dir_entry[2].name);
}

TEST(two_panes_unique)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/a");
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/b");
	compare_two_panes(CT_CONTENTS, LT_UNIQUE, 1, 0);

	assert_int_equal(1, lwin.list_rows);
	assert_int_equal(1, rwin.list_rows);

	assert_string_equal("same-name-different-content", lwin.dir_entry[0].name);
	assert_string_equal("same-name-different-content", rwin.dir_entry[0].name);
}

TEST(single_pane_all)
{
	copy_file(TEST_DATA_PATH "/read/dos-eof", SANDBOX_PATH "/dos-eof-1");
	copy_file(TEST_DATA_PATH "/read/dos-eof", SANDBOX_PATH "/dos-eof-2");
	copy_file(TEST_DATA_PATH "/read/utf8-bom", SANDBOX_PATH "/utf8-bom-1");
	copy_file(TEST_DATA_PATH "/read/utf8-bom", SANDBOX_PATH "/utf8-bom-2");

	strcpy(lwin.curr_dir, SANDBOX_PATH);
	compare_one_pane(&lwin, CT_CONTENTS, LT_ALL, 0);

	assert_int_equal(CV_COMPARE, lwin.custom.type);
	assert_int_equal(4, lwin.list_rows);
	assert_string_equal("dos-eof-1", lwin.dir_entry[0].name);
	assert_int_equal(1, lwin.dir_entry[0].id);
	assert_int_equal(1, lwin.dir_entry[1].id);
	assert_string_equal("utf8-bom-1", lwin.dir_entry[2].name);
	assert_int_equal(2, lwin.dir_entry[2].id);
	assert_int_equal(2, lwin.dir_entry[3].id);

	assert_success(remove(SANDBOX_PATH "/dos-eof-1"));
	assert_success(remove(SANDBOX_PATH "/dos-eof-2"));
	assert_success(remove(SANDBOX_PATH "/utf8-bom-1"));
	assert_success(remove(SANDBOX_PATH "/utf8-bom-2"));
}

TEST(single_pane_dups)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare");
	compare_one_pane(&lwin, CT_CONTENTS, LT_DUPS, 0);

	assert_int_equal(CV_COMPARE, lwin.custom.type);
	assert_int_equal(5, lwin.list_rows);
	assert_string_equal("same-content-different-name-1", lwin.dir_entry[0].name);
	assert_int_equal(1, lwin.dir_entry[0].id);
	assert_int_equal(1, lwin.dir_entry[1].id);
	assert_int_equal(1, lwin.dir_entry[2].id);
	assert_string_equal("same-name-same-content", lwin.dir_entry[3].name);
	assert_int_equal(2, lwin.dir_entry[3].id);
	assert_int_equal(2, lwin.dir_entry[4].id);
}

TEST(single_pane_unique)
{
	copy_file(TEST_DATA_PATH "/read/dos-eof", SANDBOX_PATH "/dos-eof");
	copy_file(TEST_DATA_PATH "/read/utf8-bom", SANDBOX_PATH "/utf8-bom-1");
	copy_file(TEST_DATA_PATH "/read/utf8-bom", SANDBOX_PATH "/utf8-bom-2");

	strcpy(lwin.curr_dir, SANDBOX_PATH);
	compare_one_pane(&lwin, CT_CONTENTS, LT_UNIQUE, 0);

	assert_int_equal(CV_REGULAR, lwin.custom.type);
	assert_int_equal(1, lwin.list_rows);
	assert_string_equal("dos-eof", lwin.dir_entry[0].name);

	assert_success(remove(SANDBOX_PATH "/dos-eof"));
	assert_success(remove(SANDBOX_PATH "/utf8-bom-1"));
	assert_success(remove(SANDBOX_PATH "/utf8-bom-2"));
}

TEST(empty_root_directories_abort_single_comparison)
{
	strcpy(lwin.curr_dir, SANDBOX_PATH);
	compare_one_pane(&lwin, CT_CONTENTS, LT_ALL, 0);

	assert_false(flist_custom_active(&lwin));
}

TEST(empty_root_directories_abort_dual_comparison)
{
	strcpy(lwin.curr_dir, SANDBOX_PATH);
	strcpy(rwin.curr_dir, SANDBOX_PATH);

	compare_two_panes(CT_CONTENTS, LT_ALL, 0, 0);
	assert_false(flist_custom_active(&lwin));
}

TEST(empty_unique_cv_are_created)
{
	strcpy(lwin.curr_dir, SANDBOX_PATH);
	strcpy(rwin.curr_dir, SANDBOX_PATH);

	compare_two_panes(CT_CONTENTS, LT_UNIQUE, 0, 0);
	assert_true(flist_custom_active(&lwin));
	assert_true(flist_custom_active(&rwin));

	assert_int_equal(1, lwin.list_rows);
	assert_int_equal(1, rwin.list_rows);

	assert_string_equal("..", lwin.dir_entry[0].name);
	assert_string_equal("..", rwin.dir_entry[0].name);
}

TEST(listing_wrong_path_does_nothing)
{
	strcpy(lwin.curr_dir, SANDBOX_PATH "/does-not-exist");
	compare_one_pane(&lwin, CT_CONTENTS, LT_ALL, 0);

	assert_false(flist_custom_active(&lwin));
}

TEST(files_are_found_recursively)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/tree");
	compare_one_pane(&lwin, CT_CONTENTS, LT_ALL, 0);

	assert_int_equal(7, lwin.list_rows);
}

TEST(compare_skips_dir_symlinks, IF(not_windows))
{
	/* symlink() is not available on Windows, but other code is fine. */
#ifndef _WIN32
	assert_success(symlink(TEST_DATA_PATH, SANDBOX_PATH "/link"));
#endif

	strcpy(lwin.curr_dir, SANDBOX_PATH);
	compare_one_pane(&lwin, CT_CONTENTS, LT_ALL, 0);
	assert_false(flist_custom_active(&lwin));

	assert_success(remove(SANDBOX_PATH "/link"));
}

TEST(not_available_files_are_ignored, IF(not_windows))
{
	copy_file(TEST_DATA_PATH "/read/utf8-bom", SANDBOX_PATH "/utf8-bom");
	assert_success(chmod(SANDBOX_PATH "/utf8-bom", 0000));

	strcpy(lwin.curr_dir, SANDBOX_PATH);
	compare_one_pane(&lwin, CT_CONTENTS, LT_ALL, 0);
	assert_false(flist_custom_active(&lwin));

	assert_success(remove(SANDBOX_PATH "/utf8-bom"));
}

TEST(relatively_complex_match)
{
	copy_file(TEST_DATA_PATH "/read/dos-eof", SANDBOX_PATH "/dos-eof-1");
	copy_file(TEST_DATA_PATH "/read/dos-eof", SANDBOX_PATH "/dos-eof-2");
	copy_file(TEST_DATA_PATH "/read/utf8-bom", SANDBOX_PATH "/utf8-bom-1");
	copy_file(TEST_DATA_PATH "/read/utf8-bom", SANDBOX_PATH "/utf8-bom-2");

	curr_view = &rwin;
	other_view = &lwin;
	strcpy(lwin.curr_dir, SANDBOX_PATH);
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/read");
	compare_two_panes(CT_CONTENTS, LT_ALL, 1, 0);

	basic_panes_check(8);

	assert_int_equal(1, lwin.dir_entry[0].id);
	assert_int_equal(2, lwin.dir_entry[1].id);
	assert_int_equal(3, lwin.dir_entry[2].id);
	assert_int_equal(4, lwin.dir_entry[3].id);
	assert_int_equal(2, lwin.dir_entry[4].id);
	assert_int_equal(5, lwin.dir_entry[5].id);
	assert_int_equal(6, lwin.dir_entry[6].id);
	assert_int_equal(5, lwin.dir_entry[7].id);

	assert_string_equal("", lwin.dir_entry[0].name);
	assert_string_equal("binary-data", rwin.dir_entry[0].name);
	assert_string_equal("dos-eof-1", lwin.dir_entry[1].name);
	assert_string_equal("dos-eof", rwin.dir_entry[1].name);
	assert_string_equal("", lwin.dir_entry[2].name);
	assert_string_equal("dos-line-endings", rwin.dir_entry[2].name);
	assert_string_equal("", lwin.dir_entry[3].name);
	assert_string_equal("two-lines", rwin.dir_entry[3].name);
	assert_string_equal("dos-eof-2", lwin.dir_entry[4].name);
	assert_string_equal("", rwin.dir_entry[4].name);
	assert_string_equal("utf8-bom-1", lwin.dir_entry[5].name);
	assert_string_equal("utf8-bom", rwin.dir_entry[5].name);
	assert_string_equal("", lwin.dir_entry[6].name);
	assert_string_equal("very-long-line", rwin.dir_entry[6].name);
	assert_string_equal("utf8-bom-2", lwin.dir_entry[7].name);
	assert_string_equal("", rwin.dir_entry[7].name);

	assert_success(remove(SANDBOX_PATH "/dos-eof-1"));
	assert_success(remove(SANDBOX_PATH "/dos-eof-2"));
	assert_success(remove(SANDBOX_PATH "/utf8-bom-1"));
	assert_success(remove(SANDBOX_PATH "/utf8-bom-2"));
}

TEST(two_panes_are_left_in_sync)
{
	strcpy(lwin.curr_dir, SANDBOX_PATH);
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/a");

	compare_two_panes(CT_CONTENTS, LT_ALL, 0, 0);
	assert_true(flist_custom_active(&lwin));
	assert_true(flist_custom_active(&rwin));

	cd_updir(&lwin, 1);
	assert_false(flist_custom_active(&lwin));
	assert_false(flist_custom_active(&rwin));
}

TEST(exclude_works_with_entries_or_their_groups)
{
	copy_file(TEST_DATA_PATH "/compare/a/same-content-different-name-1",
			SANDBOX_PATH "/same-content-different-name-1");
	copy_file(TEST_DATA_PATH "/compare/a/same-content-different-name-1",
			SANDBOX_PATH "/same-content-different-name-2");
	copy_file(TEST_DATA_PATH "/compare/a/same-name-same-content",
			SANDBOX_PATH "/same-name-same-content");
	copy_file(TEST_DATA_PATH "/compare/a/same-name-same-content",
			SANDBOX_PATH "/same-name-same-content-2");

	strcpy(lwin.curr_dir, SANDBOX_PATH);
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/a");
	compare_two_panes(CT_CONTENTS, LT_ALL, 0, 0);
	basic_panes_check(5);

	/* Does nothing on separator. */
	lwin.selected_files = 1;
	lwin.dir_entry[4].selected = 1;
	flist_custom_exclude(&lwin, 1);
	basic_panes_check(5);

	/* Exclude single file from a group. */
	lwin.selected_files = 1;
	lwin.dir_entry[0].selected = 1;
	flist_custom_exclude(&lwin, 1);
	basic_panes_check(4);

	/* Exclude the whole group. */
	lwin.selected_files = 1;
	lwin.dir_entry[2].selected = 1;
	flist_custom_exclude(&lwin, 0);
	basic_panes_check(2);

	/* Exclusion of all files leaves the mode. */
	lwin.selected_files = 1;
	lwin.dir_entry[0].selected = 1;
	flist_custom_exclude(&lwin, 0);
	basic_panes_check(1);
	rwin.selected_files = 1;
	rwin.dir_entry[0].selected = 1;
	flist_custom_exclude(&rwin, 0);
	assert_false(flist_custom_active(&lwin));
	assert_false(flist_custom_active(&rwin));

	assert_success(remove(SANDBOX_PATH "/same-content-different-name-1"));
	assert_success(remove(SANDBOX_PATH "/same-content-different-name-2"));
	assert_success(remove(SANDBOX_PATH "/same-name-same-content"));
	assert_success(remove(SANDBOX_PATH "/same-name-same-content-2"));
}

TEST(local_filter_is_not_set)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/a");
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/b");
	compare_two_panes(CT_NAME, LT_ALL, 0, 0);

	exec_command("f", &lwin, CIT_FILTER_PATTERN);
	assert_true(filter_is_empty(&lwin.local_filter.filter));

	enter_cmdline_mode(CLS_FILTER, lwin.local_filter.filter.raw, NULL);
	assert_true(vle_mode_is(NORMAL_MODE));
}

TEST(removed_files_disappear_in_both_views_on_reload)
{
	copy_file(TEST_DATA_PATH "/compare/a/same-content-different-name-1",
			SANDBOX_PATH "/same-content-different-name-1");
	copy_file(TEST_DATA_PATH "/compare/a/same-content-different-name-1",
			SANDBOX_PATH "/same-content-different-name-2");
	copy_file(TEST_DATA_PATH "/compare/a/same-name-same-content",
			SANDBOX_PATH "/same-name-same-content");
	copy_file(TEST_DATA_PATH "/compare/a/same-name-same-content",
			SANDBOX_PATH "/same-name-same-content-2");

	strcpy(lwin.curr_dir, SANDBOX_PATH);
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/a");
	compare_two_panes(CT_CONTENTS, LT_ALL, 0, 0);
	basic_panes_check(5);

	assert_success(remove(SANDBOX_PATH "/same-content-different-name-1"));
	load_dir_list(&lwin, 1);
	basic_panes_check(4);
	assert_string_equal("same-content-different-name-1", rwin.dir_entry[0].name);

	assert_success(remove(SANDBOX_PATH "/same-content-different-name-2"));
	load_dir_list(&lwin, 1);
	basic_panes_check(4);
	assert_string_equal("", lwin.dir_entry[0].name);

	assert_success(remove(SANDBOX_PATH "/same-name-same-content"));
	assert_success(remove(SANDBOX_PATH "/same-name-same-content-2"));
}

TEST(comparison_views_are_closed_when_no_files_are_left)
{
	char *const saved_cwd = save_cwd();

	assert_success(os_mkdir(SANDBOX_PATH "/a", 0777));
	assert_success(os_mkdir(SANDBOX_PATH "/b", 0777));

	copy_file(TEST_DATA_PATH "/compare/a/same-content-different-name-1",
			SANDBOX_PATH "/a/same-content-different-name-1");
	copy_file(TEST_DATA_PATH "/compare/a/same-content-different-name-1",
			SANDBOX_PATH "/b/same-content-different-name-1");

	strcpy(lwin.curr_dir, SANDBOX_PATH "/a");
	strcpy(rwin.curr_dir, SANDBOX_PATH "/b");
	compare_two_panes(CT_CONTENTS, LT_ALL, 0, 0);
	basic_panes_check(1);

	assert_success(remove(SANDBOX_PATH "/a/same-content-different-name-1"));
	load_dir_list(&lwin, 1);
	basic_panes_check(1);
	assert_string_equal("", lwin.dir_entry[0].name);

	assert_success(remove(SANDBOX_PATH "/b/same-content-different-name-1"));
	load_dir_list(&lwin, 1);
	assert_false(flist_custom_active(&lwin));
	assert_false(flist_custom_active(&rwin));

	restore_cwd(saved_cwd);

	assert_success(rmdir(SANDBOX_PATH "/a"));
	assert_success(rmdir(SANDBOX_PATH "/b"));
}

TEST(sorting_is_not_changed)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/a");
	compare_one_pane(&lwin, CT_CONTENTS, LT_ALL, 0);

	change_sort_type(&lwin, SK_BY_SIZE, 0);
	assert_int_equal(SK_NONE, lwin.sort[0]);

	init_commands();
	assert_success(exec_commands("set sort=ext", &lwin, CIT_COMMAND));
	assert_int_equal(SK_NONE, lwin.sort[0]);
	reset_cmds();
}

TEST(cursor_moves_in_both_views_synchronously)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/a");
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/b");
	compare_two_panes(CT_NAME, LT_ALL, 0, 0);

	assert_int_equal(0, lwin.list_pos);
	assert_int_equal(lwin.list_pos, rwin.list_pos);

	flist_set_pos(&lwin, 2);
	assert_int_equal(2, lwin.list_pos);
	assert_int_equal(lwin.list_pos, rwin.list_pos);

	flist_set_pos(&rwin, 3);
	assert_int_equal(3, rwin.list_pos);
	assert_int_equal(lwin.list_pos, rwin.list_pos);
}

TEST(two_independent_compare_views_are_not_bound)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/a");
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/b");
	compare_one_pane(&lwin, CT_CONTENTS, LT_ALL, 0);
	compare_one_pane(&rwin, CT_CONTENTS, LT_ALL, 0);

	assert_int_equal(0, lwin.list_pos);
	assert_int_equal(lwin.list_pos, rwin.list_pos);

	flist_set_pos(&lwin, 2);
	flist_set_pos(&rwin, 3);
	assert_int_equal(2, lwin.list_pos);
	assert_int_equal(3, rwin.list_pos);

	cd_updir(&lwin, 1);
	assert_true(flist_custom_active(&rwin));
}

TEST(diff_is_closed_by_single_compare)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/a");
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/b");
	compare_two_panes(CT_NAME, LT_ALL, 0, 0);

	assert_int_equal(CV_DIFF, lwin.custom.type);
	assert_int_equal(CV_DIFF, rwin.custom.type);

	compare_one_pane(&lwin, CT_CONTENTS, LT_ALL, 0);
	assert_int_equal(CV_COMPARE, lwin.custom.type);
	assert_int_equal(CV_REGULAR, rwin.custom.type);
}

TEST(filtering_fake_entry_does_nothing)
{
	curr_view = &rwin;
	other_view = &lwin;
	strcpy(lwin.curr_dir, SANDBOX_PATH);
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/b");
	compare_two_panes(CT_CONTENTS, LT_ALL, 1, 0);

	assert_int_equal(4, lwin.list_rows);
	assert_int_equal(4, rwin.list_rows);
	assert_string_equal("", lwin.dir_entry[0].name);

	lwin.dir_entry[0].selected = 1;
	lwin.selected_files = 1;
	filter_selected_files(&lwin);

	assert_int_equal(4, lwin.list_rows);
	assert_int_equal(4, rwin.list_rows);
	assert_string_equal("", lwin.dir_entry[0].name);
}

TEST(filtering_updates_two_bound_views)
{
	curr_view = &rwin;
	other_view = &lwin;
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/a");
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/b");
	compare_two_panes(CT_CONTENTS, LT_ALL, 0, 0);

	/* Check that single file is excluded. */

	basic_panes_check(5);
	assert_string_equal("same-content-different-name-1", lwin.dir_entry[0].name);
	assert_string_equal("same-content-different-name-1", rwin.dir_entry[0].name);

	rwin.dir_entry[0].selected = 1;
	rwin.selected_files = 1;
	filter_selected_files(&rwin);

	basic_panes_check(4);
	assert_string_equal("same-content-different-name-1", lwin.dir_entry[0].name);
	assert_string_equal("same-content-different-name-2", rwin.dir_entry[0].name);

	/* Check that compare view is left when lists are empty. */

	rwin.dir_entry[0].selected = 1;
	rwin.dir_entry[1].selected = 1;
	rwin.dir_entry[2].selected = 1;
	rwin.selected_files = 3;
	filter_selected_files(&rwin);
	basic_panes_check(3);

	lwin.dir_entry[0].selected = 1;
	lwin.dir_entry[1].selected = 1;
	lwin.dir_entry[2].selected = 1;
	lwin.selected_files = 3;
	filter_selected_files(&lwin);
	assert_int_equal(CV_REGULAR, lwin.custom.type);
	assert_int_equal(CV_REGULAR, rwin.custom.type);
}

TEST(two_pane_dups_renumbering)
{
	copy_file(TEST_DATA_PATH "/read/utf8-bom", SANDBOX_PATH "/utf8-bom-1");
	copy_file(TEST_DATA_PATH "/read/utf8-bom", SANDBOX_PATH "/utf8-bom-2");

	curr_view = &rwin;
	other_view = &lwin;
	strcpy(lwin.curr_dir, SANDBOX_PATH);
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/read");
	compare_two_panes(CT_CONTENTS, LT_DUPS, 1, 0);

	basic_panes_check(2);

	assert_int_equal(1, lwin.dir_entry[0].id);
	assert_int_equal(1, lwin.dir_entry[1].id);

	assert_success(remove(SANDBOX_PATH "/utf8-bom-1"));
	assert_success(remove(SANDBOX_PATH "/utf8-bom-2"));
}

TEST(removing_all_files_of_same_id_and_fake_entry_on_the_other_side)
{
	copy_file(TEST_DATA_PATH "/read/binary-data", SANDBOX_PATH "/binary-data");
	copy_file(TEST_DATA_PATH "/read/dos-eof", SANDBOX_PATH "/dos-eof-1");
	copy_file(TEST_DATA_PATH "/read/dos-eof", SANDBOX_PATH "/dos-eof-2");

	strcpy(lwin.curr_dir, SANDBOX_PATH);
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/read");
	compare_two_panes(CT_CONTENTS, LT_ALL, 1, 0);

	basic_panes_check(7);

	assert_success(remove(SANDBOX_PATH "/dos-eof-1"));
	assert_success(remove(SANDBOX_PATH "/dos-eof-2"));
	load_dir_list(&lwin, 1);

	basic_panes_check(6);
	assert_int_equal(1, lwin.dir_entry[0].id);
	assert_int_equal(2, lwin.dir_entry[1].id);
	assert_int_equal(3, lwin.dir_entry[2].id);
	assert_int_equal(4, lwin.dir_entry[3].id);
	assert_int_equal(5, lwin.dir_entry[4].id);
	assert_int_equal(6, lwin.dir_entry[5].id);

	assert_success(remove(SANDBOX_PATH "/binary-data"));
}

TEST(compare_considers_dot_filter)
{
	lwin.hide_dot = 1;
	rwin.hide_dot = 1;
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/tree");
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/tree");
	compare_two_panes(CT_CONTENTS, LT_ALL, 1, 0);
	basic_panes_check(5);
}

TEST(empty_failes_are_skipped_if_requested)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/a");
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/b");
	compare_two_panes(CT_CONTENTS, LT_ALL, 0, 1);
	basic_panes_check(4);
}

TEST(custom_views_are_compared)
{
	strcpy(lwin.curr_dir, "no-such-path");
	flist_custom_start(&lwin, "test");
	flist_custom_add(&lwin,
			TEST_DATA_PATH "/compare/a/same-content-different-name-1");
	flist_custom_add(&lwin,
			TEST_DATA_PATH "/compare/a/same-name-different-content");
	flist_custom_add(&lwin, TEST_DATA_PATH "/compare/a/same-name-same-content");
	assert_true(flist_custom_finish(&lwin, CV_REGULAR, 0) == 0);

	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/b");

	compare_two_panes(CT_NAME, LT_ALL, 1, 0);

	basic_panes_check(4);

	assert_string_equal("same-content-different-name-1", lwin.dir_entry[0].name);
	assert_string_equal("same-content-different-name-1", rwin.dir_entry[0].name);
	assert_string_equal("", lwin.dir_entry[1].name);
	assert_string_equal("same-content-different-name-2", rwin.dir_entry[1].name);
	assert_string_equal("same-name-different-content", lwin.dir_entry[2].name);
	assert_string_equal("same-name-different-content", rwin.dir_entry[2].name);
	assert_string_equal("same-name-same-content", lwin.dir_entry[3].name);
	assert_string_equal("same-name-same-content", rwin.dir_entry[3].name);
}

TEST(moving_does_not_work_in_non_diff)
{
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/a");
	strcpy(rwin.curr_dir, TEST_DATA_PATH "/compare/b");
	(void)compare_one_pane(&lwin, CT_CONTENTS, LT_ALL, 0);

	(void)compare_move(&lwin, &rwin);

	assert_true(files_are_identical(
				TEST_DATA_PATH "/compare/a/same-content-different-name-1",
				TEST_DATA_PATH "/compare/b/same-content-different-name-1"));
}

TEST(moving_fake_entry_removes_the_other_file)
{
	strcpy(rwin.curr_dir, SANDBOX_PATH);
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/b");

	create_file(SANDBOX_PATH "/empty");

	(void)compare_two_panes(CT_CONTENTS, LT_ALL, 1, 0);
	rwin.list_pos = 4;
	lwin.list_pos = 4;
	(void)compare_move(&lwin, &rwin);

	assert_failure(remove(SANDBOX_PATH "/empty"));
}

TEST(moving_mismatched_entry_makes_files_equal)
{
	strcpy(rwin.curr_dir, SANDBOX_PATH);
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/b");

	copy_file(TEST_DATA_PATH "/compare/a/same-name-different-content",
			SANDBOX_PATH "/same-name-different-content");

	assert_false(files_are_identical(SANDBOX_PATH "/same-name-different-content",
				TEST_DATA_PATH "/compare/b/same-name-different-content"));

	(void)compare_two_panes(CT_CONTENTS, LT_ALL, 1, 0);
	rwin.list_pos = 2;
	lwin.list_pos = 2;
	(void)compare_move(&lwin, &rwin);

	assert_true(files_are_identical(SANDBOX_PATH "/same-name-different-content",
				TEST_DATA_PATH "/compare/b/same-name-different-content"));

	assert_success(remove(SANDBOX_PATH "/same-name-different-content"));
}

TEST(moving_equal_does_nothing)
{
	strcpy(rwin.curr_dir, SANDBOX_PATH);
	strcpy(lwin.curr_dir, TEST_DATA_PATH "/compare/b");

	copy_file(TEST_DATA_PATH "/compare/a/same-name-same-content",
			SANDBOX_PATH "/same-name-same-content");

	assert_true(files_are_identical(SANDBOX_PATH "/same-name-same-content",
				TEST_DATA_PATH "/compare/b/same-name-same-content"));

	(void)compare_two_panes(CT_CONTENTS, LT_ALL, 1, 0);

	/* Replace file unbeknownst to main code. */
	copy_file(TEST_DATA_PATH "/compare/a/same-name-different-content",
			SANDBOX_PATH "/same-name-same-content");
	assert_false(files_are_identical(SANDBOX_PATH "/same-name-same-content",
				TEST_DATA_PATH "/compare/b/same-name-same-content"));

	rwin.list_pos = 3;
	lwin.list_pos = 3;
	(void)compare_move(&lwin, &rwin);

	/* Check that file wasn't replaced. */
	assert_false(files_are_identical(SANDBOX_PATH "/same-name-same-content",
				TEST_DATA_PATH "/compare/b/same-name-same-content"));

	assert_success(remove(SANDBOX_PATH "/same-name-same-content"));
}

static void
basic_panes_check(int expected_len)
{
	int i;

	assert_int_equal(expected_len, lwin.list_rows);
	assert_int_equal(expected_len, rwin.list_rows);

	for(i = 0; i < expected_len; ++i)
	{
		assert_int_equal(lwin.dir_entry[i].id, rwin.dir_entry[i].id);
	}
}

int
files_are_identical(const char a[], const char b[])
{
	FILE *const a_file = fopen(a, "rb");
	FILE *const b_file = fopen(b, "rb");
	int a_data, b_data;

	if(a_file == NULL || b_file == NULL)
	{
		if(a_file != NULL)
		{
			fclose(a_file);
		}
		if(b_file != NULL)
		{
			fclose(b_file);
		}
		return 0;
	}

	do
	{
		a_data = fgetc(a_file);
		b_data = fgetc(b_file);
	}
	while(a_data != EOF && b_data != EOF);

	fclose(b_file);
	fclose(a_file);

	return a_data == b_data && a_data == EOF;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab cinoptions-=(0 : */
/* vim: set cinoptions+=t0 : */
