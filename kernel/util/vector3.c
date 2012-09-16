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

/** Updates \f$x \leftarrow a x + y\f$. */
void X(upd_axpy_complex)(C *x, R a, C *y, INT n)
{
  INT k;

  for (k = 0; k < n; k++)
    x[k] = a * x[k] + y[k];
}

/** Updates \f$x \leftarrow a x + y\f$. */
void X(upd_axpy_double)(R *x, R a, R *y, INT n)
{
  INT k;

  for (k = 0; k < n; k++)
    x[k] = a * x[k] + y[k];
}


/** Updates \f$x \leftarrow x + a y\f$. */
void X(upd_xpay_complex)(C *x, R a, C *y, INT n)
{
  INT k;

  for (k = 0; k < n; k++)
    x[k] += a * y[k];
}

/** Updates \f$x \leftarrow x + a y\f$. */
void X(upd_xpay_double)(R *x, R a, R *y, INT n)
{
  INT k;

  for (k = 0; k < n; k++)
    x[k] += a * y[k];
}

/** Updates \f$x \leftarrow a x + b y\f$. */
void X(upd_axpby_complex)(C *x, R a, C *y, R b, INT n)
{
  INT k;

  for (k = 0; k < n; k++)
    x[k] = a * x[k] + b * y[k];
}

/** Updates \f$x \leftarrow a x + b y\f$. */
void X(upd_axpby_double)(R *x, R a, R *y, R b, INT n)
{
  INT k;

  for (k = 0; k < n; k++)
    x[k] = a * x[k] + b * y[k];
}

/** Updates \f$x \leftarrow x + a w\odot y\f$. */
void X(upd_xpawy_complex)(C *x, R a, R *w, C *y, INT n)
{
  INT k;

  for (k = 0; k < n; k++)
    x[k] += a * w[k] * y[k];
}

/** Updates \f$x \leftarrow x + a w\odot y\f$. */
void X(upd_xpawy_double)(R *x, R a, R *w, R *y, INT n)
{
  INT k;

  for (k = 0; k < n; k++)
    x[k] += a * w[k] * y[k];
}

/** Updates \f$x \leftarrow a x +  w\odot y\f$. */
void X(upd_axpwy_complex)(C *x, R a, R *w, C *y, INT n)
{
  INT k;

  for (k = 0; k < n; k++)
    x[k] = a * x[k] + w[k] * y[k];
}

/** Updates \f$x \leftarrow a x +  w\odot y\f$. */
void X(upd_axpwy_double)(R *x, R a, R *w, R *y, INT n)
{
  INT k;

  for (k = 0; k < n; k++)
    x[k] = a * x[k] + w[k] * y[k];
}

/** Swaps each half over N[d]/2. */
void X(fftshift_complex)(C *x, int d, int* N)
{
  int d_pre, d_act, d_post;
  int N_pre, N_act, N_post;
  int k_pre, k_act, k_post;
  int k, k_swap;

  C x_swap;

  for (d_act = 0; d_act < d; d_act++)
  {
    for (d_pre = 0, N_pre = 1; d_pre < d_act; d_pre++)
      N_pre *= N[d_pre];

    N_act = N[d_act];

    for (d_post = d_act + 1, N_post = 1; d_post < d; d_post++)
      N_post *= N[d_post];

    for (k_pre = 0; k_pre < N_pre; k_pre++)
      for (k_act = 0; k_act < N_act / 2; k_act++)
        for (k_post = 0; k_post < N_post; k_post++)
        {
          k = (k_pre * N_act + k_act) * N_post + k_post;
          k_swap = (k_pre * N_act + k_act + N_act / 2) * N_post + k_post;

          x_swap = x[k];
          x[k] = x[k_swap];
          x[k_swap] = x_swap;
        }
  }
}
