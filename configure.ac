#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# ===============================================================================================
# ToDo:
# -------------
# * improve agar detection (check for version >= 1.4.1)
#
# Changes (FrMo, Mai 2014):
# -------------
# * debugging support added ( ./configure --enable-debug)
# * improved intel icc support
# * updated to autoconf 2 syntax
# * updated acinclude/ with latest versions from gnu autoconf-archive
#
# Changes (FrMo, 2013):
# -------------
# * improved lua detection, add support for LUA 5.2
#
# Changes (FrMo, April 2012):
# -------------
# * auto-detect agar library
#
# Changes (FrMo, Sept 2011):
# -------------
# * better linux support
# * silent build
# * try some compiler-specific optimization flags (gcc, intel icc, open64)
#
# Changes (FrMo, Aug 2011):
# -------------
# * minor updates needed for newer autoconf versions
# * added acinclude
# * windres support (windows resources)
# * Fixed LUA detection      (LUA >= 5.1 does not need lualib)
# * Fix GL and GLU detection (other lib names required on Win32) - borrowed from SDL_ttf
# * support for new features (-DWITH_LUA -DHAVE_LUA -DHAVE_SSE -DHAVE_PNG -DUSE_PTHREAD)
#   - Detect OpenMP support and add -fopenmp to CFLAGS and LDFLAGS
#   - Detect SSE support    and define -DHAVE_SSE
#   - Detect libPNG         and define -DHAVE_PNG
#
# ===============================================================================================


AC_PREREQ([2.69])
AC_INIT([gravit],[0.5.1],[gravit@slowchop.com])
AC_CONFIG_SRCDIR([src/config.c])
AC_CONFIG_HEADER(src/config.h)

dnl Detect the canonical build and host environments
AC_CANONICAL_HOST

dnl Setup for automake
AM_INIT_AUTOMAKE([subdir-objects])
#Enable quiet compiles on automake 1.11.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug],
                              [reduce optimization and enable debugging info (default is no)])],
              [], [enable_debug=no])

AM_CONDITIONAL([DEBUG], [test x$enable_debug = xyes])


# set default optimization flags
if test x"$enable_debug" = x"yes"; then
: ${CFLAGS="-Wall -O -g"}
fi
: ${CFLAGS="-Wall -O3"}

# Checks for programs.
AC_PROG_CC
AC_LANG([C])
AC_EXEEXT
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AX_COMPILER_VENDOR

# check for OpenMP support
#AX_OPENMP
AC_OPENMP
AC_CHECK_HEADERS([omp.h],[
	CFLAGS="$CFLAGS $OPENMP_CFLAGS"
	CPPFLAGS="$CPPFLAGS $OPENMP_CFLAGS"
	LDFLAGS="$LDFLAGS $OPENMP_CFLAGS"
     ])

# if openMP is not supported -> try pthread
if test x"$ac_cv_prog_c_openmp" = x"unsupported" ; then
     AC_CHECK_HEADERS([pthread.h],[
        CFLAGS="$CFLAGS -pthread"
        CPPFLAGS="$CPPFLAGS -pthread"
        LDFLAGS="$LDFLAGS -pthread"
     ])
fi

# check for supported SIMD extensions (gcc specific)
# if SSE is supported, HAVE_SSE will be set
if test x"$ax_cv_c_compiler_vendor" != x"intel"; then
    AX_EXT_SSE
else
    # skip SSE cpuid test for intel compiler
    ax_cv_have_sse_ext=yes
    AC_DEFINE(HAVE_SSE,,[Support SSE (Streaming SIMD Extensions) instructions])
    AX_CHECK_COMPILE_FLAG(-xSSE2, SIMD_FLAGS="-xSSE2")
    #AX_CHECK_COMPILE_FLAG(-axAVX, SIMD_FLAGS="-axAVX")
    #AX_CHECK_COMPILE_FLAG(-axCORE-AVX2 -fma, SIMD_FLAGS="-axCORE-AVX2 -fma")
    AC_SUBST(SIMD_FLAGS)
fi

CFLAGS="$CFLAGS $SIMD_FLAGS -I/usr/local/include"
LDFLAGS="$SIMD_FLAGS $LDFLAGS -L/usr/local/lib"

# windows gcc needs -mstackrealign (to avoid crash on XP)
if test x"$ax_cv_c_compiler_vendor" = x"gnu" ; then
    if test x"$host" = x"i686-pc-mingw32" ; then
        CFLAGS="$CFLAGS -mstackrealign"
        LDFLAGS="$LDFLAGS -mstackrealign"
    fi
fi

# try to add compiler-specific optimization flags
if test x"$ax_cv_c_compiler_vendor" = x"intel"; then
   #check for intel (icc) style flags
   if test x"$enable_debug" != x"yes"; then
      AX_CHECK_COMPILE_FLAG(-ipo, CFLAGS="$CFLAGS -O3 -ipo")
      AX_CHECK_COMPILE_FLAG(-parallel, CFLAGS="$CFLAGS -parallel")
      AX_CHECK_COMPILE_FLAG(-fomit-frame-pointer, CFLAGS="$CFLAGS -fomit-frame-pointer")
   else
      AX_CHECK_COMPILE_FLAG(-g -debug all, CFLAGS="$CFLAGS -g -debug all")
   fi

   AX_CHECK_COMPILE_FLAG(-no-prec-div, CFLAGS="$CFLAGS -no-prec-div")
   AX_CHECK_COMPILE_FLAG(-static-intel, CFLAGS="$CFLAGS -static-intel")

else
   #check for gcc-style optimization flags
   if test x"$enable_debug" != x"yes"; then
      ## gcc: optimize for speed, no debugging
      AX_CHECK_COMPILE_FLAG(-Ofast, CFLAGS="$CFLAGS -Ofast" LDFLAGS="$LDFLAGS -Ofast")
      AX_CHECK_COMPILE_FLAG(-fomit-frame-pointer, CFLAGS="$CFLAGS -fomit-frame-pointer")
      ## enable AVX2 (incl. FMA) in gcc or clang
      # AX_CHECK_COMPILE_FLAG(-mavx2, CFLAGS="$CFLAGS -mavx2")
      # AX_CHECK_COMPILE_FLAG(-march=native, CFLAGS="$CFLAGS -march=native")
      # AX_CHECK_COMPILE_FLAG(-ffp-contract=fast, CFLAGS="$CFLAGS -ffp-contract=fast")
   else
      AX_CHECK_COMPILE_FLAG(-O2, CFLAGS="$CFLAGS -O2" LDFLAGS="$LDFLAGS -O2")
      AX_CHECK_COMPILE_FLAG(-g3, CFLAGS="$CFLAGS -g3")
      ## gcc: try to enable sanitizers
      # AX_CHECK_COMPILE_FLAG(-fsanitize=address, CFLAGS="$CFLAGS -fsanitize=address" LDFLAGS="$LDFLAGS -fsanitize=address")
      # AX_CHECK_COMPILE_FLAG(-fsanitize=thread, CFLAGS="$CFLAGS -fsanitize=thread" LDFLAGS="$LDFLAGS -fsanitize=thread")
      AX_CHECK_COMPILE_FLAG(-fsanitize=undefined, CFLAGS="$CFLAGS -fsanitize=undefined" LDFLAGS="$LDFLAGS -fsanitize=undefined")

   fi

   AX_CHECK_COMPILE_FLAG(-ffast-math, CFLAGS="$CFLAGS -ffast-math")
fi


# Checks for header files.
AC_HEADER_STDC
AC_HEADER_DIRENT
AC_HEADER_TIME
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h sys/timeb.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T

dnl get X11 default libs
AC_PATH_XTRA
CPPFLAGS="$CPPFLAGS $X_CFLAGS"
LIBS="$X_PRE_LIBS $LIBS $X_LIBS $X_EXTRA_LIBS"

#check if we need to link with -lm
AC_CHECK_LIB([m], [pow])

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
# check AC_FUNC_REALLOC may end up in linker error: undefined rpl_realloc
# AC_FUNC_MALLOC
# AC_FUNC_REALLOC
AC_CHECK_FUNCS([malloc realloc])
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit floor ftime memset mkdir pow select setenv sqrt strchr strdup strerror strstr])

# check for functions to do aligned malloc
if test x"$ax_cv_c_compiler_vendor" != x"intel"; then
    AC_CHECK_HEADERS([intrin.h x86intrin.h xmmintrin.h malloc.h memory.h])
else
    #intel compilers sometimes do not like x86intrin.h on linux (ubuntu 14.10)
    AC_CHECK_HEADERS([xmmintrin.h malloc.h memory.h])
fi
AC_CHECK_FUNCS([memalign _mm_malloc _mm_free _aligned_malloc __mingw_aligned_malloc])
# AC_CHECK_FUNCS([posix_memalign])
AX_FUNC_POSIX_MEMALIGN
if test "$ax_cv_func_posix_memalign_works" = "yes" ; then
   AC_DEFINE([HAVE_WORKING_POSIX_MEMALIGN], [1], [use 'posix_memalign' instead of memalign])
fi


# check for windres (windows ressource compiler)
if test -z "$host_alias"; then
    hostaliaswindres=
else
    hostaliaswindres="$host_alias-windres"
fi
AC_CHECK_PROGS(WINDRES, [windres $hostaliaswindres $host_os-windres])
if test x$WINDRES != x; then
     use_icon_rc=true
fi
AC_SUBST(WINDRES)
AM_CONDITIONAL(USE_ICON_RC, test x$use_icon_rc = xtrue)


dnl Call in pkg-config explicitly, so we can safely put the first
dnl PKG_CHECK_EXISTS inside an if
PKG_PROG_PKG_CONFIG

dnl Check for SDL
SDL_VERSION=1.2.10
AC_SUBST(SDL_VERSION)
AM_PATH_SDL($SDL_VERSION, :, [AC_MSG_ERROR(SDL is required for Gravit)])
CFLAGS="$CFLAGS $SDL_CFLAGS"
CPPFLAGS="$CPPFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

# Checks for libraries.
AC_CHECK_LIB([SDL_image], [IMG_Load], [], [AC_MSG_ERROR(SDL_image is required)])
AC_CHECK_LIB([SDL_ttf], [TTF_Init], [], [AC_MSG_ERROR(SDL_ttf is required)])

dnl Check for OpenGL - try pkg-config first
PKG_CHECK_EXISTS([gl], [have_gl=yes], [have_gl=no])
PKG_CHECK_EXISTS([glu], [have_glu=yes], [have_glu=no])
if test x"$have_gl" = x"yes" -a  x"$have_glu" = x"yes" ; then
   PKG_CHECK_MODULES(GL, [gl])
   PKG_CHECK_MODULES(GLU, [glu])
   #GLU requires GL, so it's enough to use GLU_CFLAGS and GLU_LIBS
   CFLAGS="$CFLAGS $GLU_CFLAGS"
   LIBS="$LIBS $GLU_LIBS"
else
dnl Check for OpenGL - borrowed from SDL_ttf
   case "$host" in
    *-*-cygwin* | *-*-mingw*)
        MATHLIB=""
        SYS_GL_LIBS="-lopengl32 -lglu32"
        ;;
    *-*-beos*)
        MATHLIB=""
        SYS_GL_LIBS="-lGL -lGLU"
        ;;
    *-*-darwin*)
        MATHLIB=""
	# cocoa video
        SYS_GL_LIBS="-Wl,-framework,OpenGL"
	# carbon video
        #SYS_GL_LIBS="-Wl,-framework,AGL"
        ;;
    *-*-aix*)
        if test x$ac_cv_c_compiler_gnu = xyes; then
            CFLAGS="-mthreads"
	fi
        SYS_GL_LIBS=""
        ;;
    *)
        MATHLIB="-lm"
        AC_PATH_X
        AC_PATH_XTRA
        if test x$have_x = xyes; then
            CFLAGS="$CFLAGS $X_CFLAGS"
            SYS_GL_LIBS="$X_LIBS -lGL -lGLU"
        else
            SYS_GL_LIBS="-lGL -lGLU"
        fi
        ;;
   esac
   AC_MSG_CHECKING(for OpenGL support)
   have_opengl=no
   AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
    #include "SDL_opengl.h"
   ]], [[
    GLuint texture;
   ]])],[
   have_opengl=yes
   ],[])
   AC_MSG_RESULT($have_opengl)
   if test x$have_opengl = xyes; then
       GL_LIBS="$SYS_GL_LIBS"
   else
       GL_LIBS=""
       AC_MSG_ERROR(GL is required)
   fi
   LIBS="$LIBS $SYS_GL_LIBS $MATHLIB"
fi

dnl Check for lua - borrowed from lighttpd
AC_MSG_CHECKING(if lua support is requested)
AC_ARG_WITH(lua, AS_HELP_STRING([--with-lua],[LUA support for custom spawn locations (recommended)]), [WITH_LUA=$withval], [WITH_LUA=yes])

AC_MSG_RESULT($WITH_LUA)
if test "$WITH_LUA" != "no"; then
 AM_CONDITIONAL(LUA, true)

 #try lua-config
 AC_PATH_PROG(LUACONFIG, lua-config)
 if test x"$LUACONFIG" != x; then
   LUA_CFLAGS=`$LUACONFIG --include`
   LUA_LIBS=`$LUACONFIG --libs --extralibs`
   AC_DEFINE([HAVE_LUA], [1], [liblua])
   AC_DEFINE([HAVE_LUA_H], [1], [lua.h])
   AC_MSG_RESULT($LUACONFIG)
 else

   #try pkgconfig
   AC_MSG_CHECKING(for pkg-config lua)
   for lua_name in lua5.2 lua-5.2 lua52 lua5.1 lua-5.1 lua51 lua5.3 lua-5.3 lua53 lua
   do
       PKG_CHECK_EXISTS([$lua_name], [LUA_NAME=$lua_name; have_lua=yes; break], [have_lua=no])
   done

   if test x$have_lua = xyes
   then
       AC_MSG_RESULT($LUA_NAME)
       PKG_CHECK_MODULES(LUA,$LUA_NAME >= 5.0,[have_lua=yes], [have_lua=no])
   else
       AC_MSG_RESULT(no)
   fi

   if test x$have_lua = xyes; then
     AC_DEFINE([HAVE_LUA], [1], [liblua])
     AC_DEFINE([HAVE_LUA_H], [1], [lua.h])
   fi

   # fallback: try to find the library directly

   if test x"$LUA_LIBS" = x; then
   # try linking with several possible lua libs
     for lua_name in lua lua52 lua51 lua53 lua5.2 lua5.1 lua5.3
     do
       if test x"$LUA_LIBS" = x; then
         AC_CHECK_LIB($lua_name, lua_close, [
           AC_CHECK_HEADERS([lua.h],[
             LUA_LIBS="-l$lua_name"
             AC_DEFINE([HAVE_LUA], [1], [liblua])
             AC_DEFINE([HAVE_LUA_H], [1], [lua.h])
             LUA_NAME=$lua_name
           ])
         ])
       fi
     done
   fi
 fi

 if test x"$LUA_LIBS" = x; then
   # show error
   AC_MSG_ERROR(LUA (version 5.1 or newer) was not found)
 fi

 AC_SUBST(LUA_CFLAGS)
 AC_SUBST(LUA_LIBS)
else
 AM_CONDITIONAL(LUA, false)
fi


dnl Check for libpng - borrowed from SDL_image
AC_ARG_WITH(png, AS_HELP_STRING([--with-png],[use PNG format for screenshots (recommended)]), [enable_png=$withval], [enable_png=yes])
if test "x$enable_png" = xyes; then
    PKG_CHECK_MODULES([LIBPNG], [libpng], [dnl
        have_png_hdr=yes
        CFLAGS="$CFLAGS $LIBPNG_CFLAGS"
	LIBS="$LIBS $LIBPNG_LIBS"
        have_png_lib=yes
        AC_DEFINE([HAVE_PNG], [1], [libpng])
      ], [dnl
        AC_CHECK_HEADER([png.h], [have_png_hdr=yes])
        AC_CHECK_LIB([png], [png_destroy_write_struct], [have_png_lib=yes], [], [-lz])
   	if test x"$have_png_lib" = xyes; then
	   LIBS="$LIBS -lpng -lz"
           AC_DEFINE([HAVE_PNG], [1], [libpng])
	fi
      ])

   if test x"$have_png_hdr" = x; then
      AC_MSG_WARN([*** Unable to find PNG library (http://www.libpng.org/pub/png/libpng.html)])
      AC_MSG_WARN([PNG screenshot support disabled])
   fi
fi

dnl Check for agar
dnl agar 1.4.1 is required for the new GUI
dnl http://www.libagar.org
AC_MSG_CHECKING(if agar support is requested)
AC_ARG_WITH(agar, AS_HELP_STRING([--with-agar],[AGAR support, required for new GUI (recommended)]), [WITH_AGAR=$withval], [WITH_AGAR=yes])

AC_MSG_RESULT($WITH_AGAR)
if test x"$WITH_AGAR" != x"no"; then
 AM_CONDITIONAL(AGAR, true)
else
 AM_CONDITIONAL(AGAR, false)
 AC_DEFINE([WITHOUT_AGAR], [1], [libagar])
fi

if test x"$WITH_AGAR" != x"no"
then
 #try agar-config
 AC_PATH_PROG(AGARCONFIG, agar-config)
 if test x"$AGARCONFIG" != x
 then
   AGAR_VERSION=`$AGARCONFIG --version`
   AGAR_CFLAGS=`$AGARCONFIG --cflags`
   AGAR_LIBS=`$AGARCONFIG --libs`
   AGAR_LIBDIR=`$AGARCONFIG --prefix`/lib

   AC_DEFINE([HAVE_AGAR], [1], [libagar])
   CFLAGS="$CFLAGS $AGAR_CFLAGS"
   CPPFLAGS="$CPPFLAGS $AGAR_CFLAGS"
   LIBS="$LIBS $AGAR_LIBS"
   have_agar_lib=yes
 else
   #check for libag_gui presence
   if test x"$host" = x"i686-pc-mingw32" ; then
     AC_CHECK_LIB([ag_gui], [AG_DestroyGUI], [have_agar_lib=yes; LIBS="-lag_gui -lag_core $LIBS -lfreetype -lwinmm"], [], [-lag_core -lfreetype -lwinmm])
   else
     AC_CHECK_LIB([ag_gui], [AG_DestroyGUI], [have_agar_lib=yes; LIBS="-lag_gui -lag_core $LIBS -lfreetype"], [], [-lag_core -lfreetype])
   fi
 fi
 
 #check if agar header files can be found 
 AC_CHECK_HEADER([agar/core.h], [have_agar_hdr=yes])
 if test x"$have_agar_hdr" = x -o x"$have_agar_lib" = x
 then
   AM_CONDITIONAL(AGAR, false)
   AC_DEFINE([WITHOUT_AGAR], [1], [libagar])
   AC_MSG_WARN([*** Unable to find AGAR library (http://www.libagar.org)])
   AC_MSG_WARN([new GUI disabled])
 else
  echo found AGAR library "$AGAR_VERSION"
  AC_SUBST(AGAR_CFLAGS)
  AC_SUBST(AGAR_LIBS)
  AC_SUBST(AGAR_LIBDIR)
 fi
fi

#check if we need to link with -ldl
AC_CHECK_LIB([dl], [dlerror])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

