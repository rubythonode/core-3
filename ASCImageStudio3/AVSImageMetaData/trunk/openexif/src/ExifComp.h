/* src/ExifComp.h.  Generated from ExifComp.h.in by configure.  */
/* 
 * The contents of this file are subject to the IBM Common Public
 * License Version 0.5 (the "License"); you may not use this file 
 * except in compliance with the License. You may obtain a copy of 
 * the License at http://oss.software.ibm.com/developerworks/
 * opensource/license-cpl.html
 * 
 * The Original Code is the Open Exif Toolkit,
 * released November 13, 2000.
 *
 * The Initial Developer of the Original Code is Eastman 
 * Kodak Company. Portions created by Kodak are 
 * Copyright (C) 2001 Eastman Kodak Company. All 
 * Rights Reserved. 
 *
 * Creation Date: 07/14/2001
 *
 * Original Author: 
 * George Sotak george.sotak@kodak.com 
 *
 * Contributor(s): 
 * FirstName LastName <email address>
 */ 


#ifndef EXIF_COMPAT_
#define	EXIF_COMPAT_

/* config.h.in.  Generated from configure.in by autoheader.  */
/* Target CPU bit order - LSB2MSB if this is defined, MSB2LSB otherwise */
/* #undef HAVE_FILLORDER_LSB2MSB */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define as `__inline' if that's what the C compiler calls it, or to nothing
   if it is not supported. */
/* #undef inline */

/* Define to `long' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to 1 if you want DCF compliant files. */
#define OPENEXIF_DCF_COMPLIANT 1

/* Define to 1 if you DON'T want IJG or jpeg lib. */
/* This will remove the ability to read/write image data. */
/* #undef OPENEXIF_NO_IJG */

#endif /* _COMPAT_ */