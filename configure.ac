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

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes]]))

AC_CONFIG_FILES(Makefile lib/Makefile libsoc.pc)
AC_OUTPUT
