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

#include <stdio.h>
#include "infft.h"

/** Print real vector to standard output. */
void X(vpr_double)(R *x, const INT n, const char *text)
{
  INT k;

  if (x == NULL)
  {
    printf("null pointer\n");
    fflush(stdout);
    exit(-1);
  }

  if (text != NULL)
  {
    printf ("\n %s, adr=%p\n", text, (void*)x);

    for (k = 0; k < n; k++)
    {
      if (k%8 == 0)
        printf("%6d.\t", k);

      printf("%+.1" FE ",", x[k]);

      if (k%8 == 7)
        printf("\n");
    }

    if (n%8 != 0)
      printf("\n");
  }
  else
    for (k = 0; k < n; k++)
      printf("%+" FE ",\n", x[k]);

  fflush(stdout);
}

/** Print complex vector to standard output. */
void X(vpr_complex)(C *x, const INT n, const char *text)
{
  INT k;

  if(text != NULL)
  {
    printf("\n %s, adr=%p\n", text, (void*)x);
    for (k = 0; k < n; k++)
    {
      if (k%4 == 0)
        printf("%6d.\t", k);

      printf("%+.1" FE "%+.1" FE "i,", CREAL(x[k]), CIMAG(x[k]));

      if (k%4==3)
        printf("\n");
    }
    if (n%4!=0)
      printf("\n");
  }
  else
    for (k = 0; k < n; k++)
      printf("%+" FE "%+" FE "i,\n", CREAL(x[k]), CIMAG(x[k]));

  fflush(stdout);
}
