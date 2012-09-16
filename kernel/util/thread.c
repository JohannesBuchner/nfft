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

#ifdef _OPENMP
#include <omp.h>
#endif

INT X(get_num_threads)(void)
{
#ifdef _OPENMP
  INT nthreads;
  #pragma omp parallel default(shared)
  {
    INT n = (INT)omp_get_num_threads();
    #pragma omp master
    {
      nthreads = n;
    }
  }
  return nthreads;
#else
  return 1;
#endif
}
