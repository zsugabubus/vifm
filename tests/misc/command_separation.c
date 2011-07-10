#include "seatest.h"

#include "../../src/commands.h"

static void
test_pipe(void)
{
	const char *buf;

	buf = "filter /a|b/";
	assert_int_equal(0, line_pos(buf, buf));
	assert_int_equal(0, line_pos(buf, buf + 1));
	assert_int_equal(2, line_pos(buf, buf + 9));

	buf = "filter 'a|b'";
	assert_int_equal(0, line_pos(buf, buf));
	assert_int_equal(0, line_pos(buf, buf + 1));
	assert_int_equal(2, line_pos(buf, buf + 9));

	buf = "filter \"a|b\"";
	assert_int_equal(0, line_pos(buf, buf));
	assert_int_equal(0, line_pos(buf, buf + 1));
	assert_int_equal(2, line_pos(buf, buf + 9));
}

static void
test_two_commands(void)
{
	const char buf[] = "apropos|locate";

	assert_int_equal(0, line_pos(buf, buf));
	assert_int_equal(0, line_pos(buf, buf + 1));
	assert_int_equal(0, line_pos(buf, buf + 7));
}

static void
test_set_command(void)
{
	const char *buf;

	buf = "set fusehome=\"a|b\"";
	assert_int_equal(0, line_pos(buf, buf));
	assert_int_equal(0, line_pos(buf, buf + 1));
	assert_int_equal(2, line_pos(buf, buf + 16));

	buf = "set fusehome='a|b'";
	assert_int_equal(0, line_pos(buf, buf));
	assert_int_equal(0, line_pos(buf, buf + 1));
	assert_int_equal(2, line_pos(buf, buf + 16));
}

static void
test_skip(void)
{
	const char *buf;

	buf = "set fusehome=a\\|b";
	assert_int_equal(0, line_pos(buf, buf));
	assert_int_equal(0, line_pos(buf, buf + 1));
	assert_int_equal(1, line_pos(buf, buf + 15));
}

void
test_command_separation(void)
{
	test_fixture_start();

	run_test(test_pipe);
	run_test(test_two_commands);
	run_test(test_set_command);
	run_test(test_skip);

	test_fixture_end();
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab : */
