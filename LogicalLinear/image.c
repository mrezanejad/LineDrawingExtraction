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

#include "image.h"
#include "alloc.h"

image_bit *
DEFUN (new_image_bit, (width, height),
       int width AND int height)
{
  image_bit *im = new (image_bit);

  if (! im) return 0;

  im->width = width;
  im->height = height;

  im->data = newvc (bit, width*height);

  if (! im->data) free (im), im = 0;

  return im;
}

void
DEFUN (free_image_bit, (im), image_bit *im)
{
  free (im->data);
  free (im);
}

image_gray8 *
DEFUN (new_image_gray8, (width, height),
       int width AND int height)
{
  image_gray8 *im = new (image_gray8);

  if (! im) return 0;

  im->width = width;
  im->height = height;
  im->max = 0xff;

  im->data = newvc (gray8, width*height);

  if (! im->data) free (im), im = 0;

  return im;
}

void
DEFUN (free_image_gray8, (im), image_gray8 *im)
{
  free (im->data);
  free (im);
}

image_gray16 *
DEFUN (new_image_gray16, (width, height),
       int width AND int height)
{
  image_gray16 *im = new (image_gray16);

  if (! im) return 0;

  im->width = width;
  im->height = height;
  im->max = 0xffff;

  im->data = newvc (gray16, width*height);

  if (! im->data) free (im), im = 0;

  return im;
}

void
DEFUN (free_image_gray16, (im), image_gray16 *im)
{
  free (im->data);
  free (im);
}

image_gray32 *
DEFUN (new_image_gray32, (width, height),
       int width AND int height)
{
  image_gray32 *im = new (image_gray32);

  if (! im) return 0;

  im->width = width;
  im->height = height;
  im->max = 0xffffffff;

  im->data = newvc (gray32, width*height);

  if (! im->data) free (im), im = 0;

  return im;
}

void
DEFUN (free_image_gray32, (im), image_gray32 *im)
{
  free (im->data);
  free (im);
}

image_float *
DEFUN (new_image_float, (width, height),
       int width AND int height)
{
  image_float *im = new (image_float);

  if (! im) return 0;

  im->width = width;
  im->height = height;
  im->min = 0.0;
  im->max = 1.0;

  im->data = newvc (float, width*height);

  if (! im->data) free (im), im = 0;

  return im;
}

void
DEFUN (free_image_float, (im), image_float *im)
{
  free (im->data);
  free (im);
}

image_double *
DEFUN (new_image_double, (width, height),
       int width AND int height)
{
  image_double *im = new (image_double);

  if (! im) return 0;

  im->width = width;
  im->height = height;
  im->min = 0.0;
  im->max = 1.0;

  im->data = newvc (double, width*height);

  if (! im->data) free (im), im = 0;

  return im;
}

void
DEFUN (free_image_double, (im), image_double *im)
{
  free (im->data);
  free (im);
}
