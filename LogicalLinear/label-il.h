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

#include <stdlib.h>

#include "alloc.h"

/*
 * Add a single label (as described) to the list.  Automatically extends as
 * necessary. 
 */

inline void
DEFUN (add_label, (lst, x, y, l, conf),
       label_list *lst AND
       unsigned short x AND unsigned short y AND
       unsigned short l AND
       unsigned short conf)
{
  int i = lst->nlabels;
  label *lp;

  if (! conf) return;

  if (++lst->nlabels > lst->labels_len) {
    if (lst->labels) {
      lst->labels_len *= 2;
      lst->labels = renewv (label, lst->labels, lst->labels_len);
    } else {
      lst->labels_len = 64;
      lst->labels = newv (label, lst->labels_len);
    }
  }

  lp = &lst->labels[i];
  lp->x = x;
  lp->y = y;
  lp->label = l;
  lp->conf = conf;
}

/*
 * Shrink wrap the label list.
 */

inline void
DEFUN (resize_label_list, (lst),
       label_list *lst)
{
  if (lst->labels_len > lst->nlabels) {
    lst->labels_len = lst->nlabels;
    lst->labels = renewv (label, lst->labels, lst->labels_len);
  }
}

/*
 * Free storage associated with label_list <lst>.
 */

inline void
DEFUN (free_label_list, (lst),
       label_list *lst)
{
  free (lst->labels);
  free (lst);
}

/*
 * Write a single label.
 */

inline void
DEFUN (write_label, (f, x, y, l, conf, type),
       FILE *f AND
       unsigned short x AND unsigned short y AND
       unsigned short l AND unsigned short conf AND
       char const *type)
{
  if (conf > 0)
    fprintf (f, "%hu %hu %hu %hu %s\n", x, y, l, conf, type);
}

