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

#ifndef _label_h_
#define _label_h_

#include <ansidecl.h>
#include <sys/types.h>

#include "image.h"

#define CONF_MAX (0xff)

typedef struct {
  unsigned short x, y;
  unsigned short label;
  unsigned short conf;
} label;

typedef struct {
  int nlabels;
  int labels_len;
  label *labels;
} label_list;

label_list *
collect_labels PARAMS((image_float *im,
		       float range, float thresh, unsigned short lbl));

void
add_label PARAMS((label_list *lst, 
		  unsigned short x, unsigned short y, 
		  unsigned short l, unsigned short conf));

void
resize_label_list PARAMS((label_list *lst));

void
write_label_head PARAMS((FILE *f, char const *desc, 
			 unsigned int w, unsigned int h, unsigned int ndirs,
			 int do_edges, int do_lines, int do_dark));

void
write_label_tail PARAMS((FILE *f));

void
write_label PARAMS((FILE *f, unsigned short x, unsigned short y,
		    unsigned short l, unsigned short conf,
		    char const *type));

int
write_labels PARAMS((FILE *f, label_list *lst, char const *type));

label_list *
read_labels PARAMS((FILE *f, char const *label_file, int *np, char ***typep));

void
free_label_list PARAMS((label_list *lst));

#if (defined(__GNUC__) && defined(__OPTIMIZE__))
# define inline extern __inline__
# include "label-il.h"
# undef inline
#endif

#endif /* _label_h_ */
