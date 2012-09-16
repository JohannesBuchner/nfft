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

/* $Id: malloc.c 3100 2009-03-12 08:42:48Z keiner $ */

#include <mex.h>
#include "nfft3.h"
#include "imex.h"

/** Replacement for fftw_malloc in mex files */
void *nfft_mex_malloc(size_t n)
{
  void *p;

  p = mxMalloc(n);

  /* Should never be reached if mxMalloc fails (in a mex file) but in Matlab
   * you never know... */
  if (!p)
    mexErrMsgTxt("Not enough memory.");

  mexMakeMemoryPersistent(p);

  return p;
}

/** Replacement for fftw_free in mex files */
void nfft_mex_free(void *p)
{
  if (p)
    mxFree(p);
}

/** install hooks. */
void nfft_mex_install_mem_hooks(void)
{
  nfft_malloc_hook = nfft_mex_malloc;
  nfft_free_hook = nfft_mex_free;
}
