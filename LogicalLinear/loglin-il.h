
inline float
DEFUN (abs_partition, (x),
       float x)
{
  return (x > 0 ? 1 : 0);
}

inline float
DEFUN (contin_partition, (x, degree),
       float x AND float degree)
{
  x += 1 / (2*degree);
  return (x < 0         ? 0 :
	  x > (1/degree) ? 1 :
	  x * degree);
}

inline float
DEFUN (partition, (x, degree),
       float x AND float degree)
{
  return (degree < 0
	  ? abs_partition (x)
	  : contin_partition (x, degree));
}

inline float
DEFUN (abs_lland2, (x, y),
       float x AND float y)
{
  return (x > 0
	  ? (y > 0 ? x + y :     y)
	  : (y > 0 ? x     : x + y));
}

inline float
DEFUN (contin_lland2, (degree, x, y),
       float degree AND
       float x AND float y)
{
  float sx = contin_partition (x, degree);
  float sy = contin_partition (y, degree);

  return (x * (1 - sx*(1-sy)) +
	  y * (1 - sy*(1-sx)));
}

inline float
DEFUN (lland2, (degree, x0, x1),
       float degree AND
       float x0 AND float x1)
{
  return (degree <= 0
	  ? abs_lland2 (x0, x1)
	  : contin_lland2 (degree, x0, x1));
}

inline float
DEFUN (abs_lland3, (x0, x1, x2),
       float x0 AND float x1 AND float x2)
{
  float xn;

  xn = abs_lland2 (x0, x1);
  xn = abs_lland2 (xn, x2);

  return xn;
}

inline float
DEFUN (contin_lland3, (degree, x0, x1, x2),
       float degree AND
       float x0 AND float x1 AND float x2)
{
  float s0, s1, s2;
  float prod = 1;

  prod *= (s0 = contin_partition (x0, degree));
  prod *= (s1 = contin_partition (x1, degree));
  prod *= (s2 = contin_partition (x2, degree));

  {
    float sum = 0;
    sum += x0 * (prod + (1-s0));
    sum += x1 * (prod + (1-s1));
    sum += x2 * (prod + (1-s2));

    return sum;
  }
}

inline float
DEFUN (lland3, (degree, x0, x1, x2),
       float degree AND float x0 AND float x1 AND float x2)
{
  return (degree <= 0
	  ? abs_lland3 (x0, x1, x2)
	  : contin_lland3 (degree, x0, x1, x2));
}

inline float
DEFUN (abs_lland4, (x0, x1, x2, x3),
       float x0 AND float x1 AND float x2 AND float x3)
{
  float xn;

  xn = abs_lland2 (x0, x1);
  xn = abs_lland2 (xn, x2);
  xn = abs_lland2 (xn, x3);

  return xn;
}

inline float
DEFUN (contin_lland4, (degree, x0, x1, x2, x3),
       float degree AND
       float x0 AND float x1 AND float x2 AND float x3)
{
  float s0, s1, s2, s3;
  float prod = 1;

  prod *= (s0 = contin_partition (x0, degree));
  prod *= (s1 = contin_partition (x1, degree));
  prod *= (s2 = contin_partition (x2, degree));
  prod *= (s3 = contin_partition (x3, degree));

  {
    float sum = 0;
    sum += x0 * (prod + (1-s0));
    sum += x1 * (prod + (1-s1));
    sum += x2 * (prod + (1-s2));
    sum += x3 * (prod + (1-s3));

    return sum;
  }
}

inline float
DEFUN (lland4, (degree, x0, x1, x2, x3),
       float degree AND
       float x0 AND float x1 AND float x2 AND float x3)
{
  return (degree <= 0
	  ? abs_lland4 (x0, x1, x2, x3)
	  : contin_lland4 (degree, x0, x1, x2, x3));
}

inline float
DEFUN (abs_lland5, (x0, x1, x2, x3, x4),
       float x0 AND float x1 AND float x2 AND float x3 AND float x4)
{
  float xn;

  xn = abs_lland2 (x0, x1);
  xn = abs_lland2 (xn, x2);
  xn = abs_lland2 (xn, x3);
  xn = abs_lland2 (xn, x4);

  return xn;
}

inline float
DEFUN (contin_lland5, (degree, x0, x1, x2, x3, x4),
       float degree AND
       float x0 AND float x1 AND float x2 AND float x3 AND float x4)
{
  float s0, s1, s2, s3, s4;
  float prod = 1;

  prod *= (s0 = contin_partition (x0, degree));
  prod *= (s1 = contin_partition (x1, degree));
  prod *= (s2 = contin_partition (x2, degree));
  prod *= (s3 = contin_partition (x3, degree));
  prod *= (s4 = contin_partition (x4, degree));

  {
    float sum = 0;
    sum += x0 * (prod + (1-s0));
    sum += x1 * (prod + (1-s1));
    sum += x2 * (prod + (1-s2));
    sum += x3 * (prod + (1-s3));
    sum += x4 * (prod + (1-s4));

    return sum;
  }
}

inline float
DEFUN (lland5, (degree, x0, x1, x2, x3, x4),
       float degree AND
       float x0 AND float x1 AND float x2 AND float x3 AND float x4)
{
  return (degree <= 0
	  ? abs_lland5 (x0, x1, x2, x3, x4)
	  : contin_lland5 (degree, x0, x1, x2, x3, x4));
}

inline float
DEFUN (lland6, (degree,	x0, x1, x2, x3, x4, x5),
       float degree AND
       float x0 AND float x1 AND float x2 AND
       float x3 AND float x4 AND float x5)
{
  return (degree <= 0
	  ? abs_lland6 (x0, x1, x2, x3, x4, x5)
	  : contin_lland6 (degree, x0, x1, x2, x3, x4, x5));
}

inline float
DEFUN (lland_n, (degree, args, n),
       float degree AND
       float const *args AND
       int n)
{
  return (degree <= 0
	  ? abs_lland_n (args, n)
	  : contin_lland_n (degree, args, n));
}

inline float
DEFUN (abs_llor2, (x, y),
       float x AND float y)
{
  return (x > 0
	  ? (y > 0 ? x + y : x    )
	  : (y > 0 ?     y : x + y));
}

inline float
DEFUN (contin_llor2, (degree, x, y),
       float degree AND
       float x AND float y)
{
  float sx = contin_partition (x, degree);
  float sy = contin_partition (y, degree);

  return (x * (1 - sy*(1-sx)) 
	  + y * (1 - sx*(1-sy)));
}

inline float
DEFUN (llor2, (degree, x0, x1),
       float degree AND
       float x0 AND float x1)
{
  return (degree <= 0
	  ? abs_llor2 (x0, x1)
	  : contin_llor2 (degree, x0, x1));
}

inline float
DEFUN (abs_llor3, (x0, x1, x2),
       float x0 AND float x1 AND float x2)
{
  float xn;

  xn = abs_llor2 (x0, x1);
  xn = abs_llor2 (xn, x2);

  return xn;
}

inline float
DEFUN (contin_llor3, (degree, x0, x1, x2),
       float degree AND
       float x0 AND float x1 AND float x2)
{
  float s0, s1, s2;
  float prod = 1;

  prod *= (s0 = contin_partition (-x0, degree));
  prod *= (s1 = contin_partition (-x1, degree));
  prod *= (s2 = contin_partition (-x2, degree));

  {
    float sum = 0;
    sum += x0 * (prod + (1-s0));
    sum += x1 * (prod + (1-s1));
    sum += x2 * (prod + (1-s2));

    return sum;
  }
}

inline float
DEFUN (llor3, (degree, x0, x1, x2),
       float degree AND
       float x0 AND float x1 AND float x2)
{
  return (degree <= 0
	  ? abs_llor3 (x0, x1, x2)
	  : contin_llor3 (degree, x0, x1, x2));
}

inline float
DEFUN (abs_llor4, (x0, x1, x2, x3),
       float x0 AND float x1 AND float x2 AND float x3)
{
  float xn;

  xn = abs_llor2 (x0, x1);
  xn = abs_llor2 (xn, x2);
  xn = abs_llor2 (xn, x3);

  return xn;
}

inline float
DEFUN (contin_llor4, (degree, x0, x1, x2, x3),
       float degree AND
       float x0 AND float x1 AND float x2 AND float x3)
{
  float s0, s1, s2, s3;
  float prod = 1;

  prod *= (s0 = contin_partition (-x0, degree));
  prod *= (s1 = contin_partition (-x1, degree));
  prod *= (s2 = contin_partition (-x2, degree));
  prod *= (s3 = contin_partition (-x3, degree));

  {
    float sum = 0;
    sum += x0 * (prod + (1-s0));
    sum += x1 * (prod + (1-s1));
    sum += x2 * (prod + (1-s2));
    sum += x3 * (prod + (1-s3));

    return sum;
  }
}

inline float
DEFUN (llor4, (degree, x0, x1, x2, x3),
       float degree AND
       float x0 AND float x1 AND float x2 AND float x3)
{
  return (degree <= 0
	  ? abs_llor4 (x0, x1, x2, x3)
	  : contin_llor4 (degree, x0, x1, x2, x3));
}

inline float
DEFUN (abs_llor5, (x0, x1, x2, x3, x4),
       float x0 AND float x1 AND float x2 AND float x3 AND float x4)
{
  float xn;

  xn = abs_llor2 (x0, x1);
  xn = abs_llor2 (xn, x2);
  xn = abs_llor2 (xn, x3);
  xn = abs_llor2 (xn, x4);

  return xn;
}

inline float
DEFUN (contin_llor5, (degree, x0, x1, x2, x3, x4),
       float degree AND
       float x0 AND float x1 AND float x2 AND float x3 AND float x4)
{
  float s0, s1, s2, s3, s4;
  float prod = 1;

  prod *= (s0 = contin_partition (-x0, degree));
  prod *= (s1 = contin_partition (-x1, degree));
  prod *= (s2 = contin_partition (-x2, degree));
  prod *= (s3 = contin_partition (-x3, degree));
  prod *= (s4 = contin_partition (-x4, degree));

  {
    float sum = 0;
    sum += x0 * (prod + (1-s0));
    sum += x1 * (prod + (1-s1));
    sum += x2 * (prod + (1-s2));
    sum += x3 * (prod + (1-s3));
    sum += x4 * (prod + (1-s4));

    return sum;
  }
}

inline float
DEFUN (llor5, (degree, x0, x1, x2, x3, x4),
       float degree AND
       float x0 AND float x1 AND float x2 AND float x3 AND float x4)
{
  return (degree <= 0
	  ? abs_llor5 (x0, x1, x2, x3, x4)
	  : contin_llor5 (degree, x0, x1, x2, x3, x4));
}

inline float
DEFUN (llor6, (degree, x0, x1, x2, x3, x4, x5),
       float degree AND
       float x0 AND float x1 AND float x2 AND float x3 AND float x4 AND float x5)
{
  return (degree <= 0
	  ? abs_llor6 (x0, x1, x2, x3, x4, x5)
	  : contin_llor6 (degree, x0, x1, x2, x3, x4, x5));
}

inline float
DEFUN (llor_n, (degree, args, n),
       float degree AND
       float const *args AND
       int n)
{
  return (degree <= 0
	  ? abs_llor_n (args, n)
	  : contin_llor_n (degree, args, n));
}

