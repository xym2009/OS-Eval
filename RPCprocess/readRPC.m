function [ RPC ] = readRPC( rpc_name )
%%   read RPC from a rpb file
data = importdata(rpc_name);
i=1;
coef=zeros(1,20);
while i<=size(data,1)
    temp = data{i};
    nums = split(temp,'=');
    flag = strip(nums{1},'both',' ');
    switch flag
        case '	longOffset'
            RPC.lon_off = str2double(strip(nums{2},'both',';'));
            i=i+1;
        case '	longScale'
            RPC.lon_scale = str2double(strip(nums{2},'both',';'));
            i=i+1;
        case '	latOffset'
            RPC.lat_off = str2double(strip(nums{2},'both',';'));
            i=i+1;
        case '	latScale'
            RPC.lat_scale = str2double(strip(nums{2},'both',';'));
            i=i+1;
        case '	heightOffset'
            RPC.height_off = str2double(strip(nums{2},'both',';'));
            i=i+1;
        case '	heightScale'
            RPC.height_scale = str2double(strip(nums{2},'both',';'));
            i=i+1;
        case '	lineOffset'
            RPC.line_off = str2double(strip(nums{2},'both',';'));
            i=i+1;
        case '	lineScale'
            RPC.line_scale = str2double(strip(nums{2},'both',';'));
            i=i+1;
        case '	sampOffset'
            RPC.samp_off = str2double(strip(nums{2},'both',';'));
            i=i+1;
        case '	sampScale'
            RPC.samp_scale = str2double(strip(nums{2},'both',';'));
            i=i+1;
        case '	lineNumCoef'
            for j=i+1:i+20
                temp1 = data{j};
                flag1 = strip(temp1,'both',';');
                flag1 = strip(flag1,'both',')');
                coef(j-i) = str2double(flag1);
            end
            RPC.line_num_coeff = coef;
            i=i+20;
        case '	lineDenCoef'
            for j=i+1:i+20
                temp1 = data{j};
                flag1 = strip(temp1,'both',';');
                flag1 = strip(flag1,'both',')');
                coef(j-i) = str2double(flag1);
            end
            RPC.line_den_coeff = coef;
            i=i+20;
        case '	sampNumCoef'
            for j=i+1:i+20
                temp1 = data{j};
                flag1 = strip(temp1,'both',';');
                flag1 = strip(flag1,'both',')');
                coef(j-i) = str2double(flag1);
            end
            RPC.samp_num_coeff = coef;
            i=i+20;
        case '	sampDenCoef'
            for j=i+1:i+20
                temp1 = data{j};
                flag1 = strip(temp1,'both',';');
                flag1 = strip(flag1,'both',')');
                coef(j-i) = str2double(flag1);
            end
            RPC.samp_den_coeff = coef;
            i=i+20;
        otherwise
            i=i+1;
    end
end   
end
    
