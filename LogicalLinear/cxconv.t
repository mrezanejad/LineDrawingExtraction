/* -*- C -*-

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

#include "image.h"
#include "conv.h"
#include "loglin.h"
#include "alloc.h"

#include <assert.h>
#include <stdio.h>

/*
 * This #define enables an optimization that should allow significantly
 * improved inner loop compilation on RISC machines.
 */
#define LOOP_AND_A_HALF 1

#define DBG 0

#define cxconvolve CONCAT_type(cxconvolve)
#define fconvolve CONCAT_type(fconvolve)

static void
fconv_line PARAMS((type const *in, unsigned int w, unsigned int h, int line, 
		   fkernel const *kern, float *out));

image_float *
DEFUN (cxconvolve, (in, degree, kern, out1, out2, cbf, cbp),
       image_type const *in AND
       float degree AND
       cxkernel const *kern AND
       image_float *out1 AND
       image_float *out2 AND
       cxcallbackf cbf AND
       void *cbp)
{
  type const *ip = in->data, *ipend;

  unsigned int w = in->width, h = in->height, len = w*h;
  int line = 0;

  int nkerns = kern->nkerns;
  fkernel *kerns = kern->kerns;
#if 0
  int *last_ref = anewvc (int, nkerns);
#endif

  int nops = kern->nops;
  cxop *op = kern->ops;

  float **s1 = anewvc (float *, nops), **s1top = s1;
  float **s2 = (out2? anewvc (float *, nops) : 0), **s2top = s2;
  float *op1, *op2;

  /* Scale depending on max contrast of image. */
#if defined(COMPILE_FLOAT) || defined(COMPILE_DOUBLE)
  degree /= (in->max - in->min);
#else
  degree /= in->max;
#endif

#if 0
  float **convs = anewvc (float *, nkerns);

  {
    int i;
    /* For each kernel determine what the nops value will be when it is last
       used.  */
    for (i = nops, op = kern->ops; i > 0; --i, ++op)
      if (op->id == CXconvolve)
	last_ref[op->cx.iarg] = i;
  }
#endif
  
  /* Copy the destination image(s) onto the top of the stack. */
  s1top[0] = op1 = out1->data;
  if (s2top) s2top[0] = op2 = out2->data;

  for (ip = in->data, ipend = ip+len; ip < ipend; ip += w, ++line) {

    if (DBG) printf ("Line %d:\n", line);

#if 0  
    /* Clear remembered convolution pointer. */
    memset (convs, 0, sizeof (float *) * nkerns);
#endif

    /* Now, walk through the ops which make up the complex kernel. */
    for (op = kern->ops, nops = kern->nops; nops; ++op, --nops) {

      /* Check for stack underflow. */
      assert (s1top >= s1);

      if (DBG) {
	int i;
	for (i = 0; i < s1top-s1; ++i)
	  printf ((s2 ? " %8x %8x\n" : " %8x"), s1[i], s2 ? s2[i] : 0);
	if (! s2) printf ("\n");
      }

      /* Select the current operation. */
      switch (op->id) {

      case CXconvolve: {
	int kern_idx = op->cx.iarg;

	if (DBG)
	  printf ("%8x: Convolve %d %08x [%dx%d] (%d,%d)\n", op,
		  kern_idx, (unsigned int) &kerns[kern_idx],
		  kerns[kern_idx].width, kerns[kern_idx].height,
		  kerns[kern_idx].origin_x, kerns[kern_idx].origin_y);
 
	/* Convolve with the given kernel. */
	if (! s1top[0]) s1top[0] = anewv (float, w);
	if (s2top && ! s2top[0]) s2top[0] = s1top[0];

#if 0
	if (convs[kern_idx]) {
	  if (last_ref[kern_idx] == nops)
	    s1top[0] = convs[kern_idx];
	} else {
	  if (last_ref[kern_idx] == nops) {
	    convs[kern_idx] = s1top[0];
	  } else {
	    convs[kern_idx] = anewv (float, w);
	  }

	  fconv_line (ip, w, h, line, &kerns[kern_idx], convs[kern_idx]);
	}
	if (last_ref[kern_idx] != nops)
	  memcpy (s1top[0], convs[kern_idx], w * sizeof (float));
#else
	fconv_line (ip, w, h, line, &kerns[kern_idx], s1top[0]);
#endif

	if (s2top && s2top[0] != s1top[0])
	  memcpy (s2top[0], s1top[0], w * sizeof (float));

	++s1top; if (s2top) ++s2top;
	break;
      }

      case CXand: {
	/* Combine <n> lines by logical/linear AND. */
	int n = op->cx.iarg;
      
	if (DBG) printf ("%8x: Logical/linear AND top %d\n", op, n);
 
	s1top -= n;
	if (s2top) {
	  s2top -= n;
	  if (s2top[0] == s1top[0]) {
	    s2top[0] = anewv (float, w);
	    memcpy (s2top[0], s1top[0], w * sizeof (float));
	  }
	}

	switch (n) {
	case 2: {
	  float *l0, *l1, *l0end;
	  l0 = s1top[0]; l1 = s1top[1];
	  for (l0end = l0+w; l0 < l0end; ++l0, ++l1)
	    l0[0] = contin_lland2 (degree, l0[0], l1[0]);
	  if (s2top) {
	    l0 = s2top[0]; l1 = s2top[1];
	    for (l0end = l0+w; l0 < l0end; ++l0, ++l1)
	      l0[0] = contin_llor2 (degree, l0[0], l1[0]);
	  }
	  break;
	}

	case 3: {
	  float *l0, *l1, *l2, *l0end;
	  l0 = s1top[0]; l1 = s1top[1]; l2 = s1top[2];
	  for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2)
	    l0[0] = contin_lland3 (degree, l0[0], l1[0], l2[0]);
	  if (s2top) {
	    l0 = s2top[0]; l1 = s2top[1]; l2 = s2top[2];
	    for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2)
	      l0[0] = contin_llor3 (degree, l0[0], l1[0], l2[0]);
	  }
	  break;
	}

	case 4: {
	  float *l0, *l1, *l2, *l3, *l0end;
	  l0 = s1top[0]; l1 = s1top[1]; l2 = s1top[2]; l3 = s1top[3];
	  for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2, ++l3)
	    l0[0] = contin_lland4 (degree, l0[0], l1[0], l2[0], l3[0]);
	  if (s2top) {
	    l0 = s2top[0]; l1 = s2top[1]; l2 = s2top[2]; l3 = s2top[3];
	    for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2, ++l3)
	      l0[0] = contin_llor4 (degree, l0[0], l1[0], l2[0], l3[0]);
	  }
	  break;
	}

	case 5: {
	  float *l0, *l1, *l2, *l3, *l4, *l0end;
	  l0 = s1top[0]; l1 = s1top[1]; l2 = s1top[2];
	  l3 = s1top[3]; l4 = s1top[4];
	  for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2, ++l3, ++l4)
	    l0[0] = contin_lland5 (degree, l0[0], l1[0], l2[0], l3[0], l4[0]);
	  if (s2top) {
	    l0 = s2top[0]; l1 = s2top[1]; l2 = s2top[2];
	    l3 = s2top[3]; l4 = s2top[4];
	    for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2, ++l3, ++l4)
	      l0[0] = contin_llor5 (degree, l0[0], l1[0], l2[0], l3[0], l4[0]);
	  }
	  break;
	}

	case 6: {
	  float *l0, *l1, *l2, *l3, *l4, *l5, *l0end;
	  l0 = s1top[0]; l1 = s1top[1]; l2 = s1top[2];
	  l3 = s1top[3]; l4 = s1top[4]; l5 = s1top[5];
	  for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2, ++l3, ++l4, ++l5)
	    l0[0] = contin_lland6 (degree, l0[0], l1[0], l2[0], l3[0], l4[0], l5[0]);
	  if (s2top) {
	    l0 = s2top[0]; l1 = s2top[1]; l2 = s2top[2];
	    l3 = s2top[3]; l4 = s2top[4]; l5 = s2top[5];
	    for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2, ++l3, ++l4, ++l5)
	      l0[0] = contin_llor6 (degree, l0[0], l1[0], l2[0], l3[0], l4[0], l5[0]);
	  }
	  break;
	}

	default: {
	  int i;
	  float **ptrs = anewv (float *, n);
	  float *l0, *l0end;

	  for (i = 0; i < n; ++i) ptrs[i] = s1top[i];
	  l0 = s1top[0];
	  for (l0end = l0+w; l0 < l0end; ++l0) {
	    l0[0] = contin_lland_np (degree, (float const **) ptrs, n);
	    for (i = 0; i < n; ++i) ++ptrs[i];
	  }
	  if (s2top) {
	    for (i = 0; i < n; ++i) ptrs[i] = s2top[i];
	    l0 = s2top[0];
	    for (l0end = l0+w; l0 < l0end; ++l0) {
	      l0[0] = contin_llor_np (degree, (float const **) ptrs, n);
	      for (i = 0; i < n; ++i) ++ptrs[i];
	    }
	  }
	  break;
	}}

	++s1top; if (s2top) ++s2top;
	break;
      }

      case CXor: {
	/* Combine <n> images by logical/linear OR. */
	int n = op->cx.iarg;
      
	if (DBG) printf ("%8x: Logical/linear OR top %d\n", op, n);
 
	s1top -= n;
	if (s2top) {
	  s2top -= n;
	  if (s2top[0] == s1top[0]) {
	    s2top[0] = anewv (float, w);
	    memcpy (s2top[0], s1top[0], w * sizeof (float));
	  }
	}

	/* Select on the number of operands. */
	switch (n) {
	case 2: {
	  float *l0, *l1, *l0end;
	  l0 = s1top[0]; l1 = s1top[1];
	  for (l0end = l0+w; l0 < l0end; ++l0, ++l1)
	    l0[0] = contin_llor2 (degree, l0[0], l1[0]);
	  if (s2top) {
	    l0 = s2top[0]; l1 = s2top[1];
	    for (l0end = l0+w; l0 < l0end; ++l0, ++l1)
	      l0[0] = contin_lland2 (degree, l0[0], l1[0]);
	  }
	  break;
	}

	case 3: {
	  float *l0, *l1, *l2, *l0end;
	  l0 = s1top[0]; l1 = s1top[1]; l2 = s1top[2];
	  for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2)
	    l0[0] = contin_llor3 (degree, l0[0], l1[0], l2[0]);
	  if (s2top) {
	    l0 = s2top[0]; l1 = s2top[1]; l2 = s2top[2];
	    for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2)
	      l0[0] = contin_lland3 (degree, l0[0], l1[0], l2[0]);
	  }
	  break;
	}

	case 4: {
	  float *l0, *l1, *l2, *l3, *l0end;
	  l0 = s1top[0]; l1 = s1top[1]; l2 = s1top[2]; l3 = s1top[3];
	  for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2, ++l3)
	    l0[0] = contin_llor4 (degree, l0[0], l1[0], l2[0], l3[0]);
	  if (s2top) {
	    l0 = s2top[0]; l1 = s2top[1]; l2 = s2top[2]; l3 = s2top[3];
	    for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2, ++l3)
	      l0[0] = contin_lland4 (degree, l0[0], l1[0], l2[0], l3[0]);
	  }
	  break;
	}

	case 5: {
	  float *l0, *l1, *l2, *l3, *l4, *l0end;
	  l0 = s1top[0]; l1 = s1top[1]; l2 = s1top[2];
	  l3 = s1top[3]; l4 = s1top[4];
	  for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2, ++l3, ++l4)
	    l0[0] = contin_llor5 (degree, l0[0], l1[0], l2[0], l3[0], l4[0]);
	  if (s2top) {
	    l0 = s2top[0]; l1 = s2top[1]; l2 = s2top[2];
	    l3 = s2top[3]; l4 = s2top[4];
	    for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2, ++l3, ++l4)
	      l0[0] = contin_lland5 (degree, l0[0], l1[0], l2[0], l3[0], l4[0]);
	  }
	  break;
	}

	case 6: {
	  float *l0, *l1, *l2, *l3, *l4, *l5, *l0end;
	  l0 = s1top[0]; l1 = s1top[1]; l2 = s1top[2];
	  l3 = s1top[3]; l4 = s1top[4]; l5 = s1top[5];
	  for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2, ++l3, ++l4, ++l5)
	    l0[0] = contin_llor6 (degree, l0[0], l1[0], l2[0], l3[0], l4[0], l5[0]);
	  if (s2top) {
	    l0 = s2top[0]; l1 = s2top[1]; l2 = s2top[2];
	    l3 = s2top[3]; l4 = s2top[4]; l5 = s2top[5];
	    for (l0end = l0+w; l0 < l0end; ++l0, ++l1, ++l2, ++l3, ++l4, ++l5)
	      l0[0] = contin_lland6 (degree, l0[0], l1[0], l2[0], l3[0], l4[0], l5[0]);
	  }
	  break;
	}

	default: {
	  int i;
	  float **ptrs = anewv (float *, n);
	  float *l0, *l0end;

	  for (i = 0; i < n; ++i) ptrs[i] = s1top[i];
	  l0 = s1top[0];
	  for (l0end = l0+w; l0 < l0end; ++l0) {
	    l0[0] = contin_llor_np (degree, (float const **) ptrs, n);
	    for (i = 0; i < n; ++i) ++ptrs[i];
	  }
	  if (s2top) {
	    for (i = 0; i < n; ++i) ptrs[i] = s2top[i];
	    l0 = s2top[0];
	    for (l0end = l0+w; l0 < l0end; ++l0) {
	      l0[0] = contin_lland_np (degree, (float const **) ptrs, n);
	      for (i = 0; i < n; ++i) ++ptrs[i];
	    }
	  }
	  break;
	}}

	++s1top; if (s2top) ++s2top;
	break;
      }

      case CXscale: {
	/* Scale the current image. */
	float *l0, *l0end;
	float scl = op->cx.farg;

	if (DBG) printf ("%8x: Scale top %f\n", op, scl);

	l0 = s1top[-1]; 
	for (l0end = l0+w; l0 < l0end; ++l0)
	  l0[0] *= scl;
	if (s2top) {
	  l0 = s2top[-1];
	  for (l0end = l0+w; l0 < l0end; ++l0)
	    l0[0] *= scl;
	}
	break;
      }
      }
    }

    if (DBG) {
      int i;
      printf ("Final stack:\n");
      for (i = 0; i < s1top-s1; ++i)
	printf ((s2 ? " %8x %8x\n" : " %8x"), s1[i], s2 ? s2[i] : 0);
      if (! s2) printf ("\n");
    }

    /* The stack must now be exactly one image long. */
    assert (s1top == &s1[1]);
    --s1top;

    /* If the top element isn't pointing at out1, then copy. */
    if (s1top[0] != op1) {
      memcpy (op1, s1top[0], w * sizeof (float));
      op1 += w;
    } else {
      s1top[0] = (op1 += w);
    }

    if (s2top) {
      --s2top;
      if (s2top[0] != op2) {
	/* The complement must be negated as it is copied. */
	float *ip = s2top[0], *ipend = ip+w, *op = op2;
	for ( ; ip < ipend; ++ip, ++op) op[0] = -ip[0];
	op2 += w;
      } else {
	/* The complement must be negated in place. */
	float *op = op2, *opend = op+w;
	for ( ; op < opend; ++op) op[0] = -op[0];
	s2top[0] = (op2 += w);
      }
    }

    /* Call function for the line(s) just completed. */
    if (cbf) (*cbf) (line, w, cbp, op1-w, op2-w);
  }

  return out1;
}

/*
 * Use this when the kernel may overlap the top of the window.
 */

static void
DEFUN (fconv_line, (in, w, h, line, kern, out),
       type const *in AND
       unsigned int w AND unsigned int h AND
       int line AND
       fkernel const *kern AND
       float *out)
{
  int kw = kern->width, kh = kern->height;
  int kx = kern->origin_x, ky = kern->origin_y;
  float const *kdata = kern->data;

  type const *kip = in - kx - ky*w;

  float *op = out, *opend;
  float const *kp = kdata;
  int klen;

  /* Adjust height of kernel near boundaries. */
  {
    int adj_top = ky-line;
    int adj_bot = kh-ky-(h-line);

    if (adj_top > 0) { kp += adj_top*kw; kip += adj_top*w; kh -= adj_top; }
    if (adj_bot > 0) { kh -= adj_bot; }
  }
  klen = kh*kw;

  /* This loop runs near the left boundary. */
  for (opend = out+kx; op < opend; ++op, ++kip) {
    float sum = 0;
    type const *kip1 = kip;
    float const *kp1 = kp, *kp1end;
    unsigned int kw1 = kw;

    /* Adjust width of kernel near boundaries. */
    {
      int adj_l = kx-(op-out);
      int adj_r = kw-kx-(out+w-op);

      if (adj_l > 0) { kp1 += adj_l; kip1 += adj_l; kw1 -= adj_l; }
      if (adj_r > 0) { kw1 -= adj_r; }
    }

    if (kw1 > 0) {
      for (kp1end = kp1+klen; kp1 < kp1end; kip1 += w, kp1 += kw) {
	type const *kip2 = kip1;
	float const *kp2 = kp1, *kp2end = kp2+kw1;

#if LOOP_AND_A_HALF
	float a, b, prod;
	a = (kp2++)[0];
	b = (kip2++)[0];
	if (kp2 < kp2end)
	  while (kp2 != kp2end) {
	    prod = a*b;
	    a = (kp2++)[0];
	    b = (kip2++)[0];
	    sum += prod;
	  }
	prod = a*b;
	sum += prod;
#else
	for ( ; kp2 < kp2end; ++kp2, ++kip2)
	  sum += kp2[0] * kip2[0];
#endif
      }
    }

    op[0] = sum;
  }

  /* This loop runs in the center (away from left or right boundaries). */
  {
    int adj_r = (kw-kx-1 >= 0 ? kw-kx-1 : 0);

    for (opend = out+w-adj_r; op < opend; ++op, ++kip) {
      float sum = 0;
      type const *kip1 = kip;
      float const *kp1 = kp, *kp1end;

      for (kp1end = kp1+klen; kp1 < kp1end; kip1 += w, kp1 += kw) {
	type const *kip2 = kip1;
	float const *kp2 = kp1, *kp2end = kp1+kw;

#if LOOP_AND_A_HALF
	float a, b, prod;
	a = (kp2++)[0];
	b = (kip2++)[0];
	if (kp2 < kp2end)
	  while (kp2 != kp2end) {
	    prod = a*b;
	    a = (kp2++)[0];
	    b = (kip2++)[0];
	    sum += prod;
	  }
	prod = a*b;
	sum += prod;
#else
	for ( ; kp2 < kp2end; ++kp2, ++kip2)
	  sum += kp2[0] * kip2[0];
#endif
      }

      op[0] = sum;
    }
  }

  /* This loop runs near the right boundary. */
  for (opend = out+w; op < opend; ++op, ++kip) {
    float sum = 0;
    type const *kip1 = kip;
    float const *kp1 = kp, *kp1end;
    unsigned int kw1 = kw;

    /* Adjust width of kernel near boundaries. */
    {
      int adj_l = kx-(op-out);
      int adj_r = kw-kx-(out+w-op);

      if (adj_l > 0) { kp1 += adj_l; kip1 += adj_l; kw1 -= adj_l; }
      if (adj_r > 0) { kw1 -= adj_r; }
    }

    if (kw1 > 0)
      for (kp1end = kp1+klen; kp1 < kp1end; kip1 += w, kp1 += kw) {
	type const *kip2 = kip1;
	float const *kp2 = kp1, *kp2end = kp2+kw1;

#if LOOP_AND_A_HALF
	float a, b, prod;
	a = (kp2++)[0];
	b = (kip2++)[0];
	if (kp2 < kp2end)
	  while (kp2 != kp2end) {
	    prod = a*b;
	    a = (kp2++)[0];
	    b = (kip2++)[0];
	    sum += prod;
	  }
	prod = a*b;
	sum += prod;
#else
	for ( ; kp2 < kp2end; ++kp2, ++kip2)
	  sum += kp2[0] * kip2[0];
#endif
      }

    op[0] = sum;
  }
}
