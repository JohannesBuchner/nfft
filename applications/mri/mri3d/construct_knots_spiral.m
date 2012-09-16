%
% Copyright (c) 2002, 2009 Jens Keiner, Stefan Kunis, Daniel Potts
%
% This program is free software; you can redistribute it and/or modify it under
% the terms of the GNU General Public License as published by the Free Software
% Foundation; either version 2 of the License, or (at your option) any later
% version.
%
% This program is distributed in the hope that it will be useful, but WITHOUT
% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
% FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
% details.
%
% You should have received a copy of the GNU General Public License along with
% this program; if not, write to the Free Software Foundation, Inc., 51
% Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
%
% $Id: construct_knots_spiral.m 3100 2009-03-12 08:42:48Z keiner $
function [M] = construct_knots_spiral ( N,Z )
B=1;
M=N^2*Z;
file = zeros(M,3);

A=0.5;

w=N/64*50;

for z=0:Z-1,
  for b=0:B-1,
    for i=1:M/B/Z,
      t=(i/(M/B/Z))^(1/2);
      file(z*M/Z+b*M/B+i,1) = A*t*cos(2*pi*w*t+b);
      file(z*M/Z+b*M/B+i,2) = A*t*sin(2*pi*w*t+b);
      file(z*M/Z+b*M/B+i,3) = z/Z-0.5;
    end
  end
end

% feel free to plot the knots by uncommenting
 plot(file(1:M/Z,1),file(1:M/Z,2),'.-');

save knots.dat -ascii file
