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

#ifndef _llimage_h_
#define _llimage_h_

#include <sys/types.h>

#include <ansidecl.h>
#include <stdio.h>

#if !defined(_PBM_H_) && !defined(_imdefs_h_)
typedef unsigned char bit;
#endif

typedef unsigned char byte;

typedef unsigned char gray8;
typedef short gray16;
typedef int gray32;

#ifndef BIT
# define BIT(n) (1 << (n))
#endif

#ifndef MAX
# define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

typedef struct {
  unsigned int width, height;
} image_header;

#define image_width(im)  (((image_header *) (im))->width)
#define image_height(im) (((image_header *) (im))->height)

typedef struct {
  unsigned int width, height;
  bit *data;
} image_bit;

extern image_bit *new_image_bit PARAMS((int width, int height));
extern void free_image_bit PARAMS((image_bit *im));

typedef struct {
  unsigned int width, height;
  gray8 *data;
  gray8 max;
} image_gray8;

extern image_gray8 *new_image_gray8 PARAMS((int width, int height));
extern void free_image_gray8 PARAMS((image_gray8 *im));

typedef struct {
  unsigned int width, height;
  gray16 *data;
  gray16 min, max;
} image_gray16;

extern image_gray16 *new_image_gray16 PARAMS((int width, int height));
extern void free_image_gray16 PARAMS((image_gray16 *im));

typedef struct {
  unsigned int width, height;
  gray32 *data;
  gray32 min, max;
} image_gray32;

extern image_gray32 *new_image_gray32 PARAMS((int width, int height));
extern void free_image_gray32 PARAMS((image_gray32 *im));

typedef struct {
  unsigned int width, height;
  float *data;
  float min, max;
} image_float;

extern image_float *new_image_float PARAMS((int width, int height));
extern void free_image_float PARAMS((image_float *im));

typedef struct {
  unsigned int width, height;
  double *data;
  double min, max;
} image_double;

extern image_double *new_image_double PARAMS((int width, int height));
extern void free_image_double PARAMS((image_double *im));

extern image_gray8 *
image_downscale_gray8 PARAMS((image_gray8 const *, int));

extern image_gray8 *
read_pgm PARAMS((FILE *));

extern void 
write_pgm PARAMS((FILE *, image_gray8 *));

extern long
timer_elapsed PARAMS((void *timer, char *s));

extern long
timer_elapsed_n PARAMS((void *timer, char *s, int n));

/*
 * This is a simple trick for a trivial kind of template expansion in the
 * C code.  If a file is compiled with any of the #defines below specified on
 * the command line (e.g. -DCOMPILE_BIT), then type gets appropriately set.
 */

#ifndef CONCAT
# ifdef __STDC__
#  define _CONCAT(a,b) a ## b
#  define CONCAT(a,b) _CONCAT(a,b)
# else /* !__STDC__ */
#  define _I(x) x
#  define _CONCAT(a,b) _I(a)b
#  define CONCAT(a,b) _CONCAT(a,b)
# endif /* !__STDC__ */
#endif /* !CONCAT */

#ifdef COMPILE_BIT
# define type bit
# define image_type image_bit
# define CONCAT_type(x) CONCAT(x,_bit)
#endif

#ifdef COMPILE_GRAY8
# define type gray8
# define image_type image_gray8
# define arith_type gray32
# define CONCAT_type(x) CONCAT(x,_gray8)
#endif

#ifdef COMPILE_GRAY16
# define type gray16
# define image_type image_gray16
# define arith_type gray32
# define CONCAT_type(x) CONCAT(x,_gray16)
#endif

#ifdef COMPILE_GRAY32
# define type gray32
# define image_type image_gray32
# define arith_type gray32
# define CONCAT_type(x) CONCAT(x,_gray32)
#endif

#ifdef COMPILE_FLOAT
# define type float
# define image_type image_float
# define arith_type float
# define CONCAT_type(x) CONCAT(x,_float)
#endif

#ifdef COMPILE_DOUBLE
# define type double
# define image_type image_double
# define arith_type double
# define CONCAT_type(x) CONCAT(x,_double)
#endif

#ifndef image_type
# define image_type CONCAT(image_,type)
#endif

#ifndef CONCAT_type
# define CONCAT_type(x) CONCAT(x,CONCAT(_,type))
#endif

#endif /* _llimage_h_ */
