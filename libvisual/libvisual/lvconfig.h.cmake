#ifndef __LV_CONFIG_H__
#define __LV_CONFIG_H__

#define VISUAL_VERSION     "@LV_VERSION@"
#define VISUAL_PLUGIN_PATH "@LV_PLUGINS_BASE_DIR@"

#ifndef __cplusplus
#cmakedefine LV_HAVE_ISO_C_VARARGS 1
#endif

/* gcc-2.95.x supports both gnu style and ISO varargs, but if -ansi
 * is passed ISO vararg support is turned off, and there is no work
 * around to turn it on, so we unconditionally turn it off.
 */
#if __GNUC__ == 2 && __GNUC_MINOR__ == 95
#undef LV_HAVE_ISO_C_VARARGS
#endif

#cmakedefine LV_HAVE_GNU_C_VARARGS 1

#define VISUAL_BIG_ENDIAN    @VISUAL_BIG_ENDIAN@
#define VISUAL_LITTLE_ENDIAN @VISUAL_LITTLE_ENDIAN@

typedef unsigned @VISUAL_SIZE_TYPE@ visual_size_t;

#cmakedefine VISUAL_SIZE_T_FORMAT "@VISUAL_SIZE_T_FORMAT@"

#cmakedefine VISUAL_RANDOM_FAST_FP_RNG

#cmakedefine VISUAL_ARCH_MIPS
#cmakedefine VISUAL_ARCH_ALPHA
#cmakedefine VISUAL_ARCH_SPARC
#cmakedefine VISUAL_ARCH_X86
#cmakedefine VISUAL_ARCH_X86_64
#cmakedefine VISUAL_ARCH_POWERPC
#cmakedefine VISUAL_ARCH_UNKNOWN

#cmakedefine VISUAL_OS_LINUX
#cmakedefine VISUAL_OS_WIN32
#cmakedefine VISUAL_OS_HPUX
#cmakedefine VISUAL_OS_AIX
#cmakedefine VISUAL_OS_BEOS
#cmakedefine VISUAL_OS_OSF
#cmakedefine VISUAL_OS_IRIX
#cmakedefine VISUAL_OS_SUNOS
#cmakedefine VISUAL_OS_SOLARIS
#cmakedefine VISUAL_OS_SCO
#cmakedefine VISUAL_OS_FREEBSD
#cmakedefine VISUAL_OS_NETBSD
#cmakedefine VISUAL_OS_OPENBSD
#cmakedefine VISUAL_OS_DARWIN
#cmakedefine VISUAL_OS_UNKNOWN

#cmakedefine VISUAL_WITH_CYGWIN
#cmakedefine VISUAL_WITH_MINGW

#cmakedefine VISUAL_HAVE_THREADS
#cmakedefine VISUAL_THREAD_MODEL_WIN32
#cmakedefine VISUAL_THREAD_MODEL_POSIX
#cmakedefine VISUAL_THREAD_MODEL_DCE
#cmakedefine VISUAL_THREAD_MODEL_GTHREAD2

#endif /* LV_CONFIG_H */
