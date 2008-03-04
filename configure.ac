#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([MVTH Image Plugin], [1.5.0], [Sam Bromley <sam@sbnumerics.ca>], [mvthimage])
AM_INIT_AUTOMAKE(mvthimage,[1.5.0])

AC_CONFIG_SUBDIRS(libltdl)

AC_CONFIG_HEADERS([config.h])


# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_PROG_LIBTOOL

# set versions of the built libraries
AC_SUBST(libmvthimageversion,["0:2:0"])

# Checks for libraries.

# Dynamic loading Library
AC_CHECK_LIB([dl], [dlopen],[],[echo dl library not found; exit 0;])

# Math Library
AC_CHECK_LIB([m], [sqrt],[],[echo math library not found; exit 0;])

# Mpeg2 Library
AC_CHECK_LIB([mpeg2], [mpeg2_init],[MPEG2_LIBS="-lmpeg2 -lmpeg2convert"],[echo mpeg2 library not found; echo Not enabling MPEG capabilities.;])
if test "x$MPEG2_LIBS" != "x"; then
	AC_DEFINE(HAVE_LIBMPEG2,1,[Define if mpeg2 library is available])
	AC_SUBST(MPEG2_LIBS,["$MPEG2_LIBS"])
fi
AM_CONDITIONAL([COND_MPEG],[test "x$MPEG2_LIBS" != "x"])

# MNG Library
AC_CHECK_LIB([mng], [mng_initialize],[MNG_LIBS=-lmng],[echo MNG library not found; echo Not enabling MNG capabilities.;])
if test "x$MNG_LIBS" != "x"; then
	AC_DEFINE(HAVE_LIBMNG,1,[Define if MNG library is available])
	AC_SUBST(MNG_LIBS,["$MNG_LIBS"])
fi
AM_CONDITIONAL([COND_MNG],[test "x$MNG_LIBS" != "x"])

# GNU Libtool dynamic loading Library
AC_CHECK_LIB([ltdl], [lt_dlopen],[],[echo libtool dynamic library not found; exit 0;])

# Freetype font library.
AC_CHECK_FT2

#Pango font library.
AC_CHECK_LIB([pangoft2-1.0], [pango_ft2_render],[pangolibs=`pkg-config --libs pangoft2`],[echo Pango font library not found; exit 0;],[`pkg-config --libs pangoft2`])
if test "x$pangolibs" != "x"; then
	AC_DEFINE(HAVE_PANGOLIBS,1,[Define if the Pango font libraries are available])
	AC_SUBST(PANGO_LIBS,["$pangolibs"])
	pangocflags=`pkg-config --cflags pangoft2`
	AC_SUBST(PANGO_CFLAGS,["$pangocflags"])
fi

# Lib DevIL image Library
AC_CHECK_LIB([IL], [ilInit],[devillibs=-lIL],[echo DevIL library not found; exit 0;])
if test "x$devillibs" != "x"; then
	AC_CHECK_HEADERS([IL/il.h],[],[echo lib DevIL header not found.; exit 0;],[])
  AC_DEFINE(HAVE_LIBIL,1,[Define if the DevIL image library is available])
  AC_SUBST(DEVIL_LIBS,["$devillibs"])
fi


# SDL Libraries
# The following macro should work, but give problems, so I do it myself
#AM_PATH_SDL([1.2.8])
#do this instead
AC_CHECK_LIB([SDL], [SDL_Init],[sdllibs="`sdl-config --libs`"],[echo SDL library not found; exit 0;],[-lpthread])
if test "x$sdllibs" != "x"; then
	AC_CHECK_HEADERS([SDL/SDL.h],[],[echo SDL header not found.; exit 0;],[])
  AC_DEFINE(HAVE_LIBSDL,1,[Define if sdl library is available])
	sdlcflags=`sdl-config --cflags`
	AC_SUBST(SDL_LIBS,["$sdllibs"])
	AC_SUBST(SDL_CFLAGS,["$sdlcflags"])
fi

# Tcl libraries
# Use the TEA (Tcl Extention Architecture) macros (in tcl.m4).
TEA_INIT([3.6])
# Load the tclConfig.sh file
# adds the --with-tcl configure option.
TEA_PATH_TCLCONFIG
TEA_LOAD_TCLCONFIG
# Load the tkConfig.sh file
# adds the --with-tk configure option.
TEA_PATH_TKCONFIG
TEA_LOAD_TKCONFIG
# Define path to Tcl/Tk headers
# adds the --with-tclincludes configure option.
# and defines TCL_INCLUDES
TEA_PUBLIC_TCL_HEADERS
# If Tcl was compiles with threads, the following line enables threads here.
TEA_ENABLE_THREADS
# We should be using the Tcl/Tk stubs libraries.
AC_ARG_ENABLE([tclstubs],
 AS_HELP_STRING([--enable-tclstubs],[use Tcl stubs interface (default is YES)]),
 [case "${enableval}" in
   yes) ac_cv_enable_tclstubs=$enableval;;
	 no)  ac_cv_enable_tclstubs=$enableval;;
	 *) AC_MSG_ERROR([bad value ${enableval} for --enable-tclstubs]);;
	esac],[ac_cv_enable_tclstubs=yes])
AC_CACHE_CHECK([whether to use Tcl stubs],
               [ac_cv_enable_tclstubs],
							 [ac_cv_enable_tclstubs=yes])
AM_CONDITIONAL([COND_TCLSTUBS],[test "x$ac_cv_enable_tclstubs" = "xyes"])
if test "x${ac_cv_enable_tclstubs}" = "xyes"; then
	AC_DEFINE(USE_TCL_STUBS,1,[Use Tcl stubs])
	AC_DEFINE(USE_TK_STUBS,1,[Use Tk stubs])
fi

if test "x" = "y" ; then
# Don't use these /usr/share/aclocal/tcl.m4 macros;
SC_PATH_TCLCONFIG
SC_LOAD_TCLCONFIG_WITH_INCLUDES
#SC_LOAD_TCLCONFIG
SC_PATH_TKCONFIG
SC_LOAD_TKCONFIG
fi

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h float.h limits.h stdlib.h string.h sys/time.h sys/timeb.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_MODE_T
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRCOLL
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit ftime ftruncate getpagesize gettimeofday memmove memset mkdir munmap rmdir sqrt strchr strdup strerror strrchr strtol])

AC_DEFINE(INTERP,1,[do linear interpolation for resizing?])
AH_TEMPLATE(DEBUG,turn on extra debugging checks/output)

# Should we build the mvthplugin in addition to the regular library? 
AC_ARG_ENABLE(mvthplugin,AS_HELP_STRING([--enable-mvthplugin],[build mvth plugin interface (default is YES)]),[ac_cv_enable_mvthplugin=$enableval],[ac_cv_enable_mvthplugin=yes])
AC_CACHE_CHECK([whether to build mvth plugin] , [ac_cv_enable_mvthplugin], [ac_cv_enable_mvthplugin=yes])
AM_CONDITIONAL([COND_MVTHPLUGIN],[test "x$ac_cv_enable_mvthplugin" = "xyes"])

AC_ARG_WITH(memutils,AS_HELP_STRING([--with-memutils],[specify where memUtils library is installed (default is /usr/local/lib/memutils)]),[ac_with_memutils=$withval],[ac_with_memutils="/usr/local/lib/memutils"])
AC_CACHE_CHECK([location of memUtils] , [ac_with_memutils], [ac_with_memutils="/usr/local/lib/memutils"])
AC_SUBST(memutils,["$ac_with_memutils"])

AC_ARG_WITH(tclargv,AS_HELP_STRING([--with-tclargv],[specify where tclArgv library is installed (default is /usr/local/lib/tclargv)]),[ac_with_tclargv=$withval],[ac_with_tclargv="/usr/local/lib/tclargv"])
AC_CACHE_CHECK([location of tclArgv] , [ac_with_tclargv], [ac_with_tclargv="/usr/local/lib/tclargv"])
AC_SUBST(tclargv,["$ac_with_tclargv"])

AC_ARG_WITH(mvth,AS_HELP_STRING([--with-mvth],[specify where mvth libraries are installed (default is /usr/local/lib/mvth)]),[ac_with_mvth=$withval],[ac_with_mvth="/usr/local/lib/mvth"])
AC_CACHE_CHECK([location of mvth] , [ac_with_mvth], [ac_with_mvth="/usr/local/lib/mvth"])
AC_SUBST(mvth,["$ac_with_mvth"])

AC_CONFIG_FILES([Makefile
                 src/Makefile
								 src/utils/Makefile
								 src/interface/Makefile
								 tools/Makefile
								 tools/sdlviewer/Makefile
								 scripts/Makefile])
AC_OUTPUT
