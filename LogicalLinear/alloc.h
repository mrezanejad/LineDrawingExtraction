/* 

Copyright (c) 1995  McGill University and Lee Iverson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including the rights to use, copy,
modify, merge, publish, and distribute copies of the Software without fee, and
to permit persons to whom the Software is furnished to do so, subject to the
following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

Any sale of this software or any work derived from this software shall be
prohibited without written agreement from the copyright holders.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
MCGILL UNIVERSITY BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef _alloc_h_
#define _alloc_h_

#ifdef HAVE_CONFIG_H
#include "llconfig.h"
#endif

#include <ansidecl.h>

#ifdef STDC_HEADERS
#include <stdlib.h>
#endif

#ifndef HAVE_MEMDUP
#define memdup(p, len) memcpy (malloc (len), (p), (len))
#endif

/* These are slightly higher level malloc functions. */
#define new(t)		    ((t *) malloc (sizeof (t)))
#define newc(t)		    ((t *) calloc (1, sizeof (t)))
#define newv(t, n)	    ((t *) malloc ((n)*sizeof (t)))
#define newvc(t, n)	    ((t *) calloc ((n), sizeof (t)))
#define renewv(t, p, n)	    ((t *) realloc (p, (n)*sizeof (t)))
#define clone(t, p)	    ((t *) memdup (p, sizeof (t)))
#define clonev(t, p, n)	    ((t *) memdup (p, (n)*sizeof (t)))

#define anew(t)		    ((t *) alloca (sizeof(t)))
#define anewc(t)	    ((t *) memset (alloca (sizeof(t)), 0, sizeof(t)))
#define anewv(t, n)	    ((t *) alloca ((n)*sizeof(t)))
#define anewvc(t, n)	    ((t *) memset (alloca ((n)*sizeof(t)), 0, (n)*sizeof(t)))
#define aclone(t, p)	    ((t *) memcpy (alloca (sizeof(t)), p, sizeof(t)))
#define aclonev(t, p, n)    ((t *) memcpy (alloca ((n)*sizeof(t)), p, (n)*sizeof(t)))

#endif /* _alloc_h_ */
