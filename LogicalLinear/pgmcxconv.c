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

#include <string.h>
#include <stdio.h>

static char *argv0;
static char usage[] = 
		"Usage: %s [-d<degree>] [-t<threshold>] kern [pgmin] [pgmout]\n";

int
main (argc, argv)
int argc;
char **argv;
{
	FILE *inf = stdin;
	char *in = 0, *out = 0, *outc = 0;
	image_gray8 *im_in = 0;
	image_float *im_out = 0, *im_outc = 0;
	unsigned int width, height;

	char *kern_file = 0;
	cxkernel *kern = 0;

	float degree = 32;
	float thresh = 0;

	argv0 = argv[0];

	for (--argc, ++argv; argc; --argc, ++argv) {
		if (**argv == '-') {
			while (*++*argv) {
				switch (**argv) {
				case 'd':
					degree = atof ( *(*argv+1)? ++*argv: (--argc, *++argv) );
					goto next_arg;

				case 't':
					thresh = atof ( *(*argv+1)? ++*argv: (--argc, *++argv) );
					goto next_arg;

				default:
					fprintf (stderr, usage, argv0);
					exit (1);
				}
			}
			next_arg:;

		} else if (! kern) {
			FILE *f;
			kern_file = *argv;

			if (! (f = fopen (kern_file, "r"))) {
				fprintf (stderr, "%s: Cannot open kernel %s\n", argv0, kern_file);
				exit (1);
			}
			if (! (kern = read_kern (f, kern_file))) {
				exit (1);
			}
			fclose (f);

		} else if (! in) {
			if (! (inf = fopen (in = *argv, "r"))) {
				fprintf (stderr, "%s: Cannot open image %s\n", argv0, in);
				exit (1);
			}
		} else if (! out) {
			out = *argv;
		}
	}

	/* Read PGM file from input. */
	if (! in) in = "<stdin>";
	im_in = read_pgm (inf);
	fclose (inf);

	if (! im_in) {
		fprintf (stderr, "%s: %s not an image.\n", argv0, in);
		exit (1);
	}

	/* Get image size. */
	width  = image_width (im_in);
	height = image_height (im_in);

	if (out) {
		char buf[80];
		sprintf (buf, "%s~", out);
		outc = strdup (buf);
	}

	im_out = new_image_float (width, height);
	if (outc) im_outc = new_image_float (width, height);

	im_out->min = -im_in->max;
	im_out->max = im_in->max;
	if (im_outc) {
		im_outc->min = im_out->min;
		im_outc->max = im_out->max;
	}
	degree /= im_in->max;
	cxconvolve_gray8 (im_in, degree, kern, im_out, im_outc, 0, 0);

	if (thresh > 0) {
		unsigned int len = width*height;
		float *op = im_out->data, *opend = op+len;
		for (; op < opend; ++op)
			if (op[0] < thresh) op[0] = 0;
		im_out->min = 0;

		op = im_outc->data; opend = op+len;
		for (; op < opend; ++op)
			if (op[0] < thresh) op[0] = 0;
		im_outc->min = 0;
	}

	{
		int len = width*height;
		FILE *f = out? fopen (out, "w") : stdout;
		image_gray8 *pgm = new_image_gray8 (width, height);
		float const *op = im_out->data, *opend = op+len, omax = im_out->max;
		gray8 *gp;

		if (! out) out = "<stdout>";
		pgm->max = 255;
		for (gp = pgm->data; op < opend; ++op, ++gp)
			gp[0] = (op[0] > 0 ? rint (255 * op[0]/omax) : 0);
		write_pgm (f, pgm);
		fclose (f);

		if (outc) {
			float const omax = im_outc->max;
			f = fopen (outc, "w");
			op = im_outc->data; opend = op+len;
			pgm->max = 255;
			for (gp = pgm->data; op < opend; ++op, ++gp)
				gp[0] = (op[0] > 0 ? rint (255 * op[0]/omax) : 0);
			write_pgm (f, pgm);
			fclose (f);
		}
	}

	return 0;
}

