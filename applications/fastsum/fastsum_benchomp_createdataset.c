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

/* $Id: simple_test.c 3372 2009-10-21 06:04:05Z skunis $ */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <complex.h>

#include "config.h"

#include "nfft3util.h"
#include "nfft3.h"
#include "infft.h"

void fastsum_benchomp_createdataset(unsigned int d, int L, int M)
{
  int t, j, k;
  R *x;
  R *y;
  C *alpha;

  x = (R*) nfft_malloc(d*L*sizeof(R));
  y = (R*) nfft_malloc(d*L*sizeof(R));
  alpha = (C*) nfft_malloc(L*sizeof(C));

  /** init source knots in a d-ball with radius 1 */
  k = 0;
  while (k < L)
  {
    double r_max = 1.0;
    double r2 = 0.0;

    for (j=0; j<d; j++)
      x[k*d+j] = 2.0 * r_max * (double)rand()/(double)RAND_MAX - r_max;

    for (j=0; j<d; j++)
      r2 += x[k*d+j] * x[k*d+j];

    if (r2 >= r_max * r_max)
      continue;

    k++;
  }

  nfft_vrand_unit_complex(alpha,L);

  /** init target knots in a d-ball with radius 1 */
  k = 0;
  while (k < M)
  {
    double r_max = 1.0;
    double r2 = 0.0;

    for (j=0; j<d; j++)
      y[k*d+j] = 2.0 * r_max * (double)rand()/(double)RAND_MAX - r_max;

    for (j=0; j<d; j++)
      r2 += y[k*d+j] * y[k*d+j];

    if (r2 >= r_max * r_max)
      continue;

    k++;
  }

  printf("%d %d %d\n", d, L, M);

  for (j=0; j < L; j++)
  {
    for (t=0; t < d; t++)
      printf("%.16e ", x[d*j+t]);
    printf("\n");
  }

  for (j=0; j < L; j++)
    printf("%.16e %.16e\n", creal(alpha[j]), cimag(alpha[j]));

  for (j=0; j < M; j++)
  {
    for (t=0; t < d; t++)
      printf("%.16e ", y[d*j+t]);
    printf("\n");
  }

  nfft_free(x);
  nfft_free(y);
  nfft_free(alpha);
}

int main(int argc, char **argv)
{
  int d;
  int L;
  int M;

  if (argc < 4) {
    fprintf(stderr, "usage: d L M\n");
    return -1;
  }

  d = atoi(argv[1]);
  L = atoi(argv[2]);
  M = atoi(argv[3]);

  fprintf(stderr, "d=%d, L=%d, M=%d\n", d, L, M);

  fastsum_benchomp_createdataset(d, L, M);

  return 0;
}

