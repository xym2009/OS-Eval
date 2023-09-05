function [tmp] = heightBilinear(imgx,imgy,SRC)
% Matlab img coordinate should add 0.5
imgx=imgx+0.5; imgy=imgy+0.5;
srcX = floor(imgx);
srcY = floor(imgy);
u = imgx - srcX;
v = imgy - srcY;
if(srcY<=0||srcX<=0||(srcX+1)>size(SRC,2)||(srcY+1)>size(SRC,1))
    tmp = 0;
else
    tmp = (1 - u) * (1 - v) * SRC(srcY, srcX) + (1 - u) * v * SRC(srcY +1, srcX) + u * (1 - v) * SRC(srcY, srcX + 1) + u * v * SRC(srcY + 1, srcX + 1);
end
end

