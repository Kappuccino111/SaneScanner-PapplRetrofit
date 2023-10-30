AC_INIT([Sane_Scanner], [1.0])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC

AC_CHECK_HEADERS([stdio.h stdlib.h string.h])

AC_CHECK_LIB([sane], [sane_init])

AC_CHECK_LIB([microhttpd], [MHD_start_daemon])

AC_CHECK_HEADERS([sane/sane.h microhttpd.h])

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
