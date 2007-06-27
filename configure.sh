#! /bin/sh

# local options:  ac_help is the help message that describes them
# and LOCAL_AC_OPTIONS is the script that interprets them.  LOCAL_AC_OPTIONS
# is a script that's processed with eval, so you need to be very careful to
# make certain that what you quote is what you want to quote.

# load in the configuration file
#
TARGET=cwatch
. ./configure.inc


# and away we go
#
AC_INIT cwatch

if ! AC_PROG_CC; then
    LOG "You need to have a functional C compiler to build cwatch"
    exit 1
fi

if ! AC_PROG_LEX; then
    LOG "You need lex or flex to build cwatch"
    exit 1
fi
if ! AC_PROG_YACC; then
    LOG "You need yacc or bison to build cwatch"
    exit 1
fi
if AC_CHECK_HEADERS pcre.h; then
    if ! AC_LIBRARY pcre_compile -lpcre ; then
	LOG "Your system appears to have pcre.h without the pcre library?"
	LOG "Please install it (http://www.pcre.org) to build cwatch"
	exit 1
    fi
else
    LOG "You must have the pcre library (http://www.pcre.org) to build cwatch"
    exit 1
fi


# don't bother to check for options.h unless we're not
# being built against lprng -- remember that magicfilter
# for lprng doesn't have any options.
#
if AC_CHECK_HEADERS basis/options.h; then
    if LIBS="-lbasis" AC_CHECK_FUNCS x_getopt; then
	AC_LIBS="$AC_LIBS -lbasis"
	HAVE_XGETOPT=T
    fi
fi

if [ "$HAVE_XGETOPT" ]; then
    AC_SUB XGETOPT ''
else
    AC_SUB XGETOPT options.o
fi

AC_CHECK_HEADERS malloc.h || AC_CHECK_HEADERS sys/malloc.h

save_AC_PATH=$AC_PATH
AC_PATH=/usr/lib:/usr/sbin:/usr/bin:/bin:/sbin MF_PATH_INCLUDE MAIL sendmail smail mail Mail
AC_PATH=$save_AC_PATH

AC_CHECK_FUNCS basename
AC_CHECK_FUNCS mmap
AC_CHECK_FUNCS memmove

AC_SUB VERSION `test -f VERSION && cat VERSION`

AC_OUTPUT Makefile cwatch.h cwatch.1
