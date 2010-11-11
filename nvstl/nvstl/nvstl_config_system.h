/*
 * Copyright (c) 1997
 * Moscow Center for SPARC Technology
 *
 * Copyright (c) 1999
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

/*
 * Purpose of this file :
 *
 * To hold COMPILER-SPECIFIC portion of STLport settings.
 * In general, user should not edit this file unless
 * using the compiler not recognized below.
 *
 * If your compiler is not being recognized yet,
 * please look for definitions of macros in stl_mycomp.h,
 * copy stl_mycomp.h to stl_YOUR_COMPILER_NAME,
 * adjust flags for your compiler, and add  <include config/stl_YOUR_COMPILER_NAME>
 * to the secton controlled by unique macro defined internaly by your compiler.
 *
 * To change user-definable settings, please edit <user_config.h>
 *
 */

#ifndef __stl_config__system_h
#define __stl_config__system_h

#if defined (__sun)
#  include "nvstl_stl_config_solaris.h"
#  if defined (__GNUC__)
#    include "nvstl_stl_confic_gcc.h"
#  elif defined (__SUNPRO_CC) || defined (__SUNPRO_C)
#    include "nvstl_stl_config_sunprocc.h"
/*
#  ifdef __KCC
#    include "nvstl_stl_config_kai.h"
#  endif
*/
#  elif defined (__APOGEE__)  /* Apogee 4.x */
#    include "nvstl_stl_config_apcc.h"
#  elif defined (__FCC_VERSION) /* Fujitsu Compiler, v4.0 assumed */
#    include "nvstl_stl_config_fujitsu.h"
#  endif
#elif defined (__hpux)
#  include "nvstl_stl_config_hpux.h"
#  if defined (__GNUC__)
#    include "nvstl_stl_confic_gcc.h"
#  elif defined (__HP_aCC)
#    include "nvstl_stl_config_hpacc.h"
#  endif
#elif defined (linux) || defined (__linux__)
#  include "nvstl_stl_config_linux.h"
#  if defined (__BORLANDC__)
#    include "nvstl_stl_config_bc.h" /* Borland C++ 0x570 */
/* Intel's icc define __GNUC__! */
#  elif defined (__INTEL_COMPILER)
#    include "nvstl_stl_config_icc.h"
#  elif defined (__GNUC__)
#    include "nvstl_stl_confic_gcc.h"
#  endif
/*
#  ifdef __KCC
#    include "nvstl_stl_config_kai.h"
#  endif
*/
#elif defined (__FreeBSD__)
#  include "nvstl_stl_config_freebsd.h"
#  if defined (__GNUC__)
#    include "nvstl_stl_confic_gcc.h"
#  endif
#elif defined (__OpenBSD__)
#  include "nvstl_stl_config_openbsd.h"
#  if defined (__GNUC__)
#    include "nvstl_stl_confic_gcc.h"
#  endif
#elif defined (__sgi) /* IRIX? */
#  define _STLP_PLATFORM "SGI Irix"
#  if defined (__GNUC__)
#    include "nvstl_stl_confic_gcc.h"
#  else
#    include "nvstl_stl_config_sgi.h"
#  endif
#elif defined (__OS400__) /* AS/400 C++ */
#  define _STLP_PLATFORM "OS 400"
#  if defined (__GNUC__)
#    include "nvstl_stl_confic_gcc.h"
#  else
#    include "nvstl_stl_config_as400.h"
#  endif
#elif defined (_AIX)
#  include "nvstl_stl_config_aix.h"
#  if defined (__xlC__) || defined (__IBMC__) || defined ( __IBMCPP__ )
     /* AIX xlC, Visual Age C++ , OS-390 C++ */
#    include "nvstl_stl_config_ibm.h"
#  endif
#elif defined (_CRAY) /* Cray C++ 3.4 or 3.5 */
#  define _STLP_PLATFORM "Cray"
#  include <config/_cray.h>
#elif defined (__DECCXX) || defined (__DECC)
#  define _STLP_PLATFORM "DECC"
#  ifdef __vms
#    include "nvstl_stl_config_dec_vms.h"
#  else
#    include "nvstl_stl_config_dec.h"
#  endif
#elif defined (macintosh) || defined (_MAC)
#  include "nvstl_stl_config_mac.h"
#  if defined (__MWERKS__)
#    include "nvstl_stl_config_mwerks.h"
#  endif
#elif defined (__APPLE__)
#  include "nvstl_stl_config_macosx.h"
#  ifdef __GNUC__
#    include "nvstl_stl_confic_gcc.h"
#  endif
#elif defined (__CYGWIN__)
#  include "nvstl_stl_config_cygwin.h"
#  if defined (__GNUC__)
#    include "nvstl_stl_confic_gcc.h"
#  endif
#elif defined (__MINGW32__)
#  define _STLP_PLATFORM "MinGW"
#  if defined (__GNUC__)
#    include "nvstl_stl_confic_gcc.h"
#  endif
#  include "nvstl_stl_config_windows.h"
#elif defined (_WIN32) || defined (__WIN32) || defined (WIN32) || defined (__WIN32__) || \
      defined (__WIN16) || defined (WIN16) || defined (_WIN16)
#  if defined ( __BORLANDC__ )  /* Borland C++ / CodeGear C++ */
#    include "nvstl_stl_config_bc.h"
#  elif defined (__WATCOM_CPLUSPLUS__) || defined (__WATCOMC__)  /* Watcom C++ */
#    include "nvstl_stl_config_watcom.h"
#  elif defined (__COMO__) || defined (__COMO_VERSION_)
#    include "nvstl_stl_config_como.h"
#  elif defined (__DMC__)   /* Digital Mars C++ */
#    include "nvstl_stl_config_dm.h"
#  elif defined (__ICL) /* Intel reference compiler for Win */
#    include "nvstl_stl_config_intel.h"
#  elif defined (__MWERKS__)
#    include "nvstl_stl_config_mwerks.h"
#  elif defined (_MSC_VER) && (_MSC_VER >= 1200) && defined (UNDER_CE)
     /* Microsoft eMbedded Visual C++ 3.0, 4.0 (.NET) */
#    include "nvstl_stl_config_evc.h"
#  elif defined (_MSC_VER)
    /* Microsoft Visual C++ 6.0, 7.0, 7.1, 8.0 */
#    include "nvstl_config_msvc.h"
#  endif

#  include "nvstl_config_windows.h"
#else
#  error Unknown platform !!
#endif

#if !defined (_STLP_COMPILER)
/* Unable to identify the compiler, issue error diagnostic.
 * Edit <config/stl_mycomp.h> to set STLport up for your compiler. */
#  include "nvstl_stl_config_stl_mycomp.h"
#endif

#endif /* __stl_config__system_h */
