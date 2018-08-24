dnl ==================================
dnl Initialization
dnl ==================================
AC_INIT([stdect], [0.0.1], [jmartine@bsc.es])
AC_CANONICAL_SYSTEM

dnl AM_INIT_AUTOMAKE([silent-rules -Wall -Werror foreign])
AM_INIT_AUTOMAKE

AC_PROG_CXX
AM_PROG_AR
LT_INIT

AC_CONFIG_FILES([
                 Makefile
                 src/Makefile
                 src/trace-utils/Makefile
                 src/1-parse/Makefile
                 src/2-reduce/Makefile
                 src/3-loops-id/Makefile
                 src/4-loops-merge/Makefile
                 src/5-gui/Makefile
                 src/wx/Makefile
                 src/model/Makefile])

AC_CONFIG_MACRO_DIR([m4])


dnl ==================================
dnl General compiling flags
dnl ==================================
dnl AX_CFLAGS_WARN_ALL
dnl AX_CXXFLAGS_WARN_ALL

dnl ==================================
dnl Check dependencies 
dnl ==================================
AX_BOOST_BASE([1.36]) 
AX_BOOST_PROGRAM_OPTIONS
dnl AX_BOOST_SPLIT
AX_CHECK_ZLIB
WX_CONFIG_OPTIONS
WX_CONFIG_CHECK([3.0.0])
AX_PROG_WITH_WXPROPGRID_DIR
AX_PROG_WXPROPGRID_NAME

dnl ==================================
dnl Generating output
dnl ==================================
AC_OUTPUT
