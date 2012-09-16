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

#include "nfft3.h"
#include "infft.h"

void nfft_benchomp_createdataset(unsigned int d, unsigned int trafo_adjoint, int *N, int M, double sigma)
{
  int n[d];
  int t, j;
  R *x;
  C *f, *f_hat;
  int N_total = 1;

  for (t = 0; t < d; t++)
    N_total *= N[t];

  x = (R*) nfft_malloc(d*M*sizeof(R));
  f = (C*) nfft_malloc(M*sizeof(C));
  f_hat = (C*) nfft_malloc(N_total*sizeof(C));

  for (t=0; t<d; t++)
    n[t] = sigma*nfft_next_power_of_2(N[t]);

  /** init pseudo random nodes */
  nfft_vrand_shifted_unit_double(x,d*M);
 
  if (trafo_adjoint==0)
  {
    nfft_vrand_unit_complex(f_hat,N_total);
  }
  else
  {
    nfft_vrand_unit_complex(f,M);
  }

  printf("%d %d ", d, trafo_adjoint);

  for (t=0; t<d; t++)
    printf("%d ", N[t]);

  for (t=0; t<d; t++)
    printf("%d ", n[t]);

  printf("%d\n", M);

  for (j=0; j < M; j++)
  {
    for (t=0; t < d; t++)
      printf("%.16e ", x[d*j+t]);
    printf("\n");
  }

  if (trafo_adjoint==0)
  {
    for (j=0; j < N_total; j++)
      printf("%.16e %.16e\n", creal(f_hat[j]), cimag(f_hat[j]));
  }
  else
  {
    for (j=0; j < M; j++)
      printf("%.16e %.16e\n", creal(f[j]), cimag(f[j]));
  }

  nfft_free(x);
  nfft_free(f);
  nfft_free(f_hat);
}

int main(int argc, char **argv)
{
  int d;
  int *N;
  int M;
  int t;
  int trafo_adjoint;
  double sigma;

  if (argc < 6) {
    fprintf(stderr, "usage: d tr_adj N_1 ... N_d M sigma\n");
    return -1;
  }

  d = atoi(argv[1]);
  
  fprintf(stderr, "d=%d", d);

  if (d < 1 || argc < 5+d) {
    fprintf(stderr, "usage: d tr_adj N_1 ... N_d M sigma\n");
    return -1;
  }

  N = malloc(d*sizeof(int));

  trafo_adjoint = atoi(argv[2]);
  if (trafo_adjoint < 0 && trafo_adjoint > 1)
    trafo_adjoint = 1;

  fprintf(stderr, ", tr_adj=%d, N=", trafo_adjoint);

  for (t=0; t<d; t++)
    N[t] = atoi(argv[3+t]);

  for (t=0; t<d; t++)
    fprintf(stderr, "%d ",N[t]);


  M = atoi(argv[3+d]);
  sigma = atof(argv[4+d]);

  fprintf(stderr, ", M=%d, sigma=%.16g\n", M, sigma);

  nfft_benchomp_createdataset(d, trafo_adjoint, N, M, sigma);

  free(N);

  return 0;
}
