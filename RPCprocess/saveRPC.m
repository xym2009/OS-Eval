function saveRPC(rpc_name, sRPC)
%SAVERPC 此处显示有关此函数的摘要
%   此处显示详细说明
fp=fopen(rpc_name,'w+');
fprintf(fp,'satId = "test";\n');
fprintf(fp,'bandId = "1";\n');
fprintf(fp,'SpecId = "RPC";\n');
fprintf(fp,'BEGIN_GROUP = IMAGE\n');
fprintf(fp,'	errBias = 1.0;\n');
fprintf(fp,'	errRand = 0.0;\n');
fprintf(fp,'	lineOffset = %.15f;\n',sRPC.line_off);
fprintf(fp,'	sampOffset = %.15f;\n',sRPC.samp_off);	
fprintf(fp,'	latOffset = %.15f;\n',sRPC.lat_off);	
fprintf(fp,'	longOffset = %.15f;\n',sRPC.lon_off);	
fprintf(fp,'	heightOffset = %.15f;\n',sRPC.height_off);	
fprintf(fp,'	lineScale = %.15f;\n',sRPC.line_scale);	
fprintf(fp,'	sampScale = %.15f;\n',sRPC.samp_scale);	
fprintf(fp,'	latScale = %.15f;\n',sRPC.lat_scale);	
fprintf(fp,'	longScale = %.15f;\n',sRPC.lon_scale);	
fprintf(fp,'	heightScale = %.15f;\n',sRPC.height_scale);	
fprintf(fp,'	lineNumCoef = (\n');
for ii = 1:19
    fprintf(fp,' %.11f,\n',sRPC.line_num_coeff(ii));
end
fprintf(fp,' %.11f);\n',sRPC.line_num_coeff(20));
fprintf(fp,'	lineDenCoef = (\n');
for ii = 1:19
    fprintf(fp,' %.11f,\n',sRPC.line_den_coeff(ii));
end
fprintf(fp,' %.11f);\n',sRPC.line_den_coeff(20));
fprintf(fp,'	sampNumCoef = (\n');
for ii = 1:19
    fprintf(fp,' %.11f,\n',sRPC.samp_num_coeff(ii));
end
fprintf(fp,' %.11f);\n',sRPC.samp_num_coeff(20));
fprintf(fp,'	sampDenCoef = (\n');
for ii = 1:19
    fprintf(fp,' %.11f,\n',sRPC.samp_den_coeff(ii));
end
fprintf(fp,' %.11f);\n',sRPC.samp_den_coeff(20));
fprintf(fp,'END_GROUP = IMAGE\n');
fprintf(fp,'END;\n');
fclose(fp);
end

