#include "conv.h"
#include "basis.h"

#include <stdio.h>
#include <limits.h>

int
main ()
{
  cxkernel *line0 =
    make_line2_cxkern (0 * M_PI/8, 2.5, 4, 2.0, 0.2, 1.4, 0.7);

  cxkernel *line1 =
    make_line2_cxkern (1 * M_PI/8, 2.5, 4, 2.0, 0.2, 1.4, 0.7);

  cxkernel *line2 =
    make_line2_cxkern (2 * M_PI/8, 2.5, 4, 2.0, 0.2, 1.4, 0.7);

  cxkernel *edge0 =
    make_edge2_cxkern (0 * M_PI/8, 2.5, 4, 2.0, 0.2, 2.0, 0.7);

  cxkernel *edge1 =
    make_edge2_cxkern (1 * M_PI/8, 2.5, 4, 2.0, 0.2, 2.0, 0.7);

  cxkernel *edge2 =
    make_edge2_cxkern (2 * M_PI/8, 2.5, 4, 2.0, 0.2, 2.0, 0.7);

  FILE *f;

  f = fopen ("line0.kern", "w");
  write_kern (f, line0);
  fclose (f);

  f = fopen ("line1.kern", "w");
  write_kern (f, line1);
  fclose (f);

  f = fopen ("line2.kern", "w");
  write_kern (f, line2);
  fclose (f);

  f = fopen ("edge0.kern", "w");
  write_kern (f, edge0);
  fclose (f);

  f = fopen ("edge1.kern", "w");
  write_kern (f, edge1);
  fclose (f);

  f = fopen ("edge2.kern", "w");
  write_kern (f, edge2);
  fclose (f);

  return 0;
}
