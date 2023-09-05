function [ imgx, imgy ] = GeoToImg( lon, lat, height, RPC)

V=(lon-RPC.lon_off)/RPC.lon_scale;
U=(lat-RPC.lat_off)/RPC.lat_scale;
W=(height-RPC.height_off)/RPC.height_scale;
a=RPC.line_num_coeff;
b=RPC.line_den_coeff;
c=RPC.samp_num_coeff;
d=RPC.samp_den_coeff;

p1=a(1)+a(2)*V+a(3)*U+a(4)*W+a(5)*V*U+a(6)*V*W+a(7)*U*W+a(8)*V*V+a(9)*U*U+a(10)*W*W+a(11)*U*V*W+a(12)*V*V*V+a(13)*V*U*U+a(14)*V*W*W+a(15)*V*V*U+a(16)*U*U*U+a(17)*U*W*W+a(18)*V*V*W+a(19)*U*U*W+a(20)*W*W*W;
p2=b(1)+b(2)*V+b(3)*U+b(4)*W+b(5)*V*U+b(6)*V*W+b(7)*U*W+b(8)*V*V+b(9)*U*U+b(10)*W*W+b(11)*U*V*W+b(12)*V*V*V+b(13)*V*U*U+b(14)*V*W*W+b(15)*V*V*U+b(16)*U*U*U+b(17)*U*W*W+b(18)*V*V*W+b(19)*U*U*W+b(20)*W*W*W;
p3=c(1)+c(2)*V+c(3)*U+c(4)*W+c(5)*V*U+c(6)*V*W+c(7)*U*W+c(8)*V*V+c(9)*U*U+c(10)*W*W+c(11)*U*V*W+c(12)*V*V*V+c(13)*V*U*U+c(14)*V*W*W+c(15)*V*V*U+c(16)*U*U*U+c(17)*U*W*W+c(18)*V*V*W+c(19)*U*U*W+c(20)*W*W*W;
p4=d(1)+d(2)*V+d(3)*U+d(4)*W+d(5)*V*U+d(6)*V*W+d(7)*U*W+d(8)*V*V+d(9)*U*U+d(10)*W*W+d(11)*U*V*W+d(12)*V*V*V+d(13)*V*U*U+d(14)*V*W*W+d(15)*V*V*U+d(16)*U*U*U+d(17)*U*W*W+d(18)*V*V*W+d(19)*U*U*W+d(20)*W*W*W;

y=p1/p2;
x=p3/p4;
imgx=x*RPC.samp_scale+RPC.samp_off;
imgy=y*RPC.line_scale+RPC.line_off;

end

