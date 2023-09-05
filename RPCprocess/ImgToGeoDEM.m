function [ lat,lon,height ] = ImgToGeoDEM(imgx,imgy,DEM,DEMR,RPC)
%   coordinates in L1B img to lat and lon using DEM

[lat,lon] = ImgToGeo(imgx, imgy, RPC.height_off, RPC);
iter = 0; H1 = 0; H2 = 1;
while (abs(H2 - H1) > 0.01 && iter < 1000)
    H1 = H2;
    [demy,demx] = latlon2pix(DEMR,lat,lon);
    H2 = heightBilinear(demx,demy,DEM);
    [lat,lon]=ImgToGeo(imgx, imgy, H2, RPC);
    iter=iter+1;
    if (iter == 1000)
        disp("ImgToGeoDEM Out of Iterations!");
    end
end
height = H2;
end