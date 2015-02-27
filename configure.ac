AC_PREREQ([2.69])


AC_INIT([rux], [0.81], [anikinsl@gmail.com])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h:config.in)

AC_PROG_CC
AC_HEADER_STDC
AM_MAINTAINER_MODE

dnl AC_CHECK_HEADERS(stddef.h)
AC_CHECK_LIB(iconv, iconvctl)
dnl AC_EGREP_HEADER(utimbuf, utime.h, AC_DEFINE(HAVE_STRUCT_UTIMBUF))
dnl AC_FUNC_UTIME_NULL
AH_TEMPLATE([USE_ICONV], [If having iconv.h we able to convert text to utf-8])
AC_CHECK_HEADERS(iconv.h, AC_DEFINE(USE_ICONV))

dnl Check OS for os_dep files.
uname=`uname`
AC_MSG_CHECKING(for OS type)
case "$uname" in
  *inux*)
    AC_MSG_RESULT($uname found.)
    OUTCP="5"
  ;;
  *olaris*)
    AC_MSG_RESULT($uname found.)
    OUTCP="2"
  ;;
  *SunOS*)
    AC_MSG_RESULT($uname found.)
    OUTCP="2"
  ;;
  *OpenBSD*)
    AC_MSG_RESULT($uname found.)
    OUTCP="2"
  ;;
  *BSD*)
    AC_MSG_RESULT($uname found.)
    OUTCP="2"
  ;;
  *)
    AC_MSG_RESULT(Unknown OS. Using default codepage koi8-r.)
    OUTCP="2"
  ;;
esac
AC_DEFINE_UNQUOTED(DEFAULT_OUTPUT_CP, $OUTCP, [ Default output code page. ])

AC_ARG_WITH(default-codepage,
        AC_HELP_STRING([--with-default-codepage=[OUT_CP_NAME]],
        [Defines the default output code page]),
[       case "$withval" in
        *1251*)         OUTCP=1
                        ;;
        *866*)          OUTCP=0
                        ;;
        *iso*)          OUTCP=3
                        ;;
        *mac*)          OUTCP=4
                        ;;
        *utf*8*)        OUTCP=5
                        ;;
        *)              OUTCP=2
                        ;;
        esac
        AC_DEFINE_UNQUOTED(DEFAULT_OUTPUT_CP, $OUTCP, [ Default output code page. ])
])

AC_ARG_ENABLE(debug,
        AC_HELP_STRING([--enable-debug],
                [Enables the debug information output.]),
        AC_DEFINE(DEBUG, 1, [ Turning on debug mode. ]))

AC_ARG_ENABLE(memdebug,
        AC_HELP_STRING([--enable-memdebug],
                [Enables the memory debug information output.]),
        AC_DEFINE(MEMDEBUG, 1, [ Turning on memory debug mode. ]))

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
