function [M] = construct_knots_radial_3d( N,P )
xk=zeros(N*P^2,4);
counter=1;
for r=0:N-1,
 for i=1:P,
  for j=1:P,
   x1=sqrt(2)*r/N/2*cos(2*(i-1/2)*pi/P)*sin((j-1/2)*pi/P);
   x2=sqrt(2)*r/N/2*sin(2*(i-1/2)*pi/P)*sin((j-1/2)*pi/P);
   x3=sqrt(2)*r/N/2*cos((j-1/2)*pi/P);
   if (abs(x1)<0.5 & abs(x2)<0.5 & abs(x3)<0.5),
     if(r==0)
       xk(counter,:)=[ x1 x2 x3 (r/N/2)^2*pi]; %weights
     else
       xk(counter,:)=[ x1 x2 x3 sqrt(2)*r/N/2*sin((j-1/2)*pi/P)]; %weights
     end
     counter=counter+1;
   end 
  end
 end
end

xk=xk(1:counter,:);

% feel free to plot the knots by uncommenting
% plot3(xk(:,1),xk(:,2),xk(:,3),'.');
M=size(xk,1);

knots=xk(:,1:3);
weights=xk(:,4);
save knots.dat -ascii knots
save weights.dat -ascii weights

