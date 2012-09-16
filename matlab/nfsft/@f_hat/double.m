%DOUBLE Double conversion for f_hat class
%   Copyright (c) 2006, 2009 Jens Keiner, Stefan Kunis, Daniel Potts

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
% $Id: double.m 3108 2009-03-13 12:57:05Z keiner $
function d = double(p)

d = zeros(2*p.N+1,p.N+1);
o = 1;
for k = 0:p.N
  d(k*(2*p.N+1)+p.N+1-k:k*(2*p.N+1)+p.N+1+k) = p.f_hat(o:o+2*k);
  o = o + 2*k+1;
end
