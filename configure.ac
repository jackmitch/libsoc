AC_INIT([libsoc], [0.8.2])

AC_CONFIG_SRCDIR([lib/gpio.c])
AC_CONFIG_MACRO_DIR([config/m4])
AC_CONFIG_AUX_DIR([config/autoconf])

AC_CANONICAL_SYSTEM
AC_PROG_CC
AM_INIT_AUTOMAKE([foreign])
AM_PROG_CC_C_O

LT_INIT

AC_SEARCH_LIBS([pthread_create, pthread_cancel],[pthread], , AC_MSG_WARN(["ERROR: Could not find pthread library"]))

AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--enable-debug], [Enable the debug code]))

AS_IF([test "x$enable_debug" != "xno"], [
  AC_DEFINE([DEBUG])
])

AC_ARG_WITH([board-configs],
    AS_HELP_STRING([--with-board-configs], [Install all board config files]))
AM_CONDITIONAL([BOARD_CONFIGS], [test "x$with_board_configs" = xyes])

AC_ARG_ENABLE([python],
    AS_HELP_STRING([--enable-python=PYTHON_VERSION],
    [Enable Python language bindings to libsoc API. PYTHON_VERSION can be empty for autodetect, the value 2 or 3 to search
	the PATH environment variable for python2 or python3, or an absolute path to a python binary]),
    [python_arg=$enable_python], [python_arg="no"])

AS_IF([test "x$python_arg" != xno], [

	AS_CASE(["$python_arg"],
	  [2|3], [
	    PYTHON="python$python_arg"
	  ],
	  [yes], [],
	  [PYTHON="$python_arg"]
	)

	AM_PATH_PYTHON()

	if ! which "$PYTHON"; then
		AC_MSG_ERROR([Python interpreter $PYTHON does not exist])
	fi

	PKG_CHECK_MODULES([PYTHON], [python-"$PYTHON_VERSION"])
])

AM_CONDITIONAL([HAVE_PYTHON], [test "x$PYTHON" != x])

AC_ARG_ENABLE([board],
    AS_HELP_STRING([--enable-board=BOARD], [Enable installation of board config]))

AS_IF([test "x$enable_board" != "x"], [
  valid_boards=`ls $srcdir/contrib/board_files/ | grep .conf | tr '\n' ' ' | sed -e 's/.conf//g' -e 's/example//'`
  for x in $valid_boards ; do
    test "$x" = "$enable_board" && found=1
  done
  test -z "$found" && AC_MSG_ERROR([Invalid board name: $enable_board, must be one of: $valid_boards])
])

AC_SUBST([board], $enable_board)
AM_CONDITIONAL([BOARD], [test "x$enable_board" != x])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_FILES(Makefile lib/Makefile contrib/board_files/Makefile bindings/python/Makefile libsoc.pc)
AC_OUTPUT
