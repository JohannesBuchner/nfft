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

#define z_swap(_a_, _b_, _t_) do { (_t_) = (_a_); (_a_) = (_b_); (_b_) = (_t_); } while (0)

/**
 * Auxiliary function of radix sort for node indices.
 *
 * \author Michael Hofmann
 */
static inline void sort_node_indices_sort_bubble(int n, int *keys)
{
  int i, j, ti;

  for (i = 0; i < n; ++i)
  {
    j = i;
    while (j > 0 && keys[2 * j + 0] < keys[2 * (j - 1) + 0])
    {
      z_swap(keys[2 * j + 0], keys[2 * (j - 1) + 0], ti);
      z_swap(keys[2 * j + 1], keys[2 * (j - 1) + 1], ti);
      --j;
    }
  }
}

/**
 * Auxiliary function of radix sort for node indices.
 *
 * \author Michael Hofmann
 */
static inline void sort_node_indices_radix_count(int n, int *keys, int shift, int mask, int *counts)
{
  int i, k;

  for (i = 0; i < n; ++i)
  {
    k = (keys[2 * i + 0] >> shift) & mask;
    ++counts[k];
  }
}

/**
 * Auxiliary function of radix sort for node indices.
 *
 * \author Michael Hofmann
 */
static inline void sort_node_indices_radix_rearrange(int n, int *keys_in, int *keys_out, int shift, int mask, int *displs)
{
  int i, k;

  for (i = 0; i < n; ++i)
  {
    k = (keys_in[2 * i + 0] >> shift) & mask;
    keys_out[2 * displs[k] + 0] = keys_in[2 * i + 0];
    keys_out[2 * displs[k] + 1] = keys_in[2 * i + 1];
    ++displs[k];
  }
}

/**
 * Radix sort for node indices with OpenMP support.
 *
 * \author Michael Hofmann
 */
void X(sort_node_indices_radix_lsdf)(int n, int *keys0, int *keys1, int rhigh)
{
  const int rwidth = 9;
  const int radix_n = 1 << rwidth;
  const int radix_mask = radix_n - 1;
  const int rhigh_in = rhigh;

  const int tmax =
#ifdef _OPENMP
    omp_get_max_threads();
#else
    1;
#endif

  int *from, *to, *tmp;

  int i, k, l, h;
  int lcounts[tmax * radix_n];

  int tid = 0, tnum = 1;


  from = keys0;
  to = keys1;

  while (rhigh >= 0)
  {
#ifdef _OPENMP
    #pragma omp parallel private(tid, tnum, i, l, h)
    {
      tid = omp_get_thread_num();
      tnum = omp_get_num_threads();
#endif

      for (i = 0; i < radix_n; ++i) lcounts[tid * radix_n + i] = 0;

      l = (tid * n) / tnum;
      h = ((tid + 1) * n) / tnum;

      sort_node_indices_radix_count(h - l, from + (2 * l), rhigh_in - rhigh, radix_mask, &lcounts[tid * radix_n]);
#ifdef _OPENMP
    }
#endif

    k = 0;
    for (i = 0; i < radix_n; ++i)
    {
      for (l = 0; l < tmax; ++l) lcounts[l * radix_n + i] = (k += lcounts[l * radix_n + i]) - lcounts[l * radix_n + i];
    }

#ifdef _OPENMP
    #pragma omp parallel private(tid, tnum, i, l, h)
    {
      tid = omp_get_thread_num();
      tnum = omp_get_num_threads();
#endif

      l = (tid * n) / tnum;
      h = ((tid + 1) * n) / tnum;

      sort_node_indices_radix_rearrange(h - l, from + (2 * l), to, rhigh_in - rhigh, radix_mask, &lcounts[tid * radix_n]);
#ifdef _OPENMP
    }
#endif

/*    print_keys(n, to);*/

    tmp = from;
    from = to;
    to = tmp;

    rhigh -= rwidth;
  }

  if (to == keys0) memcpy(to, from, n * 2 * sizeof(int));
}

/**
 * Radix sort for node indices with OpenMP support.
 *
 * \author Michael Hofmann
 */
void X(sort_node_indices_radix_msdf)(int n, int *keys0, int *keys1, int rhigh)
{
  const int rwidth = 9;
  const int radix_n = 1 << rwidth;
  const int radix_mask = radix_n - 1;

  const int tmax =
#ifdef _OPENMP
    omp_get_max_threads();
#else
    1;
#endif

  int i, k, l, h;
  int lcounts[tmax * radix_n];

  int counts[radix_n], displs[radix_n];

  int tid = 0, tnum = 1;


  rhigh -= rwidth;

#ifdef _OPENMP
  #pragma omp parallel private(tid, tnum, i, l, h)
  {
    tid = omp_get_thread_num();
    tnum = omp_get_num_threads();
#endif

    for (i = 0; i < radix_n; ++i) lcounts[tid * radix_n + i] = 0;

    l = (tid * n) / tnum;
    h = ((tid + 1) * n) / tnum;

    sort_node_indices_radix_count(h - l, keys0 + (2 * l), rhigh + 1, radix_mask, &lcounts[tid * radix_n]);
#ifdef _OPENMP
  }
#endif

  k = 0;
  for (i = 0; i < radix_n; ++i)
  {
    for (l = 0; l < tmax; ++l) lcounts[l * radix_n + i] = (k += lcounts[l * radix_n + i]) - lcounts[l * radix_n + i];

    displs[i] = lcounts[0 * radix_n + i];
    if (i > 0) counts[i - 1] = displs[i] - displs[i - 1];
  }
  counts[radix_n - 1] = n - displs[radix_n - 1];

#ifdef _OPENMP
  #pragma omp parallel private(tid, tnum, i, l, h)
  {
    tid = omp_get_thread_num();
    tnum = omp_get_num_threads();
#endif

    l = (tid * n) / tnum;
    h = ((tid + 1) * n) / tnum;

    sort_node_indices_radix_rearrange(h - l, keys0 + (2 * l), keys1, rhigh + 1, radix_mask, &lcounts[tid * radix_n]);
#ifdef _OPENMP
  }
#endif

  memcpy(keys0, keys1, n * 2 * sizeof(int));

  if (rhigh >= 0)
  {
    for (i = 0; i < radix_n; ++i)
    {
      if (counts[i] > 1)
      {
        if (counts[i] > 256)
          X(sort_node_indices_radix_msdf)(counts[i], keys0 + 2 * displs[i], keys1 + 2 * displs[i], rhigh);
        else
          sort_node_indices_sort_bubble(counts[i], keys0 + 2 * displs[i]);
      }
    }
  }
}
