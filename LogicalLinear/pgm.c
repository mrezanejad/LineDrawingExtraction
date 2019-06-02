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

#include <stdio.h>
#include <ctype.h>

#include "image.h"
#include "alloc.h"

static void
DEFUN (skip_whitespace, (in),
       FILE *in)
{
  int c;

  while ((c = getc(in)) != EOF) {
    if (c == '#') {
      while ((c = getc(in)) != EOF)
	if (c == '\n') break;
    } else if (! isspace (c)) {
      ungetc (c, in);
      break;
    }
  }
}

static int
DEFUN (freadgray, (fp, vp),
       FILE *fp AND
       gray8 *vp)
{
  int c;
  int accum = 0;
  int valid = 0;

  /* Skip whitespace. */
  skip_whitespace (fp);

  /* Accumulate val. */
  for ( ; c != EOF && isdigit(c); c = getc(fp) ) {
    valid = 1;
    accum = accum*10 + (c - '0');
  }

  if ( valid ) *vp = accum;

  return valid;
}

image_gray8 *
DEFUN (read_pgm, (file),
       FILE *file)
{
  register short i;
  register gray8 *gp;
  int format;
  int w, h, maxval;
  char fstr[10];
  image_gray8 *pgm;

  /* Read two initial bytes. */
  if (fread (fstr, 1, 2, file) != 2)
    return 0;
  fstr[2] = '\0';

  if ( fstr[0] != 'P' || ! isdigit (fstr[1]) ) return 0;

  skip_whitespace (file);
  if ( fscanf(file, "%d", &w) != 1 )
    return 0;

  skip_whitespace (file);
  if ( fscanf(file, "%d", &h) != 1 )
    return 0;

  switch (fstr[1]) {

  case '1': /* PBM */
    format = 1;
    maxval = 1;
    break;

  case '2': /* PGM */
    format = 1;
    skip_whitespace (file);
    if ( fscanf(file, "%d", &maxval) != 1 ) return 0;
    break;

  case '4': /* PBM raw */
    format = 2; 
    maxval = 1; 
    break;

  case '5': /* PGM raw */
    format = 3;
    skip_whitespace (file);
    if ( fscanf(file, "%d", &maxval) != 1 ) return 0;
    break;

  default:
    fprintf(stderr, "Not a PGM file.\n");
    return 0;
  }

  pgm = new_image_gray8 (w, h);
  pgm->max = maxval;

  switch ( format ) {

  case 1: /* Digits as strings. */
  {
    skip_whitespace (file);
    for ( i = 0, gp = pgm->data; i < h; ++i, gp += w ) {
      int j; gray8 *p;
      for (j = 0, p = gp; j < w; ++j, ++p )
	freadgray (file, p);
    }
    break;
  }

  case 2: /* PBM raw */
  {
    int scanw = (w+7)/8;
    gray8 *buf = anewv (gray8, scanw);
    fgetc (file); /* Skip single char */
    for (i = 0, gp = pgm->data; i < h; ++i, gp += w ) {
      int j, mask = 0x80;
      gray8 *p, *q = buf;
      fread (buf, 1, scanw, file);
      for ( j = 0, p = gp; j < w; ++j, ++p, mask >>= 1 ) {
	if ( mask == 0 ) mask = 0x80, ++q;
	*p = !(*q&mask);
      }
    }
    break;
  }

  case 3: /* PGM raw */
    fgetc (file); /* Skip single char */
    fread (pgm->data, 1, w*h, file);
    break;
  }

  return pgm;
}

void
DEFUN (write_pgm, (file, pgm),
       FILE *file AND
       register image_gray8 *pgm)
{
  fprintf (file, "P5\n%d %d %d\n", pgm->width, pgm->height, pgm->max);
  fwrite (pgm->data, 1, pgm->width*pgm->height, file);
}
