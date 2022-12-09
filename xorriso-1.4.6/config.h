/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define this if eaccess function is available */
/* #undef HAVE_EACCESS */

/* Define to 1 if fseeko (and presumably ftello) exists and is declared. */
#define HAVE_FSEEKO 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `acl' library (-lacl). */
/* #undef HAVE_LIBACL */

/* Define to 1 if you have the `bz2' library (-lbz2). */
/* #undef HAVE_LIBBZ2 */

/* Define to 1 if you have the `cdio' library (-lcdio). */
/* #undef HAVE_LIBCDIO */

/* Define to 1 if you have the `iconv' library (-liconv). */
/* #undef HAVE_LIBICONV */

/* Define to 1 if you have the `readline' library (-lreadline). */
/* #undef HAVE_LIBREADLINE */

/* Define to 1 if you have the `z' library (-lz). */
#define HAVE_LIBZ 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define this if timegm function is available */
#define HAVE_TIMEGM 1

/* Define this if tm structure includes a tm_gmtoff entry. */
#define HAVE_TM_GMTOFF 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Whether to apply const qualifier to iconv inbuf */
#define ICONV_CONST /**/

/* Define to use libbz2 by built-in libjte */
/* #undef LIBJTE_WITH_LIBBZ2 */

/* Allow libjte to use zlib */
#define LIBJTE_WITH_ZLIB /**/

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Define to use statvfs() with libburn stdio */
#define Libburn_os_has_statvfS /**/

/* Define to use O_DIRECT with -as cdrskin */
/* #undef Libburn_read_o_direcT */

/* Define to use libcdio as system adapter */
/* #undef Libburn_use_libcdiO */

/* Either timezone or 0 */
#define Libburnia_timezonE timezone

/* Define to use ACL capabilities */
/* #undef Libisofs_with_aaip_acL */

/* Define to use Linux xattr capabilities */
/* #undef Libisofs_with_aaip_xattR */

/* Define to use FreeBSD extattr capabilities */
/* #undef Libisofs_with_freebsd_extattR */

/* Define to use Jigdo Template Extraction via libjte */
#define Libisofs_with_libjtE /**/

/* Define to use compression via zlib */
#define Libisofs_with_zliB /**/

/* Name of package */
#define PACKAGE "xorriso"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://libburnia-project.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "xorriso"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "xorriso 1.4.6"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "xorriso"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.4.6"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to use multi-threading in built-in libjte */
#define THREADED_CHECKSUMS /**/

/* Version number of package */
#define VERSION "1.4.6"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define to allow xorriso to start external filter processes */
#define Xorriso_allow_external_filterS /**/

/* Define to allow xorriso command -launch_frontend when running under setuid
   */
/* #undef Xorriso_allow_extf_suiD */

/* Define to allow xorriso command -launch_frontend */
#define Xorriso_allow_launch_frontenD /**/

/* Define to make 64 KB default size for DVD writing */
/* #undef Xorriso_dvd_obs_default_64K */

/* Define to prepare sources for statically linked xorriso */
#define Xorriso_standalonE /**/

/* Define to use libedit if not libreadline */
/* #undef Xorriso_with_editlinE */

/* Define to use Jigdo Template Extraction via libjte */
#define Xorriso_with_libjtE /**/

/* Define to use libreadline */
/* #undef Xorriso_with_readlinE */

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define to 1 to make fseeko visible on some hosts (e.g. glibc 2.2). */
/* #undef _LARGEFILE_SOURCE */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif
