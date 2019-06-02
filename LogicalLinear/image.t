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
#include "alloc.h"

#include <assert.h>
#include <stdio.h>

#define image_downscale CONCAT_type(image_downscale)

#if defined(COMPILE_FLOAT) || defined(COMPILE_DOUBLE)
#define ROUND(x,y) ((x)/(y))
#else
#define ROUND(x,y) (((x)+((y)/2))/(y))
#endif

image_type *
DEFUN (image_downscale, (in, scale),
       image_type const *in AND
       int scale)
{
  image_type *out = new (image_type);
  type const *p;
  type *op;

  int w = in->width, h = in->height;
  int nw = (w+scale-1)/scale, nh = (h+scale-1)/scale;
  int i, j, ni, nj;

  arith_type *accum = anewv (arith_type, nw);

  out->width = nw;
  out->height = nh;
#if defined(COMPILE_FLOAT) || defined(COMPILE_DOUBLE)
  out->min = in->min;
#endif
  out->max = in->max;
  out->data = op = newv (type, nw*nh);

  p = in->data;
  for (j = 0, nj; j < h; j += scale, ++nj) {
    int r, nrows = (scale > h-j ? h-j : scale);
    for (i = 0; i < nw; ++i) accum[i] = 0;
    for (r = 0; r < nrows; ++r) {
      arith_type *a = accum;
      for (i = 0, ni; i < w; i += scale, ++ni, ++a) {
	int c, ncols = (scale > w-i ? w-i : scale);
	arith_type sum = 0;
	for (c = 0; c < ncols; ++c, ++p)
	  sum += *p;
	*a += ROUND (sum, ncols);
      }
    }
    /* Now place the results in the output image */
    for (i = 0; i < nw; ++i) {
      *op++ = ROUND (accum[i], nrows);
    }
  }

  return out;
}
