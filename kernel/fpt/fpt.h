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

/* $Id: fpt.h 3100 2009-03-12 08:42:48Z keiner $ */

#ifndef _FPT_H_
#define _FPT_H_

/**
 * Holds data for a single multiplication step in the cascade summation.
 */
typedef struct fpt_step_
{
  bool stable;                            /**< Indicates if the values
                                               contained represent a fast or
                                               a slow stabilized step.        */
  int Ns;                                 /**< TODO Add comment here.         */
  int ts;                                 /**< TODO Add comment here.         */
  double **a11,**a12,**a21,**a22;         /**< The matrix components          */
  double *g;                              /**<                                */
} fpt_step;

/**
 * Holds data for a single cascade summation.
 */
typedef struct fpt_data_
{
  fpt_step **steps;                       /**< The cascade summation steps    */
  int k_start;                            /**< TODO Add comment here.         */
  double *alphaN;                         /**< TODO Add comment here.         */
  double *betaN;                          /**< TODO Add comment here.         */
  double *gammaN;                         /**< TODO Add comment here.         */
  double alpha_0;                         /**< TODO Add comment here.         */
  double beta_0;                          /**< TODO Add comment here.         */
  double gamma_m1;                        /**< TODO Add comment here.         */
  /* Data for direct transform. */        /**< TODO Add comment here.         */
  double *alpha;                          /**< TODO Add comment here.         */
  double *beta;                           /**< TODO Add comment here.         */
  double *gamma;                          /**< TODO Add comment here.         */
} fpt_data;

/**
 * Holds data for a set of cascade summations.
 */
typedef struct fpt_set_s_
{
  int flags;                              /**< The flags                     */
  int M;                                  /**< The number of DPT transforms  */
  int N;                                  /**< The transform length. Must be
                                               a power of two.               */
  int t;                                  /**< The exponent of N             */
  fpt_data *dpt;                          /**< The DPT transform data        */
  double **xcvecs;                        /**< Array of pointers to arrays
                                               containing the Chebyshev
                                               nodes                         */
  double *xc;                             /**< Array for Chebychev-nodes.    */
  double _Complex *temp;                          /**< */
  double _Complex *work;                          /**< */
  double _Complex *result;                        /**< */
  double _Complex *vec3;
  double _Complex *vec4;
  double _Complex *z;
  fftw_plan *plans_dct3;                  /**< Transform plans for the fftw
                                               library                       */
  fftw_plan *plans_dct2;                  /**< Transform plans for the fftw
                                               library                       */
  fftw_r2r_kind *kinds;                   /**< Transform kinds for fftw
                                               library                       */
  fftw_r2r_kind *kindsr;                  /**< Transform kinds for fftw
                                               library                       */

  int *lengths; /**< Transform lengths for fftw library */

  /* Data for slow transforms. */
  double *xc_slow;
} fpt_set_s;

#endif /*_FPT_H_*/
