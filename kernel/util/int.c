/*
 * Copyright (c) 2002, 2012 Jens Keiner, Stefan Kunis, Daniel Potts
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/* $Id: util.c 3483 2010-04-23 19:02:34Z keiner $ */

#include "infft.h"

INT X(exp2i)(const INT a)
{
  return (1U << a);
}

INT X(log2i)(const INT m)
{
  INT l = 0;
  INT mm = m;

  while (mm > 0)
  {
    mm = (mm >> 1);
    l++;
  }
  return (l-1);
}

/** Computes /f$n\ge N/f$ such that /f$n=2^j,\, j\in\mathhb{N}_0/f$.
 */
INT X(next_power_of_2)(const INT N)
{
  INT n,i,logn;
  INT N_is_not_power_of_2=0;

  if (N == 0)
    return 1;
  else
  {
    n = N;
    logn = 0;
    while (n != 1)
    {
      if (n%2 == 1)
        N_is_not_power_of_2=1;
      n = n/2;
      logn++;
    }

    if (!N_is_not_power_of_2)
      logn--;

    for (i = 0; i <= logn; i++)
      n = n*2;

    return n;
  }
}

/** Computes /f$n\ge N/f$ such that /f$n=2^j,\, j\in\mathhb{N}_0/f$.
 */
void X(next_power_of_2_exp)(const INT N, int *N2, int *t)
{
  INT n,i,logn;
  INT N_is_not_power_of_2=0;

  if (N == 0)
  {
    *N2 = 1;
    *t = 0;
  }
  else
  {
    n=N;
    logn=0;
    while (n != 1)
    {
      if (n%2 == 1)
      {
          N_is_not_power_of_2=1;
      }
      n = n/2;
      logn++;
    }

    if (!N_is_not_power_of_2)
    {
      logn--;
    }

    for (i = 0; i <= logn; i++)
    {
      n = n*2;
    }

    *N2 = n;
    *t = logn+1;
  }
}

/** Computes integer /f$\prod_{t=0}^{d-1} v_t/f$. */
INT X(prod_int)(int *vec, INT d)
{
  INT t, prod;

  prod = 1;
  for (t = 0; t < d; t++)
    prod *= vec[t];

  return prod;
}

/** Computes integer /f$\prod_{t=0}^{d-1} v_t-a/f$. */
INT X(prod_minus_a_int)(int *vec, INT a, INT d)
{
  INT t, prod;

  prod = 1;
  for (t = 0; t < d; t++)
    prod *= vec[t] - a;

  return prod;
}
