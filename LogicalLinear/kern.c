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
#include "conv.h"
#include "basis.h"
#include "loglin.h"
#include "alloc.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define DBG 0

/* Keep basis_comp opaque for now. */
typedef struct basis_comp basis_comp;

/* 
 * A basis description is, at this point, just a list of these opaque
 * components.
 */

typedef struct basis {
  int ncomps;
  basis_comp *comps;
} basis;

static basis *
make_divided_G_basis PARAMS((int n, float sigma, float degree, float stab));

static basis *
make_simple_basis PARAMS((double (*f)(), float scl, float off,
			  int nargs, float *args));

static basis *
make_line1_basis PARAMS((float sigma, float sep));

static basis *
make_line2_basis PARAMS((float sigma, float sep));

static basis *
make_edge1_basis PARAMS((float sigma, float sep));

static basis *
make_edge2_basis PARAMS((float sigma, float sep));

static void
free_basis PARAMS((basis *bas));

static void
fill_kernel PARAMS((fkernel *kern,
		    basis *bx, int i,
		    basis *by, int j,
		    float rot, float scl));

static int
make_basis_kernels PARAMS((unsigned int w, unsigned int h, int org_x, int org_y,
			   basis *bx, basis *by,
			   float rot, float scl,
			   int *nkernsp, fkernel **kernsp));

static int
normalize_kerns PARAMS((fkernel *kerns, int nkerns, float response));

static void
shrink_kerns PARAMS((fkernel *kern, int nkerns, float range));

static ikernel *
ikernelize PARAMS((fkernel *fkern, int imax));

void
DEFUN (free_fkernel, (kern),
       fkernel *kern)
{
  free (kern->data);
  free (kern);
}

void
DEFUN (free_ikernel, (kern),
       ikernel *kern)
{
  free (kern->data);
  free (kern);
}

void
DEFUN (free_cxkernel, (kern),
       cxkernel *kern)
{
  int i;

  for (i = 0; i < kern->nkerns; ++i)
    free (kern->kerns[i].data);
  free (kern->kerns);
  free (kern->ops);

  free (kern);
}

static cxkernel *
make_divided_cxkern PARAMS((float dir, int size, basis *by, 
			    float sx, int n, float degree, float stab));

cxkernel *
DEFUN (make_line1_cxkern, (dir, sx, n, degree, stab, sy, sep),
       float dir AND 
       float sx AND
       int n AND
       float degree AND 
       float stab AND
       float sy AND
       float sep)
{
  basis *by = make_line1_basis (sy, sep);
  int size = ceil (8*MAX(sx,sy) + sep);

  cxkernel *cxkern = make_divided_cxkern (dir, size, by, sx, n, degree, stab);

  free_basis (by);
  
  return cxkern;
}

cxkernel *
DEFUN (make_line2_cxkern, (dir, sx, n, degree, stab, sy, sep),
       float dir AND
       float sx AND
       int n AND
       float degree AND
       float stab AND
       float sy AND
       float sep)
{
  basis *by = make_line2_basis (sy, sep);
  int size = ceil (8*MAX(sx,sy) + sep);

  cxkernel *cxkern = make_divided_cxkern (dir, size, by, sx, n, degree, stab);

  free_basis (by);
  
  return cxkern;
}

cxkernel *
DEFUN (make_edge1_cxkern, (dir, sx, n, degree, stab, sy, sep),
       float dir AND
       float sx AND
       int n AND
       float degree AND
       float stab AND
       float sy AND
       float sep)
{
  basis *by = make_edge1_basis (sy, sep);
  int size = ceil (8*MAX(sx,sy) + sep);

  cxkernel *cxkern = make_divided_cxkern (dir, size, by, sx, n, degree, stab);

  free_basis (by);
  
  return cxkern;
}

cxkernel *
DEFUN (make_edge2_cxkern, (dir, sx, n, degree, stab, sy, sep),
       float dir AND
       float sx AND
       int n AND
       float degree AND 
       float stab AND
       float sy AND 
       float sep)
{
  basis *by = make_edge2_basis (sy, sep);
  int size = ceil (8*MAX(sx,sy) + sep);

  cxkernel *cxkern = make_divided_cxkern (dir, size, by, sx, n, degree, stab);

  free_basis (by);
  
  return cxkern;
}

static void
make_endline_cxops PARAMS((int nx, int ny, int *nopsp, cxop **opsp));

static cxkernel *
DEFUN (make_divided_cxkern, (dir, size, by, sx, n, degree, stab),
       float dir AND
       int size AND
       basis *by AND
       float sx AND
       int n AND
       float degree AND float stab)
{
  cxkernel *cxkern = new (cxkernel);

  basis *bx = make_divided_G_basis (n, sx, degree, stab);
    
  make_basis_kernels (size, size, size/2, size/2, bx, by, dir, 1.0,
		      &cxkern->nkerns, &cxkern->kerns);

  normalize_kerns (cxkern->kerns, cxkern->nkerns, 1.0);
  shrink_kerns (cxkern->kerns, cxkern->nkerns, 127.0);

  make_endline_cxops (bx->ncomps, by->ncomps, &cxkern->nops, &cxkern->ops);

  free_basis (bx);
  
  return cxkern;
}

static cxop *
combine_region PARAMS((cxop *op, int r, int n));

static void
DEFUN (make_endline_cxops, (nx, ny, nopsp, opsp),
       int nx AND int ny AND
       int *nopsp AND
       cxop **opsp)
{
  int nops = (ny+1)*nx + 5;
  cxop *ops = newv (cxop, nops), *op = ops;

  if (nx == 1) {
    op = combine_region (op, 0, ny);
  } else if (nx%2 == 0) {
    int n_side = nx/2 - 1;
    int j;

    /* Both centre regions must be positive. */
    op = combine_region (op, n_side, ny);
    op = combine_region (op, n_side+1, ny);
    op->id = CXand, op->cx.iarg = 2, ++op;

    /* And then either the "and" of the left or the "and" of the right. */
    for (j = 0; j < n_side; ++j)
      op = combine_region (op, j, ny);
    if (n_side > 1)
      op->id = CXand, op->cx.iarg = n_side, ++op;
    for (j = n_side+2; j < nx; ++j)
      op = combine_region (op, j, ny);
    if (n_side > 1)
      op->id = CXand, op->cx.iarg = n_side, ++op;

    /* Inside and outside must both be on. */
    if (n_side > 0) {
      op->id = CXor,  op->cx.iarg = 2, ++op;
      op->id = CXand, op->cx.iarg = 2, ++op;
    }
  } else {
    int n_side = nx/2 + 1;
    int j;

    /* Centre region must be positive. */
    op = combine_region (op, n_side, ny);

    /* And then either the "and" of the left or the "and" of the right. */
    for (j = 0; j < n_side; ++j)
      op = combine_region (op, j, ny);
    if (n_side > 1)
      op->id = CXand, op->cx.iarg = n_side, ++op;
    for (j = n_side+2; j < nx; ++j)
      op = combine_region (op, j, ny);
    if (n_side > 1)
      op->id = CXand, op->cx.iarg = n_side, ++op;

    /* Inside and outside must both be on. */
    if (n_side > 0) {
      op->id = CXor,  op->cx.iarg = 2, ++op;
      op->id = CXand, op->cx.iarg = 2, ++op;
    }
  }

  *nopsp = nops = op - ops;
  *opsp = renewv (cxop, ops, nops);
}

static cxop *
DEFUN (combine_region, (op, r, n),
       cxop *op AND
       int r AND int n)
{
  int b = n*r, j;
  for (j = 0; j < n; ++j)
    op->id = CXconvolve, op->cx.iarg = b+j, ++op;
  op->id = CXand, op->cx.iarg = n, ++op;

  return op;
}

static basis *
make_selected_basis PARAMS((int ncomps,
			    double (*f)(), float scale, float off,
			    int nargs, float *args));

static double
divided_G PARAMS((double x, double r, double n,
		  double sigma, double degree, double stab));

static void
compute_partition_points PARAMS((int n, float degree));

static basis *
DEFUN (make_divided_G_basis, (n, sx, degree, stab),
       int n AND
       float sx AND float degree AND float stab)
{
  float args[4];
  args[0] = n; args[1] = sx; args[2] = degree; args[3] = stab;

  compute_partition_points (n, degree);

  return make_selected_basis (n, divided_G, 1.0, 0.0, 4, args);
}

/*
 * This is the tangential division which is used to ensure continuity through
 * the centre of an operator.  In essence, a Gaussian (of width <sigma>) is
 * partitioned into <n> regions (the function assumes we are calculating
 * region number <r> between 0 and <n-1>).  If <stab> is non-zero, then a
 * stabilizer is added to the central two regions.
 */

#define max_parts (8)
static float part_off[max_parts+1][max_parts];
static float part_degree[max_parts+1];

static double
DEFUN (divided_G, (x, r_, n_, sigma, degree, stab),
       double x AND
       double r_ AND double n_ AND
       double sigma AND double degree AND double stab)
{
  int r = r_, n = n_;
  float v;
  float const *part = part_off[n];

  assert (r >= 0 && r < n);

  degree *= sigma;

  v = Gf (x, sigma);
  if (n > 1) {
    if (r == 0)
      /* Leftmost region. */
      v *= smooth_partition (-(x-part[r  ]*sigma), degree);
    else if (r == n-1)
      /* Rightmost region. */
      v *= smooth_partition ( (x-part[r-1]*sigma), degree);
    else
      /* Interior regions. */
      v *= (  smooth_partition (-(x-part[r  ]*sigma), degree)
	    + smooth_partition ( (x-part[r-1]*sigma), degree)
	    - 1.0);

    if (stab > 0) {
      assert (n%2 == 0);

      if (r == n/2-1)
	v += stab * sigma * dG (x, sigma);
      else if (r == n/2)
	v -= stab * sigma * dG (x, sigma);
    }
  }

  return v;
}

static void
DEFUN (compute_partition_points, (n, degree),
       int n AND
       float degree)
{
#define XVAL(i) (2*range*(i)/w - range)
  const int w = 256;
  const float range = 8;
  float *vals = anewv (float, w);
  int i, j;
  float y, sum;

  assert (n <= max_parts);

  /* Weakly memoize on the basis of degree. */
  if (part_degree[n] == degree) return;
  part_degree[n] = degree;

  /* Compute the integral of the Gaussian... */
  for (i = 0, sum = 0; i < w; ++i)
    vals[i] = (sum += Gf (2*range*i/w - range, 1.0));
  /* and normalize to 1.0 */
  for (i = 0; i < w; ++i)
    vals[i] /= sum;

  /*
   * This is a bad first approximation to the partition point, since it is
   * independent of <degree>.
   */
  for (i = 0, j = 0, y = 1.0/n; i < w-1; ++i) {
    if (vals[i] <= y && vals[i+1] > y) {
      float y0 = vals[i]   - y;
      float y1 = vals[i+1] - y;
      float x0 = XVAL(i+0.5);
      float x1 = XVAL(i+1.5);
      part_off[n][j] = (x1*y0 - x0*y1)/(y0-y1);
#if 0
      fprintf (stderr, "[%6.3f,%6.3f->%6.3f,%6.3f] Part[%d/%d] = %6.3f\n",
	       y, x0, y0, x1, y1, 
	       j, n, part_off[n][j]);
#endif
      if (++j >= n) break;
      y += 1.0/n;
    }
  }
#undef XVAL
}

/*
 * Scale the kernels so that the maximum *linear* response from the kernels is
 * as specified.  All kernels must be of identical size and origin.
 */

static int
DEFUN (normalize_kerns, (kerns, nkerns, response),
       fkernel *kerns AND
       int nkerns AND
       float response)
{
  unsigned int kw = kerns[0].width, kh = kerns[0].height, len = kw*kh;
  int org_x = kerns[0].origin_x, org_y = kerns[0].origin_y;

  float *sum = newv (float, len);
  float *sp, *spend;
  float scale;
  int i;

  /* Clear summing array. */
  for (sp = sum, spend = sp+len; sp < spend; ++sp) sp[0] = 0;

  /* Linearly combine (add) kernel elements. */
  for (i = 0; i < nkerns; ++i) {
    float *kp = kerns[i].data;
    if (kerns[i].width != kw
	|| kerns[i].height != kh
	|| kerns[i].origin_x != org_x
	|| kerns[i].origin_y != org_y) {
      fprintf (stderr, "Cannot normalize, kernels incompatible.\n");
      return 0;
    }
    for (sp = sum, spend = sp+len; sp < spend; ++sp, ++kp)
      sp[0] += kp[0];
  }

  /* Compute potential positive response. */
  {
    float pos = 0, neg = 0;
    for (sp = sum, spend = sp+len; sp < spend; ++sp) {
      float v = sp[0];
      if (v > 0) pos += v; else neg += v;
    }

    scale = response/pos;
  }

  if (DBG)
    fprintf (stderr, "Normalize kernel set... scale = %f\n", scale);

  /* Scale all kernels in order to guarantee response. */
  for (i = 0; i < nkerns; ++i) {
    float *kp = kerns[i].data, *kpend;
    for (kpend = kp+len; kp < kpend; ++kp)
      kp[0] *= scale;
  }
  
#if 0
  {
    fkernel sumk = {kw, kh, sum, org_x, org_y};
    cxop cxops[1] = {{CXconvolve, 1}};
    cxkernel cxkern = {1, &sumk, 1, cxops};

    ikernelize (&sumk, 127);
    printf ("Linear equivalent:\n");
    write_kern (stdout, &cxkern);
  }
#endif

  return 1;
}

/*
 * Shrinkwarp the kernels so that the range of values between largest 
 * and smallest is no greater than range.
 */

static void
DEFUN (shrink_kerns, (kern, nkerns, range),
       fkernel *kern AND
       int nkerns AND
       float range)
{
  for ( ; nkerns; ++kern, --nkerns) {
    unsigned int kw = kern->width, kh = kern->height, klen = kw*kh;
    int org_x = kern->origin_x, org_y = kern->origin_y;
    float *data = kern->data;
    float min = 1/range;

    if (DBG)
      fprintf (stderr, "Shrink min=%f %08x [%dx%d] (%d,%d) => ",
	       min, (unsigned int) kern, kw, kh, org_x, org_y);

    /*
     * Find maximum absolute entry in kernel and then determine appropriate
     * value scaling.
     */
    {
      float max = 0;
      float *kp, *kpend;
      for (kp = kern->data, kpend = kp+klen; kp < kpend; ++kp) {
	float v = fabs (kp[0]);
	if (v > max) max = v;
      }
      min *= max;
    }

    /* Trim zeroed boundaries from kernel. */
    {
      unsigned char *row_nonzero = anewvc (unsigned char, kh);
      unsigned char *col_nonzero = anewvc (unsigned char, kw);
      int nw = kw, nh = kh;
      int offx = 0, offy = 0;

      /* Fill nonzero arrays where nonzero entries exist in kernel. */
      {
	float *ip1, *ip1end;
	int row = 0;
	for (ip1 = data, ip1end = data+klen; ip1 < ip1end; ip1 += kw, ++row) {
	  float *ip2, *ip2end;
	  if (DBG) fprintf (stderr, "\n");
	  for (ip2 = ip1, ip2end = ip2+kw; ip2 < ip2end; ++ip2) {
	    unsigned char nonzero = (fabs (ip2[0]) > min);
	    if (DBG) fprintf (stderr, "%d", nonzero);
	    row_nonzero[row] |= nonzero;
	    col_nonzero[ip2-ip1] |= nonzero;
	  }
	}
      }
      if (DBG) fprintf (stderr, "\n");

      /*
       * Determine how many rows to trim off top/bottom and how many columns to
       * trim off sides.
       */
      {
	int i;
	/* Find top trim. */
	for (i = 0; i < kh; ++i) {
	  if (row_nonzero[i]) break;
	  else                { --nh; ++offy; }
	}
	/* Find bottom trim. */
	for (i = kh-1; i >= 0; --i) {
	  if (row_nonzero[i]) break;
	  else                --nh;
	}
	/* Find left trim. */
	for (i = 0; i < kw; ++i) {
	  if (col_nonzero[i]) break;
	  else                { --nw; ++offx; }
	}
	/* Find right trim. */
	for (i = kw-1; i >= 0; --i) {
	  if (col_nonzero[i]) break;
	  else                --nw;
	}
      }

      /* Now trim by copying and resize kernel array. */
      if (nw < kw || nh < kh) {
	float *ip1 = data+kw*offy+offx, *ip1end;
	float *op = data;
	for (ip1end = ip1+kw*nh; ip1 < ip1end; ip1 += kw) {
	  float *ip2, *ip2end;
	  for (ip2 = ip1, ip2end = ip2+nw; ip2 < ip2end; ++ip2, ++op)
	    op[0] = fabs (ip2[0]) > min ? ip2[0] : 0.0;
	}

	/* Adjust parameters describing size of kernel. */
	kw = nw; kh = nh;
	org_x -= offx; org_y -= offy;
	data = renewv (float, data, kw*kh);
      }
    }

    if (DBG) fprintf (stderr, "[%dx%d] (%d,%d)\n", kw, kh, org_x, org_y);

    kern->width = kw; kern->height = kh;
    kern->origin_x = org_x; kern->origin_y = org_y;
    kern->data = data;
  }
}

/*
 * Find closest ikernel which is equivalent to this fkernel.  The integral
 * values in the new kernel are scaled so that the maximum entry in the kernel
 * is <imax>.
 */

static ikernel *
DEFUN (ikernelize, (fkern, imax),
       fkernel *fkern AND
       int imax)
{
  unsigned int kw = fkern->width, kh = fkern->height, klen = kw*kh;
  int org_x = fkern->origin_x, org_y = fkern->origin_y;
  float scale;
  ikernel *ikern = new (ikernel);
  int *idata = newv (int, klen);

  /*
   * Find maximum absolute entry in kernel and then determine appropriate
   * value scaling.
   */
  {
    float max = 0, amax = 0;
    float *kp, *kpend;
    for (kp = fkern->data, kpend = kp+klen; kp < kpend; ++kp) {
      float v = kp[0], av = fabs (v);
      if (av > amax) { amax = av; max = v; }
    }
    scale = (max > 0 ? imax/max : -imax/max);
  }

  /* Scale kernel elements by computed value and round to integer. */
  {
    float *kp, *kpend;
    int *ip = idata;
    for (kp = fkern->data, kpend = kp+klen; kp < kpend; ++kp, ++ip)
      ip[0] = rint (scale * kp[0]);
  }

  /* Trim zeroed boundaries from kernel. */
  {
    unsigned char *row_nonzero = anewvc (unsigned char, kh);
    unsigned char *col_nonzero = anewvc (unsigned char, kw);
    int nw = kw, nh = kh;
    int offx = 0, offy = 0;

    /* Fill nonzero arrays where nonzero entries exist in kernel. */
    {
      int *ip1, *ip1end, row = 0;
      for (ip1 = idata, ip1end = idata+klen; ip1 < ip1end; ip1 += kw, ++row) {
	int *ip2, *ip2end;
	for (ip2 = ip1, ip2end = ip2+kw; ip2 < ip2end; ++ip2) {
	  unsigned char nonzero = (ip2[0] != 0);
	  row_nonzero[row] |= nonzero;
	  col_nonzero[ip2-ip1] |= nonzero;
	}
      }
    }

    /*
     * Determine how many rows to trim off top/bottom and how many columns to
     * trim off sides.
     */
    {
      int i;
      /* Find top trim. */
      for (i = 0; i < kh; ++i) {
	if (row_nonzero[i]) break;
	else                { --nh; ++offy; }
      }
      /* Find bottom trim. */
      for (i = kh-1; i >= 0; --i) {
	if (row_nonzero[i]) break;
	else                --nh;
      }
      /* Find left trim. */
      for (i = 0; i < kw; ++i) {
	if (col_nonzero[i]) break;
	else                { --nw; ++offx; }
      }
      /* Find right trim. */
      for (i = kw-1; i >= 0; --i) {
	if (col_nonzero[i]) break;
	else                --nw;
      }
    }

    /* Now trim by copying and resize kernel array. */
    if (nw < kw || nh < kh) {
      int *ip1 = idata+kw*offy+offx, *ip1end;
      int *op = idata;
      for (ip1end = ip1+kw*nh; ip1 < ip1end; ip1 += kw) {
	int *ip2, *ip2end;
	for (ip2 = ip1, ip2end = ip2+nw; ip2 < ip2end; ++ip2, ++op)
	  op[0] = ip2[0];
      }
      kw = nw; kh = nh;
      org_x -= offx; org_y -= offy;
      idata = renewv (int, idata, kw*kh);
    }
  }

  ikern->width = kw; ikern->height = kh;
  ikern->origin_x = org_x; ikern->origin_y = org_y;
  ikern->data = idata;
  ikern->divisor = scale;

  return ikern;
}

/*
 * These structures define a one-dimensional basis for building a set of
 * linear convolution operators to be combined by some L/L combination.  Each
 * such basis can be described as <ncomps> components, each component being a
 * function taking <nargs>+1 arguments.  The first argument provided to the
 * generating function <f> is the position offset by <off>, and subsequent
 * arguments (all floats) are taken from the <args> list.
 *
 * For example, the structure describing a simple Gaussian smoothing operator
 * can be described as:
 *
 *     struct basis G_op = { 1, [{ G, 1.0, 0.0, 1, [sigma_x] }] };
 *
 * where [a, b, ...] is a pointer to an array of {a, b, ...}.
 *
 * A simple local maxima selective L/L operator is built as:
 *
 *     struct basis max_op = {2, [{dG,  1.0, -0.5, 1, [sigma_x]},
 *                                {dG, -1.0,  0.5, 1, [sigma_x]}]};
 */

/*typedef*/ struct basis_comp { 
  double (*f)();
  float scale, off;
  int nargs;
  float *args;
} /*basis_comp*/;

static float
eval_basis_comp PARAMS((basis_comp *comp, float x));

/*
 * This is the function used to evaluate a particular point in a given basis
 * description.  It is passed the component <comp> and point <x> and calls the
 * basis function <f> with <x-off> follwed by the extra arguments stored in
 * <comp->args>.  The result is multiplied by <scale>.
 */

static float
DEFUN (eval_basis_comp, (comp, x),
       basis_comp *comp AND
       float x)
{
  float v;
  int n = comp->nargs;
  float *a = comp->args;

  x -= comp->off;
  switch (n) {
  case 0: v = (*comp->f) (x);                                     break;
  case 1: v = (*comp->f) (x, a[0]);                               break;
  case 2: v = (*comp->f) (x, a[0], a[1]);                         break;
  case 3: v = (*comp->f) (x, a[0], a[1], a[2]);                   break;
  case 4: v = (*comp->f) (x, a[0], a[1], a[2], a[3]);             break;
  case 5: v = (*comp->f) (x, a[0], a[1], a[2], a[3], a[4]);       break;
  case 6: v = (*comp->f) (x, a[0], a[1], a[2], a[3], a[4], a[5]); break;

  default:
    fprintf (stderr, "Can't evaluate %d argument basis_comp structs.\n", n);
    return 0;
  }

  return comp->scale * v;
}

/*
 * This creates a basis set with <ncomps> components.  They all use the same
 * generating function, but the calls to subsequent components differ by the
 * insertion of an additional argument prior to args, which selects which
 * component to use.  This argument is an integer (passed as a float) from 0
 * to <ncomps>-1.
 */

static basis *
DEFUN (make_selected_basis, (ncomps, f, scale, off, nargs, args),
       int ncomps AND
       double (*f)() AND
       float scale AND float off AND
       int nargs AND
       float *args)
{
  basis_comp *comps = newv (basis_comp, ncomps);
  basis *basisp = new (basis);
  int i;
  
  basisp->ncomps = ncomps;
  basisp->comps = comps;

  /* Fill components. */
  for (i = 0; i < ncomps; ++i) {
    comps[i].f = f;
    comps[i].scale = scale;
    comps[i].off = off;
    comps[i].nargs = nargs+1;
    comps[i].args = newv (float, nargs+1);
    comps[i].args[0] = (float) i;
    memcpy (&comps[i].args[1], args, sizeof (float) * nargs);
  }

  return basisp;
}

/* 
 * construct the description of a single component basis using the convolution
 * function <f> scaled by <scl> and offset from the origin by <off>.  The
 * <nargs> extra arguments <args> will be passed to each call of <f>
 */

static basis *
DEFUN (make_simple_basis, (f, scl, off, nargs, args),
       double (*f)() AND
       float scl AND float off AND
       int nargs AND
       float *args)
{
  basis_comp *comp = new (basis_comp);
  basis *basisp = new (basis);

  comp->f = f;
  comp->scale = scl;
  comp->off = off;
  comp->nargs = nargs;
  comp->args = clonev (float, args, nargs);
  
  basisp->ncomps = 1;
  basisp->comps = comp;

  return basisp;
}

/* 
 * Creates a basis description for the 1st line operator definition.
 * This only includes line conditions up to 2nd derivatives.
 */

static basis *
DEFUN (make_line1_basis, (sigma, sep),
       float sigma AND float sep)
{
  basis_comp *comps = newv (basis_comp, 2);
  basis *bas = new (basis);

  /* Base scale such that max response is 1.0.  Must scale by # components. */
  float base_scl = M_2_SQRTPI/sigma;
  base_scl /= 2;

  {
    float scl = base_scl;

    /* Scale derivative estimate by separation. */
    scl /= 2*sep;

    comps[0].f = dG;
    comps[0].scale = scl;
    comps[0].off = sep;
    comps[0].nargs = 1;
    comps[0].args = new (float); comps[0].args[0] = sigma;
  
    comps[1].f = dG;
    comps[1].scale = -scl;
    comps[1].off = -sep;
    comps[1].nargs = 1;
    comps[1].args = new (float); comps[1].args[0] = sigma;
  }
  
  bas->ncomps = 2;
  bas->comps = comps;

  return bas;
}

/* 
 * Creates a basis description for the 2nd line operator definition.
 * This includes line conditions up to 4th derivatives.
 */

static basis *
DEFUN (make_line2_basis, (sigma, sep),
       float sigma AND float sep)
{
  basis_comp *comps = newv (basis_comp, 4);
  basis *bas = new (basis);

  /* Base scale such that max response is 1.0.  Must scale by # components. */
  float base_scl = M_2_SQRTPI/sigma;
  base_scl /= 3;

  {
    float scl = base_scl;

    /* Scale derivative estimate by separation. */
    scl /= 2*sep;

    comps[0].f = dG;
    comps[0].scale = scl;
    comps[0].off = sep;
    comps[0].nargs = 1;
    comps[0].args = new (float); comps[0].args[0] = sigma;
  
    comps[1].f = dG;
    comps[1].scale = -scl;
    comps[1].off = -sep;
    comps[1].nargs = 1;
    comps[1].args = new (float); comps[1].args[0] = sigma;
  }
  
  {
    /* This value is (d3G*Step)(0)/(dG*Step)(0) */
    float scl = base_scl*M_SQRT2*sigma*sigma/8;

    /* Scale derivative estimate by separation. */
    scl /= 2*sep;

    comps[2].f = d3G;
    comps[2].scale = -scl;
    comps[2].off = sep;
    comps[2].nargs = 1;
    comps[2].args = new (float); comps[2].args[0] = sigma/M_SQRT2;
  
    comps[3].f = d3G;
    comps[3].scale = scl;
    comps[3].off = -sep;
    comps[3].nargs = 1;
    comps[3].args = new (float); comps[3].args[0] = sigma/M_SQRT2;
  }
  
  bas->ncomps = 4;
  bas->comps = comps;

  return bas;
}

/* 
 * Creates a basis description for the 1st edge operator definition.
 * This includes edge conditions up to 3rd derivatives.
 */

static basis *
DEFUN (make_edge1_basis, (sigma, sep),
       float sigma AND float sep)
{
  basis_comp *comps = newv (basis_comp, 3);
  basis *bas = new (basis);

  /* Base scale such that max response is 1.0.  Must scale by # components. */
  float base_scl = M_2_SQRTPI/sigma;
  base_scl /= 2;

  comps[0].f = dG;
  comps[0].scale = base_scl;
  comps[0].off = 0.0;
  comps[0].nargs = 1;
  comps[0].args = new (float); comps[0].args[0] = sigma;
  
  {
    /* This value is (d3G*Step)(0)/(dG*Step)(0) */
    float scl = base_scl*sigma*sigma/2;

    /* Scale derivative estimate by separation. */
    scl /= 2*sep;

    comps[1].f = d2G;
    comps[1].scale = scl;
    comps[1].off = sep;
    comps[1].nargs = 1;
    comps[1].args = new (float); comps[1].args[0] = sigma;
  
    comps[2].f = d2G;
    comps[2].scale = -scl;
    comps[2].off = -sep;
    comps[2].nargs = 1;
    comps[2].args = new (float); comps[2].args[0] = sigma;
  }
  
  bas->ncomps = 3;
  bas->comps = comps;

  return bas;
}

/* 
 * Creates a basis description for the 2nd edge operator definition.
 * This includes edge conditions up to 5th derivatives.
 */

static basis *
DEFUN (make_edge2_basis, (sigma, sep),
       float sigma AND float sep)
{
  basis_comp *comps = newv (basis_comp, 5);
  basis *bas = new (basis);

  /* Base scale such that max response is 1.0. */
  float base_scl = M_2_SQRTPI/sigma;

  if (DBG) 
    fprintf (stderr, "edge2: sigma=%g => base_scale = %g\n", sigma, base_scl);

  /* Scale inversely by # of components. */
  base_scl /= 3;

  if (DBG) fprintf (stderr, "  3 comps => base_scale = %g\n", base_scl);

  comps[0].f = dG;
  comps[0].scale = base_scl;
  comps[0].off = 0.0;
  comps[0].nargs = 1;
  comps[0].args = new (float); comps[0].args[0] = sigma;
  
  {
    /* This value is (dG*Step)(0)/(d3G*Step)(0). */
    float scl = base_scl*sigma*sigma/2;

    if (DBG) fprintf (stderr, "  d3G sigma=%g => scale = %g\n", sigma, scl);

    /* Scale derivative estimate by separation. */
    scl /= 2*sep;

    if (DBG) fprintf (stderr, "  d3G sep=%g => scale = %g\n", sep, scl);

    comps[1].f = d2G;
    comps[1].scale = scl;
    comps[1].off = sep;
    comps[1].nargs = 1;
    comps[1].args = new (float); comps[1].args[0] = sigma;
  
    comps[2].f = d2G;
    comps[2].scale = -scl;
    comps[2].off = -sep;
    comps[2].nargs = 1;
    comps[2].args = new (float); comps[2].args[0] = sigma;
  }

  {
    /* This value is (dG*Step)(0)/(d5G*Step)(0). */
    float scl = base_scl*M_SQRT2*sigma*sigma*sigma*sigma/96;

    if (DBG) fprintf (stderr, "  d5G sigma=%g => scale = %g\n", sigma, scl);

    /* Scale derivative estimate by separation. */
    scl /= 2*sep;

    if (DBG) fprintf (stderr, "  d3G sep=%g => scale = %g\n", sep, scl);

    comps[3].f = d4G;
    comps[3].scale = -scl;
    comps[3].off = sep;
    comps[3].nargs = 1;
    comps[3].args = new (float); comps[3].args[0] = sigma/M_SQRT2;
  
    comps[4].f = d4G;
    comps[4].scale = scl;
    comps[4].off = -sep;
    comps[4].nargs = 1;
    comps[4].args = new (float); comps[4].args[0] = sigma/M_SQRT2;
  }

  bas->ncomps = 5;
  bas->comps = comps;

  return bas;
}

/*
 * Free the whole structure associated with the basis pointer <bas>.
 */

static void
DEFUN (free_basis, (bas),
       basis *bas)
{
  int i;
  for (i = 0; i < bas->ncomps; ++i)
    free (bas->comps[i].args);
  free (bas->comps);
  free (bas);
}


/*
 * Compute the convolution kernel associated with component <i> of basis <bx>
 * and component <j> of basis <by> rotated by <rot> and scaled (element-wise)
 * by <scl>.
 */

static void
DEFUN (fill_kernel, (kern, bx, i, by, j, rot, scl),
       fkernel *kern AND
       basis *bx AND int i AND
       basis *by AND int j AND
       float rot AND float scl)
{
  unsigned int kw = kern->width, kh = kern->height, klen = kw*kh;
  int org_x = kern->origin_x, org_y = kern->origin_y;
  float *kp = kern->data, *kpend;
  float cr = cosf (rot), sr = sinf (rot);
  float y;

  y = -org_y;
  for (kpend = kp+klen; kp < kpend; ++y) {
    float *kp2 = kp, *kp2end;
    float x = -org_x;
    for (kp2end = kp+kw; kp2 < kp2end; ++kp2, ++x) {
      float v = scl;
      v *= eval_basis_comp (&bx->comps[i],  cr*x + sr*y);
      v *= eval_basis_comp (&by->comps[j], -sr*x + cr*y);
      kp2[0] = v;
    }
    kp = kp2;
  }
}

/*
 * This creates the 2D component kernels from the orthogonal 1D bases <bx> and
 * <by> provided.  If both bases have more than one component, then the
 * component kernels are created in the following order:
 *
 *   bx[0]*by[0], bx[0]*by[1], ..., bx[1]*by[0], bx[1]*by[1], ...
 *
 * <nkerns = bx->ncomps * by->ncomps> kernels are allocated and the values
 * returned in the locations pointed to by <nkernsp> and <kernsp>.
 */

static int
DEFUN (make_basis_kernels, (w, h, org_x, org_y, bx, by,
			    rot, scl, nkernsp, kernsp),
       unsigned int w AND unsigned int h AND
       int org_x AND int org_y AND
       basis *bx AND basis *by AND
       float rot AND float scl AND
       int *nkernsp AND
       fkernel **kernsp)
{
  int nx = bx->ncomps, ny = by->ncomps, nkerns = nx * ny;
  fkernel *kerns = newv (fkernel, nkerns), *kp;
  int i, j;
  
  for (i = 0, kp = kerns; i < nx; ++i) {
    for (j = 0; j < ny; ++j, ++kp) {
      kp->width = w; kp->height = h;
      kp->origin_x = org_x; kp->origin_y = org_y;
      kp->data = newv (float, w*h);

      fill_kernel (kp, bx, i, by, j, rot, scl);
    }
  }

  *nkernsp = nkerns;
  *kernsp = kerns;

  return 1;
}

/*
 * Write a description of the complex kernel <cxkern> on the stream <f>.  The
 * description is ASCII and human readable.  The kernels are first described
 * as convolution masks, followed by the complex operations in a kind of RPN
 * format.  The four complex operations are described as: "conv", "and", "or",
 * and "scale".
 */

int
DEFUN (write_kern, (f, cxkern),
       FILE *f AND
       cxkernel *cxkern)
{
  int nkerns = cxkern->nkerns;
  fkernel *kern = cxkern->kerns;
  int nops = cxkern->nops;
  cxop *op = cxkern->ops;

  for ( ; nkerns; ++kern, --nkerns) {
    ikernel *ikern = ikernelize (kern, 127);
    int *ip = ikern->data, *lineend, *ipend;
    unsigned int kw = ikern->width, kh = ikern->height;
    fprintf (f, "kern %d %d %d %d %d\n",
	     ikern->width, ikern->height,
	     ikern->origin_x, ikern->origin_y, ikern->divisor);
    for (lineend = ip+kw, ipend = ip+kw*kh; ip < ipend; ++ip) {
      if (ip == lineend) { fprintf (f, "\n"); lineend = ip+kw; }
      fprintf (f, " %4d", ip[0]);
    }
    fprintf (f, "\n\n");
    free_ikernel (ikern);
  }

  for ( ; nops; ++op, --nops) {
    switch (op->id) {
    case CXconvolve:
      fprintf (f, "conv %d\n", op->cx.iarg);
      break;
    case CXand:
      fprintf (f, "and %d\n", op->cx.iarg);
      break;
    case CXor:
      fprintf (f, "or %d\n", op->cx.iarg);
      break;
    case CXscale:
      fprintf (f, "scale %g\n", op->cx.farg);
      break;
    default:
      fprintf (stderr, "Bad cxkernel op %d\n", cxkern->nops-nops);
      return 0;
    }
  }

  return 1;
}

/* Read in a complete line. */
static int
DEFUN (get_line, (bufp, buf_lenp, f),
       char **bufp AND
       int *buf_lenp AND
       FILE *f)
{
  char *buf = *bufp;
  int buf_len = *buf_lenp;
  int n, eof;

  buf[0] = '\0';
  eof = !fgets (buf, buf_len, f);

  while (buf[(n=strlen(buf))-1] != '\n') {
    *buf_lenp = buf_len *= 2;
    *bufp = buf = renewv (char, buf, buf_len);
    eof = !fgets (buf+n, buf_len-n, f);
    if (eof) break;
  }

  return eof;
}

#define STRNEQ !strncasecmp

/*
 * Read a description of a complex kernel from the stream <f> and return the
 * corresponding <cxkernel> structure.
 */

cxkernel *
DEFUN (read_kern, (f, kern_file),
       FILE *f AND
       char const *kern_file)
{
  int buf_len = 128;
  char *buf = newv (char, buf_len);
  int eof, lineno = 0;

  cxkernel *kern;
  int nops = 0, ops_len = 8;
  int nkerns = 0, kerns_len = 8;

  if (! f) return 0;

  kern = newc (cxkernel);
  kern->ops = newvc (cxop, ops_len);
  kern->kerns = newvc (fkernel, kerns_len);

  do {
    char *p;
    char *errmsg;
    int n;

    eof = get_line (&buf, &buf_len, f);
    ++lineno;

    /* Any text after # is commented out. */
    if ((p = strchr(buf, '#')))
      *p = '\0';

    /* Skip over initial white space. */
    for (p = buf; *p && strchr (" \t\n", *p); ++p);

    /* Ignore line if empty. */
    if (! *p) continue;

    {
      fkernel *k = &kern->kerns[nkerns];
      cxop *op = &kern->ops[nops];

      int new_kern = 0;
      int new_op = 0;

      if (STRNEQ (p, "kern", 4)) {
	float scale;

	n = sscanf (p, "%*s%d%d%d%d%f",
		    &k->width, &k->height, &k->origin_x, &k->origin_y, &scale);
	if (n < 4) {
	  errmsg = "Bad kernel description";
	  goto bad_line;
	} else if (n != 5)
	  scale = 1;
	
	{
	  int klen = k->width * k->height;
	  float *kp = newv (float, klen), *kend = kp+klen;
	  k->data = kp;
	  
	  eof = get_line (&buf, &buf_len, f);
	  ++lineno;
	  p = buf;
	  for ( ; kp < kend; ++kp) {
	    int nchars;
	    while (sscanf (p, "%f%n", kp, &nchars) < 1) {
	      eof = get_line (&buf, &buf_len, f);
	      p = buf;
	      ++lineno;
	    }
	    kp[0] /= scale;
	    p += nchars;
	  }
	}
	++new_kern;

      } else if (STRNEQ (p, "conv", 3)) {
	op->id = CXconvolve;
	n = sscanf (p, "%*s%d", &op->cx.iarg);
	if (n < 1) {
	  errmsg = "Bad `and' line";
	  goto bad_line;
	}
	++new_op;

      } else if (STRNEQ (p, "and", 3)) {
	op->id = CXand;
	n = sscanf (p, "%*s%d", &op->cx.iarg);
	if (n < 1) {
	  errmsg = "Bad `and' line";
	  goto bad_line;
	}
	++new_op;
	  
      } else if (STRNEQ (p, "or", 2)) {
	op->id = CXor;
	n = sscanf (p, "%*s%d", &op->cx.iarg);
	if (n < 1) {
	  errmsg = "Bad `and' line";
	  goto bad_line;
	}
	++new_op;

      } else if (STRNEQ (p, "scale", 5)) {
	op->id = CXscale;
	n = sscanf (p, "%*s%f", &op->cx.farg);
	if (n < 1) {
	  errmsg = "Bad `scale' line";
	  goto bad_line;
	}
	++new_op;

      } else {
	errmsg = "Unrecognized keyword";
	goto bad_line;
      }

      if (new_op && ++nops >= ops_len) {
	ops_len *= 2;
	kern->ops = renewv (cxop, kern->ops, ops_len);
      }

      if (new_kern && ++nkerns >= kerns_len) {
	kerns_len *= 2;
	kern->kerns = renewv (fkernel, kern->kerns, kerns_len);
      }

      /* Ignore except for goto's. */
      if (0) {
	bad_line:
	fprintf (stderr, "%s line %d: %s.\n", kern_file, lineno, errmsg);
	fclose (f);
	return 0;
      }
    }
  } while (! eof);

  
  kern->nops = nops;
  kern->ops = renewv (cxop, kern->ops, nops);

  kern->nkerns = nkerns;
  kern->kerns = renewv (fkernel, kern->kerns, nkerns);

  return kern;
}
