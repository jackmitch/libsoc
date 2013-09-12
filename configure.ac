AC_INIT([libsoc], [0.2])
AM_INIT_AUTOMAKE()

AC_CONFIG_SRCDIR([lib/gpio.c])

AC_CANONICAL_SYSTEM
AC_PROG_CC
AM_PROG_CC_C_O

LT_INIT

AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--enable-debug], [Enable the debug code]))

AS_IF([test "x$enable_debug" = "xyes"], [
  AC_DEFINE([DEBUG])
])

AC_CONFIG_FILES(Makefile lib/Makefile)
AC_OUTPUT
