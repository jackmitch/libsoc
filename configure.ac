AC_INIT([libsoc], [0.6.5])

AC_CONFIG_SRCDIR([lib/gpio.c])
AC_CONFIG_MACRO_DIR([config/m4])
AC_CONFIG_AUX_DIR([config/autoconf])

AC_CANONICAL_SYSTEM
AC_PROG_CC
AM_INIT_AUTOMAKE()
AM_PROG_CC_C_O

LT_INIT

AC_SEARCH_LIBS([pthread_create, pthread_cancel],[pthread], , AC_MSG_WARN(["ERROR: Could not find pthread library"]))

AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--enable-debug], [Enable the debug code]))

AS_IF([test "x$enable_debug" != "xno"], [
  AC_DEFINE([DEBUG])
])

AC_ARG_ENABLE([board],
    AS_HELP_STRING([--enable-board=BOARD], [Enable installation of board config]))

AS_IF([test "x$enable_board" != "x"], [
  valid_boards=`ls contrib/board_files/ | grep -v Makefile | tr '\n' ' '`
  for x in $valid_boards ; do
    test "$x" = "$enable_board" && found=1
  done
  test -z "$found" && AC_MSG_ERROR([Invalid board name: $enable_board, must be one of: $valid_boards])
])

AC_SUBST([board], $enable_board)
AM_CONDITIONAL([BOARD], [test "x$enable_board" != x])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_FILES(Makefile lib/Makefile contrib/board_files/Makefile libsoc.pc)
AC_OUTPUT
