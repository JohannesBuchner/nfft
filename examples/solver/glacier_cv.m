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
% $Id: glacier_cv.m 3100 2009-03-12 08:42:48Z keiner $
N=256;
border_eps=0.1;

M=8345;

load vol87.dat -ascii
input_data=vol87(2:end,:);

x_range=max(input_data(:,1))-min(input_data(:,1));
input_data(:,1)=(input_data(:,1)-min(input_data(:,1))) /x_range*(1-2*border_eps)-0.5+border_eps;

y_range=max(input_data(:,2))-min(input_data(:,2));
input_data(:,2)=(input_data(:,2)-min(input_data(:,2))) /y_range*(1-2*border_eps)-0.5+border_eps;

% Resort samples randomly
P=randperm(M);
input_data=input_data(P,:);

M_cv_start=200;
M_cv_step=200;
M_cv_end=1000;

save input_data.dat -ascii -double -tabs input_data

system(sprintf('./glacier %d %d %d %d %d > output_data_cv.tex',N,M,M_cv_start,M_cv_step,M_cv_end));
