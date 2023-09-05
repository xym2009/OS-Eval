function [ lat,lon ] = ImgToGeo(imgx,imgy,height,RPC)
%   coordinates in L1B img to lat and lon

threshold = 0.0000001;
cycle_times = 100000;
U=0;V=0;
W=(height-RPC.height_off)/RPC.height_scale;
y=(imgy-RPC.line_off)/RPC.line_scale;
x=(imgx-RPC.samp_off)/RPC.samp_scale;
a=RPC.line_num_coeff;
b=RPC.line_den_coeff;
c=RPC.samp_num_coeff;
d=RPC.samp_den_coeff;

times = 0;
while(1)
    if (times >=cycle_times)
        disp("ImgToGeo Out of Iterations!");
        break;
    end
    [tempU,tempV] = NewtonIter(U,V,W,y,x,a,b,c,d);
    if((abs(U-tempU)<threshold)&&(abs(V-tempV)<threshold))
        break;
    end
    U=tempU;
    V=tempV;
    times=times+1;
end
lat=U*RPC.lat_scale+RPC.lat_off;
lon=V*RPC.lon_scale+RPC.lon_off;
end

function [tempU,tempV] = NewtonIter(U,V,W,x,y,a,b,c,d)

p1=a(1)+a(2)*V+a(3)*U+a(4)*W+a(5)*V*U+a(6)*V*W+a(7)*U*W+a(8)*V*V+a(9)*U*U+a(10)*W*W+a(11)*U*V*W+a(12)*V*V*V+a(13)*V*U*U+a(14)*V*W*W+a(15)*V*V*U+a(16)*U*U*U+a(17)*U*W*W+a(18)*V*V*W+a(19)*U*U*W+a(20)*W*W*W;
p2=b(1)+b(2)*V+b(3)*U+b(4)*W+b(5)*V*U+b(6)*V*W+b(7)*U*W+b(8)*V*V+b(9)*U*U+b(10)*W*W+b(11)*U*V*W+b(12)*V*V*V+b(13)*V*U*U+b(14)*V*W*W+b(15)*V*V*U+b(16)*U*U*U+b(17)*U*W*W+b(18)*V*V*W+b(19)*U*U*W+b(20)*W*W*W;
p3=c(1)+c(2)*V+c(3)*U+c(4)*W+c(5)*V*U+c(6)*V*W+c(7)*U*W+c(8)*V*V+c(9)*U*U+c(10)*W*W+c(11)*U*V*W+c(12)*V*V*V+c(13)*V*U*U+c(14)*V*W*W+c(15)*V*V*U+c(16)*U*U*U+c(17)*U*W*W+c(18)*V*V*W+c(19)*U*U*W+c(20)*W*W*W;
p4=d(1)+d(2)*V+d(3)*U+d(4)*W+d(5)*V*U+d(6)*V*W+d(7)*U*W+d(8)*V*V+d(9)*U*U+d(10)*W*W+d(11)*U*V*W+d(12)*V*V*V+d(13)*V*U*U+d(14)*V*W*W+d(15)*V*V*U+d(16)*U*U*U+d(17)*U*W*W+d(18)*V*V*W+d(19)*U*U*W+d(20)*W*W*W;

f=p1/p2-x;
g=p3/p4-y;

fU=((a(3)+a(5)*V+a(7)*W+2*a(9)*U+a(11)*V*W+2*a(13)*V*U+a(15)*V*V+3*a(16)*U*U+a(17)*W*W+2*a(19)*U*W)*p2-p1*(b(3)+b(5)*V+b(7)*W+2*b(9)*U+b(11)*V*W+2*b(13)*V*U+b(15)*V*V+3*b(16)*U*U+b(17)*W*W+2*b(19)*U*W))/(p2*p2);
fV=((a(2)+a(5)*U+a(6)*W+2*a(8)*V+a(11)*U*W+3*a(12)*V*V+a(13)*U*U+a(14)*W*W+2*a(15)*V*U+2*a(18)*V*W)*p2-p1*(b(2)+b(5)*U+b(6)*W+2*b(8)*V+b(11)*U*W+3*b(12)*V*V+b(13)*U*U+b(14)*W*W+2*b(15)*V*U+2*b(18)*V*W))/(p2*p2);
gU=((c(3)+c(5)*V+c(7)*W+2*c(9)*U+c(11)*V*W+2*c(13)*V*U+c(15)*V*V+3*c(16)*U*U+c(17)*W*W+2*c(19)*U*W)*p4-p3*(d(3)+d(5)*V+d(7)*W+2*d(9)*U+d(11)*V*W+2*d(13)*V*U+d(15)*V*V+3*d(16)*U*U+d(17)*W*W+2*d(19)*U*W))/(p4*p4);
gV=((c(2)+c(5)*U+c(6)*W+2*c(8)*V+c(11)*U*W+3*c(12)*V*V+c(13)*U*U+c(14)*W*W+2*c(15)*V*U+2*c(18)*V*W)*p4-p3*(d(2)+d(5)*U+d(6)*W+2*d(8)*V+d(11)*U*W+3*d(12)*V*V+d(13)*U*U+d(14)*W*W+2*d(15)*V*U+2*d(18)*V*W))/(p4*p4);

tempU=U*(gU*fV-fU*gV)+(f*gV-g*fV);
tempV=V*(gU*fV-fU*gV)+(g*fU-f*gU);
tempU=tempU/(gU*fV-fU*gV);
tempV=tempV/(gU*fV-fU*gV);

end