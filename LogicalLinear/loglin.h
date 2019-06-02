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

#ifndef _loglin_h_
#define _loglin_h_

#include "ansidecl.h"
#include "image.h"

float partition PARAMS((float x, float degree));
float abs_partition PARAMS((float x));
float contin_partition PARAMS((float x, float degree));
float smooth_partition PARAMS((float x, float degree));

float lland2 PARAMS((float degree, float x0, float x1));
float lland3 PARAMS((float degree, float x0, float x1, float x2));
float lland4 PARAMS((float degree, float x0, float x1, float x2, float x3));
float lland5 PARAMS((float degree, float x0, float x1, float x2, float x3,
		     float x4));
float lland6 PARAMS((float degree, float x0, float x1, float x2, float x3,
		     float x4, float x5));
float lland_n PARAMS((float degree, float const *args, int n));
float lland_np PARAMS((float degree, float const **args, int n));

float abs_lland2 PARAMS((float x0, float x1));
float abs_lland3 PARAMS((float x0, float x1, float x2));
float abs_lland4 PARAMS((float x0, float x1, float x2, float x3));
float abs_lland5 PARAMS((float x0, float x1, float x2, float x3,
			 float x4));
float abs_lland6 PARAMS((float x0, float x1, float x2, float x3,
			 float x4, float x5));
float abs_lland_n PARAMS((float const *args, int n));
float abs_lland_np PARAMS((float const **args, int n));

float contin_lland2 PARAMS((float degree, float x0, float x1));
float contin_lland3 PARAMS((float degree, float x0, float x1, float x2));
float contin_lland4 PARAMS((float degree, float x0, float x1, float x2, 
			    float x3));
float contin_lland5 PARAMS((float degree, float x0, float x1, float x2,
			    float x3, float x4));
float contin_lland6 PARAMS((float degree, float x0, float x1, float x2,
			    float x3, float x4, float x5));
float contin_lland_n PARAMS((float degree, float const *args, int n));
float contin_lland_np PARAMS((float degree, float const **args, int n));

float llor2 PARAMS((float degree, float x0, float x1));
float llor3 PARAMS((float degree, float x0, float x1, float x2));
float llor4 PARAMS((float degree, float x0, float x1, float x2, float x3));
float llor5 PARAMS((float degree, float x0, float x1, float x2, float x3,
		    float x4));
float llor6 PARAMS((float degree, float x0, float x1, float x2, float x3,
		    float x4, float x5));
float llor_n PARAMS((float degree, float const *args, int n));
float llor_np PARAMS((float degree, float const **args, int n));

float abs_llor2 PARAMS((float x0, float x1));
float abs_llor3 PARAMS((float x0, float x1, float x2));
float abs_llor4 PARAMS((float x0, float x1, float x2, float x3));
float abs_llor5 PARAMS((float x0, float x1, float x2, float x3,
			float x4));
float abs_llor6 PARAMS((float x0, float x1, float x2, float x3,
			float x4, float x5));
float abs_llor_n PARAMS((float const *args, int n));
float abs_llor_np PARAMS((float const **args, int n));

float contin_llor2 PARAMS((float degree, float x0, float x1));
float contin_llor3 PARAMS((float degree, float x0, float x1, float x2));
float contin_llor4 PARAMS((float degree, float x0, float x1, float x2,
			   float x3));
float contin_llor5 PARAMS((float degree, float x0, float x1, float x2,
			   float x3, float x4));
float contin_llor6 PARAMS((float degree, float x0, float x1, float x2,
			   float x3, float x4, float x5));
float contin_llor_n PARAMS((float degree, float const *args, int n));
float contin_llor_np PARAMS((float degree, float const **args, int n));

extern image_float *
llor_n_float PARAMS((float degree, image_float **ims, int n,
		     image_float *out));

#if (defined(__GNUC__) && defined(__OPTIMIZE__))
# define inline extern __inline__
# include "loglin-il.h"
# undef inline
#endif

#endif /* _loglin_h_ */
