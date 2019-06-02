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

#ifndef _llconv_h_
#define _llconv_h_

#include "image.h"

typedef struct {
  unsigned int width, height;
  int *data;
  int origin_x, origin_y;
  int divisor;
} ikernel;

typedef struct {
  unsigned int width, height;
  float *data;
  int origin_x, origin_y;
} fkernel;

typedef struct {
  int nkerns;
  fkernel *kerns;
  int nops;
  struct cxop {
    enum { CXconvolve, CXand, CXor, CXscale} id;
    union {
      int iarg;
      float farg;
    } cx;
  } *ops;
} cxkernel;

typedef struct cxop cxop;

typedef void (*cxcallbackf) PARAMS((int l, int w, void *cbp,
				    float const *o1,
				    float const *o2));

image_float *
cxconvolve_gray8 PARAMS((image_gray8 const *in, float degree,
			 cxkernel const *kern,
			 image_float *out1, image_float *out2,
			 cxcallbackf cbf, void *cbp));

image_float *
cxconvolve_gray16 PARAMS((image_gray16 const *in, float degree,
			  cxkernel const *kern,
			  image_float *out1, image_float *out2,
			  cxcallbackf cbf, void *cbp));

image_float *
cxconvolve_gray32 PARAMS((image_gray32 const *in, float degree,
			  cxkernel const *kern,
			  image_float *out1, image_float *out2,
			  cxcallbackf cbf, void *cbp));

image_float *
cxconvolve_float PARAMS((image_float const *in, float degree,
			 cxkernel const *kern,
			 image_float *out1, image_float *out2,
			 cxcallbackf cbf, void *cbp));

cxkernel *
make_line1_cxkern PARAMS((float dir,
			  float sx, int n, float degree, float stab,
			  float sy, float sep));

cxkernel *
make_line2_cxkern PARAMS((float dir,
			  float sx, int n, float degree, float stab,
			  float sy, float sep));

cxkernel *
make_edge1_cxkern PARAMS((float dir,
			  float sx, int n, float degree, float stab,
			  float sy, float sep));

cxkernel *
make_edge2_cxkern PARAMS((float dir,
			  float sx, int n, float degree, float stab,
			  float sy, float sep));

void
free_fkernel PARAMS((fkernel *kern));

void
free_ikernel PARAMS((ikernel *kern));

void
free_cxkernel PARAMS((cxkernel *kern));

int
write_kern PARAMS((FILE *f, cxkernel *cxkern));

cxkernel *
read_kern PARAMS((FILE *f, char const *kern_file));

#endif /* _llconv_h_ */
