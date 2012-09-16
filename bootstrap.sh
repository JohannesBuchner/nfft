#!/bin/sh
#
# $Id$
#
# Copyright (c) 2003, 2006 Matteo Frigo
# Copyright (c) 2003, 2006 Massachusetts Institute of Technology
# Copyright (c) 2007 Jens Keiner
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
################################################################################
# NOTE: If you just want to build NFFT3, do not use this file. Just follow the 
# installation instructions as described in the tutorial found under 
# doc/tutorial.
#
# This file is based on the bootstrap.sh script from FFTW 3.1.2 by
# M. Frigo and S. G. Johnson
################################################################################

touch ChangeLog

echo "PLEASE IGNORE WARNINGS AND ERRORS"

# paranoia: sometimes autoconf doesn't get things right the first time
rm -rf autom4te.cache
glibtoolize
autoreconf --verbose --install --force
autoreconf --verbose --install --force
autoreconf --verbose --install --force

rm -f config.cache
