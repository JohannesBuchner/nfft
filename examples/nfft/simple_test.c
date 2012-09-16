/*
 * Copyright (c) 2002, 2009 Jens Keiner, Stefan Kunis, Daniel Potts
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

#include "nfft3util.h"
#include "nfft3.h"

void simple_test_nfft_1d(void)
{
  nfft_plan p;
  double t;

  int N=14;
  int M=19;
  int n=32;

  /** init an one dimensional plan */
  nfft_init_1d(&p,N,M);

  /** init pseudo random nodes */
  nfft_vrand_shifted_unit_double(p.x,p.M_total);
 
  /** precompute psi, the entries of the matrix B */
  if(p.nfft_flags & PRE_ONE_PSI)
      nfft_precompute_one_psi(&p);

  /** init pseudo random Fourier coefficients and show them */
  nfft_vrand_unit_complex(p.f_hat,p.N_total);
  nfft_vpr_complex(p.f_hat,p.N_total,"given Fourier coefficients, vector f_hat");

  /** direct trafo and show the result */
  t=nfft_second();
  ndft_trafo(&p);
  t=nfft_second()-t;
  nfft_vpr_complex(p.f,p.M_total,"ndft, vector f");
  printf(" took %e seconds.\n",t);

  /** approx. trafo and show the result */
  nfft_trafo(&p);
  nfft_vpr_complex(p.f,p.M_total,"nfft, vector f");

  /** approx. adjoint and show the result */
  ndft_adjoint(&p);
  nfft_vpr_complex(p.f_hat,p.N_total,"adjoint ndft, vector f_hat");

  /** approx. adjoint and show the result */
  nfft_adjoint(&p);
  nfft_vpr_complex(p.f_hat,p.N_total,"adjoint nfft, vector f_hat");

  /** finalise the one dimensional plan */
  nfft_finalize(&p);
}

void simple_test_nfft_2d(void)
{
  int K,N[2],n[2],k,M;
  double t;

  nfft_plan p;

  N[0]=32; n[0]=64;
  N[1]=14; n[1]=32;
  M=N[0]*N[1];
  K=16;

  t=nfft_second();
  /** init a two dimensional plan */
  nfft_init_guru(&p, 2, N, M, n, 7,
		 PRE_PHI_HUT| PRE_FULL_PSI| MALLOC_F_HAT| MALLOC_X| MALLOC_F |
		 FFTW_INIT| FFT_OUT_OF_PLACE,
		 FFTW_ESTIMATE| FFTW_DESTROY_INPUT);

  /** init pseudo random nodes */
  nfft_vrand_shifted_unit_double(p.x,p.d*p.M_total);

  /** precompute psi, the entries of the matrix B */
  if(p.nfft_flags & PRE_ONE_PSI)
    nfft_precompute_one_psi(&p);

  /** init pseudo random Fourier coefficients and show them */
  nfft_vrand_unit_complex(p.f_hat,p.N_total);

  t=nfft_second()-t;
  nfft_vpr_complex(p.f_hat,K,
              "given Fourier coefficients, vector f_hat (first few entries)");
  printf(" ... initialisation took %e seconds.\n",t);

  /** direct trafo and show the result */
  t=nfft_second();
  ndft_trafo(&p);
  t=nfft_second()-t;
  nfft_vpr_complex(p.f,K,"ndft, vector f (first few entries)");
  printf(" took %e seconds.\n",t);

  /** approx. trafo and show the result */
  t=nfft_second();
  nfft_trafo(&p);
  t=nfft_second()-t;
  nfft_vpr_complex(p.f,K,"nfft, vector f (first few entries)");
  printf(" took %e seconds.\n",t);

  /** direct adjoint and show the result */
  t=nfft_second();
  ndft_adjoint(&p);
  t=nfft_second()-t;
  nfft_vpr_complex(p.f_hat,K,"adjoint ndft, vector f_hat (first few entries)");
  printf(" took %e seconds.\n",t);

  /** approx. adjoint and show the result */
  t=nfft_second();
  nfft_adjoint(&p);
  t=nfft_second()-t;
  nfft_vpr_complex(p.f_hat,K,"adjoint nfft, vector f_hat (first few entries)");
  printf(" took %e seconds.\n",t);

  /** finalise the two dimensional plan */
  nfft_finalize(&p);
}

int main(void)
{
  system("clear");
  printf("1) computing an one dimensional ndft, nfft and an adjoint nfft\n\n");
  simple_test_nfft_1d();
  getc(stdin);

  system("clear");
  printf("2) computing a two dimensional ndft, nfft and an adjoint nfft\n\n");
  simple_test_nfft_2d();

  return 1;
}
