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
#include "loglin.h"
#include "alloc.h"

#define inline
#include "loglin-il.h"
#undef inline

#include <math.h>

#ifndef HAVE_EXPF
#define expf(x) exp(x)
#endif

float
DEFUN (smooth_partition, (x, degree),
       float x AND float degree)
{
  x *= degree;
  if (x <= -0.5) return 0;
  if (x >=  0.5) return 1;

  {
    float f0 = expf (-1/(0.5+x));
    float f1 = expf (-1/(0.5-x));

    return f0/(f0+f1);
  }
}

float
DEFUN (abs_lland6, (x0, x1, x2, x3, x4, x5),
       float x0 AND float x1 AND float x2 AND
       float x3 AND float x4 AND float x5)
{
  float xn;

  xn = abs_lland2 (x0, x1);
  xn = abs_lland2 (xn, x2);
  xn = abs_lland2 (xn, x3);
  xn = abs_lland2 (xn, x4);
  xn = abs_lland2 (xn, x5);

  return xn;
}

float
DEFUN (contin_lland6, (degree, x0, x1, x2, x3, x4, x5),
       float degree AND
       float x0 AND float x1 AND float x2 AND
       float x3 AND float x4 AND float x5)
{
  float s0, s1, s2, s3, s4, s5;
  float prod = 1;

  prod *= (s0 = contin_partition (x0, degree));
  prod *= (s1 = contin_partition (x1, degree));
  prod *= (s2 = contin_partition (x2, degree));
  prod *= (s3 = contin_partition (x3, degree));
  prod *= (s4 = contin_partition (x4, degree));
  prod *= (s5 = contin_partition (x5, degree));

  {
    float sum = 0;
    sum += x0 * (prod + (1-s0));
    sum += x1 * (prod + (1-s1));
    sum += x2 * (prod + (1-s2));
    sum += x3 * (prod + (1-s3));
    sum += x4 * (prod + (1-s4));
    sum += x5 * (prod + (1-s5));

    return sum;
  }
}

float
abs_lland_n (args, n)
  float const *args;
  int n;
{
  float xn;
  int i;

  xn = abs_lland2 (args[0], args[1]);
  for (i = 2; i < n; ++i)
    xn = abs_lland2 (xn, args[i]);

  return xn;
}

float
DEFUN (abs_lland_np, (args, n),
       float const **args AND
       int n)
{
  float xn;
  int i;

  xn = abs_lland2 (*args[0], *args[1]);
  for (i = 2; i < n; ++i)
    xn = abs_lland2 (xn, *args[i]);

  return xn;
}

float
DEFUN (contin_lland_n, (degree, args, n),
       float degree AND
       float const *args AND
       int n)
{
  float *sigs, prod = 1, sum = 0;
  int i;

  sigs = anewv (float, n);

  for (i = 0; i < n; ++i)
    prod *= (sigs[i] = contin_partition (args[i], degree));

  for (i = 0; i < n; ++i)
    sum += args[i] * (prod + (1-sigs[i]));

  return sum;
}

float
DEFUN (contin_lland_np, (degree, args, n),
       float degree AND
       float const **args AND
       int n)
{
  float *sigs, prod = 1, sum = 0;
  int i;

  sigs = anewv (float, n);

  for (i = 0; i < n; ++i)
    prod *= (sigs[i] = contin_partition (*args[i], degree));

  for (i = 0; i < n; ++i)
    sum += *args[i] * (prod + (1-sigs[i]));

  return sum;
}

float
DEFUN (abs_llor6, (x0, x1, x2, x3, x4, x5),
       float x0 AND float x1 AND float x2 AND
       float x3 AND float x4 AND float x5)
{
  float xn;

  xn = abs_llor2 (x0, x1);
  xn = abs_llor2 (xn, x2);
  xn = abs_llor2 (xn, x3);
  xn = abs_llor2 (xn, x4);
  xn = abs_llor2 (xn, x5);

  return xn;
}

float
DEFUN (contin_llor6, (degree, x0, x1, x2, x3, x4, x5),
       float degree AND
       float x0 AND float x1 AND float x2 AND
       float x3 AND float x4 AND float x5)
{
  float s0, s1, s2, s3, s4, s5;
  float prod = 1;

  prod *= (s0 = contin_partition (-x0, degree));
  prod *= (s1 = contin_partition (-x1, degree));
  prod *= (s2 = contin_partition (-x2, degree));
  prod *= (s3 = contin_partition (-x3, degree));
  prod *= (s4 = contin_partition (-x4, degree));
  prod *= (s5 = contin_partition (-x5, degree));

  {
    float sum = 0;
    sum += x0 * (prod + (1-s0));
    sum += x1 * (prod + (1-s1));
    sum += x2 * (prod + (1-s2));
    sum += x3 * (prod + (1-s3));
    sum += x4 * (prod + (1-s4));
    sum += x5 * (prod + (1-s5));

    return sum;
  }
}

float
abs_llor_n (args, n)
  float const *args;
  int n;
{
  float xn;
  int i;

  xn = abs_llor2 (args[0], args[1]);
  for (i = 2; i < n; ++i)
    xn = abs_llor2 (xn, args[i]);

  return xn;
}

float
DEFUN (abs_llor_np, (args, n),
       float const **args AND
       int n)
{
  float xn;
  int i;

  xn = abs_llor2 (*args[0], *args[1]);
  for (i = 2; i < n; ++i)
    xn = abs_llor2 (xn, *args[i]);

  return xn;
}

float
DEFUN (contin_llor_n, (degree, args, n),
       float degree AND
       float const *args AND
       int n)
{
  float *sigs, prod = 1, sum = 0;
  int i;

  sigs = anewv (float, n);

  for (i = 0; i < n; ++i)
    prod *= (sigs[i] = contin_partition (-args[i], degree));

  for (i = 0; i < n; ++i)
    sum += args[i] * (prod + (1-sigs[i]));

  return sum;
}

float
DEFUN (contin_llor_np, (degree, args, n),
       float degree AND
       float const **args AND
       int n)
{
  float *sigs, prod = 1, sum = 0;
  int i;

  sigs = anewv (float, n);

  for (i = 0; i < n; ++i)
    prod *= (sigs[i] = contin_partition (-*args[i], degree));

  for (i = 0; i < n; ++i)
    sum += *args[i] * (prod + (1-sigs[i]));

  return sum;
}
