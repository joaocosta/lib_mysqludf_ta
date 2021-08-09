AC_PREREQ(2.59)

AC_INIT(lib_mysqludf_ta, 0.1, joaocosta@zonalivre.org)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR(config)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(subdir-objects)

AC_PROG_CC
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL)dnl

m4_include(config/ax_lib_mysql.m4)
AX_LIB_MYSQL(,,AC_MSG_ERROR("Can't find mysql development library" ))

if test -n "$MYSQL_PLUGINDIR" ; then
  AC_MSG_NOTICE(setting libdir to mysql plugin dir $MYSQL_PLUGINDIR)
  libdir=$MYSQL_PLUGINDIR
  AC_SUBST(libdir)
fi

m4_include(config/ax_mysql_bin.m4)
AX_MYSQL_BIN

#AC_C_CONST
#AC_TYPE_SIZE_T
#AC_CHECK_HEADERS(limits.h syslimits.h)
AC_OUTPUT(Makefile)
