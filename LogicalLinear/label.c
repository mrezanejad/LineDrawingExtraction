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

#ifdef HAVE_CONFIG_H
#include "llconfig.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "label.h"
#include "image.h"

/* Provide externed definitions of inlined functions. */
#define inline
#include "label-il.h"
#undef inline

label_list *
DEFUN (collect_labels, (im, range, thresh, lbl),
       image_float *im AND
       float range AND
       float thresh AND
       unsigned short lbl)
{
  label_list *lst = newc (label_list);

  unsigned int w = im->width, h = im->height, len = w*h;
  float *ip = im->data, *ipend = ip+len;
  unsigned short y = 0;

  thresh *= range;

  for ( ; ip < ipend; ip += w, ++y) {
    float *ip2 = ip, *ip2end = ip2+w;
    for ( ; ip2 < ip2end; ++ip2) {
      float v = ip2[0];
      if (v >= thresh) {
	unsigned short conf = rint (v / range * CONF_MAX);
	if (conf > CONF_MAX) abort ();
	add_label (lst, ip2-ip, y, lbl, conf);
      }
    }
  }

  return lst;
}


static char const *label_head =
"% <ndirs> initcoords \n\
/initcoords { \n\
 % ndirs \n\
 2 mul \n\
 % 2*ndirs \n\
 dup array /x0 exch def \n\
 dup array /y0 exch def \n\
 dup array /x1 exch def \n\
 dup array /y1 exch def \n\
 % 2*ndirs \n\
 dup 360 exch div \n\
 % 2*ndirs 180/ndirs \n\
 0 1 3 index 1 sub { \n\
  % 2*ndirs 180/ndirs i \n\
  dup 2 index mul \n\
  % 2*ndirs 180/ndirs i i*180/ndirs \n\
  dup cos 0.75 mul \n\
  dup 0.5 add x0 4 index 3 -1 roll put \n\
  neg 0.5 add x1 3 index 3 -1 roll put \n\
  % 2*ndirs 180/ndirs i i*180/ndirs \n\
  sin 0.75 mul \n\
  dup 0.5 add y0 3 index 3 -1 roll put \n\
  neg 0.5 add y1 2 index 3 -1 roll put \n\
  % 2*ndirs 180/ndirs i \n\
  pop \n\
 } for \n\
 % 2*ndirs 180/ndirs \n\
 pop pop \n\
} bind def \n\
 \n\
% <x> <y> <dir> coords \n\
/coords { \n\
 % x y dir \n\
 3 copy \n\
 % x y dir x y dir \n\
 dup y0 exch get exch x0 exch get  \n\
 % x y dir x y y0 x0 \n\
 4 -1 roll add  \n\
 % x y dir y y0 x+x0 \n\
 6 1 roll add 4 1 roll \n\
 % x+x0 y+y0 x y dir \n\
 dup y1 exch get exch x1 exch get \n\
 % x+x0 y+y0 x y y1 x1 \n\
 4 -1 roll add \n\
 % x+x0 y+y0 y y1 x+x1 \n\
 3 1 roll add \n\
 % x+x0 y+y0 x+x1 y+y1 \n\
} bind def \n\
 \n\
% <x> <y> <dir> <conf> edge \n\
/edge { \n\
 0.3 setlinewidth \n\
 edgeconf coords \n\
 % x+x0 y+y0 x+x1 y+y1 \n\
 moveto lineto stroke \n\
} bind def \n\
 \n\
% <x> <y> <dir> <conf> plin \n\
/plin { \n\
 0.2 setlinewidth \n\
 plinconf coords \n\
 % x+x0 y+y0 x+x1 y+y1 \n\
 moveto lineto stroke \n\
} bind def \n\
 \n\
% <x> <y> <dir> <conf> nlin \n\
/nlin { \n\
 0.2 setlinewidth \n\
 nlinconf coords \n\
 % x+x0 y+y0 x+x1 y+y1 \n\
 moveto lineto stroke \n\
} bind def \n\
 \n\
% <conf> setconf \n\
/b0conf { -510 div 0.5   add setgray} bind def \n\
/b1conf {-1020 div 0.25  add setgray} bind def \n\
/w1conf { 1020 div 0.75  add setgray} bind def \n\
/g1conf {-1020 div 0.375 add setgray} bind def \n\
 \n\
";

void
DEFUN (write_label_head, (f, desc, w, h, ndirs, do_edges, do_lines, do_dark),
       FILE *f AND
       char const *desc AND
       unsigned int w AND unsigned int h AND unsigned int ndirs AND
       int do_edges AND int do_lines AND int do_dark)
{
  int grey_bg = (do_edges+do_lines+do_dark > 1);

  fprintf (f, "%%!PS-Adobe-2.0 EPSF-2.0\n");
  fprintf (f, "%%%%BoundingBox: 0 0 %u %u\n", w, h);
  fprintf (f, "%%%%Pages: 1\n%%%%EndProlog\n%%%%Page: 1 1\n");
  fprintf (f, "gsave [1 0 0 -1 0 %u] concat\n", h);
  fprintf (f, "512 dict begin\n");
  fprintf (f, "%s\n", label_head);
  fprintf (f, "%u initcoords\n", ndirs);

  if (do_edges)
    fprintf (f, "/edgeconf /%sconf load def\n", 
	     grey_bg ? "b1" : "b0");
  if (do_lines)
    fprintf (f, "/plinconf /%sconf load def\n",
	     grey_bg ? "w1" : "b0");
  if (do_dark)
    fprintf (f, "/nlinconf /%sconf load def\n",
	     grey_bg ? (do_edges ? "g1" : "b1") : "b0");

  if (do_edges+do_lines+do_dark > 1) 
    fprintf (f, "0.5 setgray 0 0 moveto 0 %u lineto %u %u lineto %u 0 lineto closepath fill\n",
	     h, w, h, w);

  fprintf (f, "%%labels:\n");
}

void
DEFUN (write_label_tail, (f),
       FILE *f)
{
  fprintf (f, "end grestore showpage\n");
  fprintf (f, "%%%%Trailer\n");
}

int
DEFUN (write_labels, (f, lst, type),
       FILE *f AND
       label_list *lst AND
       char const *type)
{
  label *lp, *lpend;
  int len = lst->nlabels;

  for (lp = lst->labels, lpend = lp+len; lp < lpend; ++lp) {
    if (lp->conf > CONF_MAX) abort ();
    fprintf (f, "%hu %hu %hu %hu %.4s\n",
	     lp->x, lp->y, lp->label, lp->conf, type);
  }

  return 1;
}

label_list *
DEFUN (read_labels, (f, fname, np, typep),
       FILE *f AND
       char const *fname AND
       int *np AND
       char ***typep)
{
  label_list *lst = newc (label_list);
  char **type = newc (char *);
  int n = 0;
  char line[512];

  while (fgets (line, 512, f)) 
    if (! strcmp (line, "%%labels:\n"))
      break;

  while (fgets (line, 512, f)) {
    unsigned short x, y, l, conf;
    char ltype[5];
    if (sscanf (line, "%hu %hu %hu %hu %4s", &x, &y, &l, &conf, ltype) == 5) {
      int i;
      for (i = 0; i < n; ++i) 
	if (! strncmp (ltype, type[i], 4)) break;
      if (i == n) {
	++n;
	lst = renewv (label_list, lst, n);
	lst[i].nlabels = lst[i].labels_len = 0;
	lst[i].labels = 0;
	type = renewv (char *, type, n);
	type[i] = newv (char, 5);
	strncpy (type[i], ltype, 4);
      }

      add_label (&lst[i], x, y, l, conf);
    }
  }

  if (np) *np = n;
  if (typep) *typep = type;

  return lst;
}
