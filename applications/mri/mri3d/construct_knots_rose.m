function [M] = construct_knots_rose( N,Z )

N=ceil(1.5*N);  
B=1;
M=N^2*Z;
file = zeros(M,3);

A=0.5;

w=N/64*50;

for z=0:Z-1,
for b=0:B-1,
for i=1:M/B/Z,
    t=i/(M/B/Z);
    file(z*M/Z+b*M/B+i,1) = A*cos(2*pi*w*t)*cos(2*pi*t+b*2*pi/B);
    file(z*M/Z+b*M/B+i,2) = A*cos(2*pi*w*t)*sin(2*pi*t+b*2*pi/B);
    file(z*M/Z+b*M/B+i,3) = z/Z-0.5;
end
end
end

% feel free to plot the knots by uncommenting
% plot(file(1:M/Z,1),file(1:M/Z,2),'.-');

save knots.dat -ascii file
