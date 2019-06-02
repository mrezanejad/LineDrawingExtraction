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

inline double 
DEFUN (G, (x, s), double x AND double s)
{
  return exp (-(x*x)/(2*s*s))*M_2_SQRTPI/s;
}

inline double 
DEFUN (dG, (x, s), double x AND double s)
{
  return -x*G(x,s)/(s*s);
}

inline double 
DEFUN (d2G, (x, s), double x AND double s)
{
  double s2 = s*s, s4 = s2*s2;
  return (x*x-s2)*G(x,s)/s4;
}

inline double
DEFUN (d3G, (x, s), double x AND double s)
{
  double s2 = s*s, s4 = s2*s2, s6 = s4*s2;
  return -x*(x*x-3*s2)*G(x,s)/(s6);
}

inline double
DEFUN (d4G, (x, s), double x AND double s)
{
  double s2 = s*s, s4 = s2*s2, s8 = s4*s4;
  return (x*x*(x*x-6*s2)+3*s4)*G(x,s)/(s8);
}

inline double
DEFUN (Gabor, (x, s, period, phase), 
       double x AND double s AND double period AND double phase)
{
  return G(x,s) * sin (((2*M_PI*x)/period) - phase);
}

inline double 
DEFUN (HG, (x, s), double x AND double s)
{
  double t1, t3, t4, t6, t7, t9, t11, t20;
  x /= s;
  t1 = 1/0.3141592653589793E1;
  t3 = x*x;
  t4 = t3*x;
  t6 = t3*t3;
  t7 = t6*x;
  t9 = t6*t4;
  t11 = t6*t6;
  t20 = -t1*x-t1*t4/6-t1*t7/40-t1*t9/336-t1*t11*x/3456-t1*t11*t4/42240
    -t1*t11*t7/599040-t1*t11*t9/9676800;
  return t20*exp (-(x*x)/(2*s*s))/s;
}

inline double 
DEFUN (HdG, (x, s), double x AND double s)
{
  double t1, t2, t4, t6, t8, t16;
  x /= s;
  t1 = 1/0.3141592653589793E1;
  t2 = x*x;
  t4 = t2*t2;
  t6 = t4*t2;
  t8 = t4*t4;
  t16 = -t1+t1*t2/2+t1*t4/24+t1*t6/240+t1*t8/2688+t1*t8*t2/34560
    +t1*t8*t4/506880+t1*t8*t6/8386560;
  return t16*exp (-(x*x)/(2*s*s))/(s*s);
}

inline double 
DEFUN (Hd2G, (x, s), double x AND double s)
{
  double t1, t3, t4, t6, t7, t9, t11, t20;
  x /= s;
  t1 = 1/0.3141592653589793E1;
  t3 = x*x;
  t4 = t3*x;
  t6 = t3*t3;
  t7 = t6*x;
  t9 = t6*t4;
  t11 = t6*t6;
  t20 = 2.0*t1*x-t1*t4/3-t1*t7/60-t1*t9/840-t1*t11*x/12096-t1*t11*t4/190080
    -t1*t11*t7/3294720-t1*t11*t9/62899200;
  return t20*exp (-(x*x)/(2*s*s))/(s*s*s);
}

inline float 
DEFUN (Gf, (x, s), float x AND float s)
{
  return expf (-(x*x)/(2*s*s))*M_2_SQRTPI/s;
}

inline float 
DEFUN (dGf, (x, s), float x AND float s)
{
  return -x*Gf(x,s)/(s*s);
}

inline float 
DEFUN (d2Gf, (x, s), float x AND float s)
{
  float s2 = s*s, s4 = s2*s2;
  return (x*x-s2)*Gf(x,s)/s4;
}

inline float
DEFUN (d3Gf, (x, s), float x AND float s)
{
  float s2 = s*s, s4 = s2*s2, s6 = s4*s2;
  return -x*(x*x-3*s2)*Gf(x,s)/(s6);
}

inline float
DEFUN (d4Gf, (x, s), float x AND float s)
{
  float s2 = s*s, s4 = s2*s2, s8 = s4*s4;
  return (x*x*(x*x-6*s2)+3*s4)*Gf(x,s)/(s8);
}

inline float
DEFUN (Gaborf, (x, s, period, phase),
       float x AND float s AND float period AND float phase)
{
  return Gf(x,s) * sinf (((2*M_PI*x)/period) - phase);
}

inline float 
DEFUN (HGf, (x, s), float x AND float s)
{
  float t1, t3, t4, t6, t7, t9, t11, t20;
  x /= s;
  t1 = 1/0.3141592653589793E1;
  t3 = x*x;
  t4 = t3*x;
  t6 = t3*t3;
  t7 = t6*x;
  t9 = t6*t4;
  t11 = t6*t6;
  t20 = -t1*x-t1*t4/6-t1*t7/40-t1*t9/336-t1*t11*x/3456-t1*t11*t4/42240
    -t1*t11*t7/599040-t1*t11*t9/9676800;
  return t20*expf (-(x*x)/(2*s*s))/s;
}

inline float 
DEFUN (HdGf, (x, s), float x AND float s)
{
  float t1, t2, t4, t6, t8, t16;
  x /= s;
  t1 = 1/0.3141592653589793E1;
  t2 = x*x;
  t4 = t2*t2;
  t6 = t4*t2;
  t8 = t4*t4;
  t16 = -t1+t1*t2/2+t1*t4/24+t1*t6/240+t1*t8/2688+t1*t8*t2/34560
    +t1*t8*t4/506880+t1*t8*t6/8386560;
  return t16*expf (-(x*x)/(2*s*s))/(s*s);
}

inline float 
DEFUN (Hd2Gf, (x, s), float x AND float s)
{
  float t1, t3, t4, t6, t7, t9, t11, t20;
  x /= s;
  t1 = 1/0.3141592653589793E1;
  t3 = x*x;
  t4 = t3*x;
  t6 = t3*t3;
  t7 = t6*x;
  t9 = t6*t4;
  t11 = t6*t6;
  t20 = 2.0*t1*x-t1*t4/3-t1*t7/60-t1*t9/840-t1*t11*x/12096-t1*t11*t4/190080
    -t1*t11*t7/3294720-t1*t11*t9/62899200;
  return t20*expf (-(x*x)/(2*s*s))/(s*s*s);
}
