clear;close all;
warning('off');
geocodedL2P = 'C:\Users\xym\source\repos\OSBenchMark\x64\Release\Preprocess-GecodedL2P.exe';
geocodedResize = 'C:\Users\xym\source\repos\OSBenchMark\x64\Release\Preprocess.exe';

optN{1} = 'ZZG-L20';
sarN{1}='GF3_KRN_SL_010497_W158.0_N21.4_20180808_L1B_HH_L10003377724';
sarN{2}='GF3_KRN_SL_010669_W157.9_N21.4_20180820_L1B_HH_L10003401490';
sarN{3}='GF3_KRN_SL_020284_W158.0_N21.4_20200617_L1B_HH_L10004876483';
sarN{4}='GF3_KRN_SL_020398_W157.9_N21.4_20200624_L1B_HH_L10004889000';
sarN{5}='GF3_KRN_SL_027880_W158.0_N21.4_20211126_L1B_HH_L10000000001';
sarN{6}='GF3_KRN_SL_028053_W157.9_N21.4_20211208_L1B_HH_L10000000001';
sarN{7}='GF3_KRN_SL_028125_W157.9_N21.4_20211213_L1B_HH_L10000000001';
sarN{8}='GF3_KRN_SL_028644_W157.9_N21.4_20220118_L1B_HH_L10000000001';
sarN{9}='GF3_KRN_SL_029235_W158.0_N21.4_20220228_L1B_HH_L10000000001';
sarN{10}='GF3_KRN_SL_029430_W158.0_N21.4_20220313_L1B_HH_L10000000001';
sarN{11}='GF3_KRN_SL_029848_W158.0_N21.4_20220411_L1B_HH_L10000000001';
sarN{12}='GF3_KRN_SL_919744_W158.0_N21.4_20200510_L1B_HH_L10004794605';
sarN{13}='GF3_MYN_SL_021144_W158.0_N21.4_20200815_L1B_HH_L10004993692';
sarN{14}='GF3_MYN_SL_025670_W157.9_N21.4_20210625_L1B_HH_L10000000001';
sarN{15}='GF3_MYN_SL_026924_W158.0_N21.4_20210920_L1B_HH_L10000000001';
sarN{16}='GF3_MYN_SL_027349_W158.0_N21.4_20211019_L1B_HH_L10000000001';
sarN{17}='GF3_MYN_SL_027414_W158.0_N21.4_20211024_L1B_HH_L10000000001';
sarN{18}='GF3_MYN_SL_028841_W157.9_N21.4_20220131_L1B_HH_L10000000001';
sarN{19}='GF3_MYN_SL_030268_W158.0_N21.4_20220510_L1B_HH_L10000000001';

outRes = 1.0; outLat = 21.324027;
[DEMSRC,dR]=geotiffread('H:\REFdata\USGS10m\ZhenZhuGang_USGS10m.tif');
%%
k=1;
for kkk=2:8
    name1=[optN{k},'-',num2str(kkk)]; 
    inTIF1 = [' H:\MatchTest\BenchMark2\',name1,'.tiff'];
    file_image1=['C:\Users\xym\Documents\MATLAB\Image-Registration-master\OSBenchMark\CFOG-main\L2\',name1,'-L2.tiff'];
    outTIF1 = [' ',file_image1];
    if(~exist(file_image1,'file'))
        CMD1 = [geocodedResize, inTIF1, outTIF1, ' ', num2str(outRes), ' ', num2str(outLat)];system(CMD1);
    end
    [~,srcR]=geotiffread(inTIF1);
    for kk=1:19
        name2=[sarN{kk},'-',num2str(kkk)];
        X = ['Match ',name1,' ',name2];disp(X)
        file_image2=['C:\Users\xym\Documents\MATLAB\Image-Registration-master\OSBenchMark\CFOG-main\L2\',name2,'-L2.tiff'];
       %% geocodedL2P
        DEM = ' H:\REFdata\USGS10m\ZhenZhuGang_USGS10m.tif'; 
        inRPC2 = [' H:\MatchTest\BenchMark2\',name2,'.rpb']; inTIF2 = [' H:\MatchTest\BenchMark2\',name2,'.tiff'];
        if(~exist(inRPC2,'file'))
            continue;
        end
        outTIF2 = [' ',file_image2];
        if(~exist(file_image2,'file'))
            CMD = [geocodedL2P, inTIF2, inRPC2, DEM, outTIF2, ' ', num2str(outRes), ' ', num2str(outLat)];system(CMD);
        end
        %% read geotiff
        matchName = ['./Links/',name1,'.tiff_Link_',name2,'.tiff.txt'];
%         if(exist(matchName,'file'))
%             continue;
%         end
        [im_Ref,R1]=geotiffread(file_image1); [im_Sen,R2]=geotiffread(file_image2);
        im_Ref=(im2double(im_Ref)); im_Sen=imadjust(im2double(im_Sen));
        %% match geotiff
        tic
        [des_m1,des_m2] = RIFT_no_rotation_invariance(im_Ref,im_Sen,4,6,96);
        [indexPairs,matchmetric] = matchFeatures(des_m1.des,des_m2.des,'MaxRatio',1,'MatchThreshold', 100);
        matchedPoints1 = des_m1.kps(indexPairs(:, 1), :); matchedPoints2 = des_m2.kps(indexPairs(:, 2), :);
%         [H,rmse,cor1,cor2]=FSC(matchedPoints1,matchedPoints2,'affine',3);
        size(cor1,1)
        toc
       %% save links
        addpath 'C:\Users\xym\Documents\MATLAB\Image-Registration-master\OSBenchMark\RPCprocess'
        addpath 'C:\Users\xym\Documents\MATLAB\Image-Registration-master\MyAvailable\OSSIFT_v1.3'
        [DEM,dR]=geotiffread('H:/REFdata/USGS10m/ZhenZhuGang_USGS10m.tif');
        rpc_image2=['H:\MatchTest\BenchMark2\',name2,'.rpb'];
        RPC2=readRPC(rpc_image2);
        cleanedPoints1 = matchedPoints1; cleanedPoints2 = matchedPoints2;
        fp=fopen(matchName,'w+');
        fprintf(fp,'%d\n',size(cleanedPoints1,1));
        for i = 1:size(cleanedPoints1,1)
            x1 = cleanedPoints1(i,1); y1 = cleanedPoints1(i,2);
            [lat1,lon1] = pix2latlon(R1,y1,x1);
            x2 = cleanedPoints2(i,1); y2 = cleanedPoints2(i,2);
            [lat2,lon2] = pix2latlon(R2,y2,x2);
            fprintf(fp, '%d\t%10f\t%10f\t%10f\t%10f\t%f\t%15f\t%15f\t%10f\t%15f\t%15f\t%10f', i, x1, y1, x2, y2, 1.0, lon1, lat1, 0, lon2, lat2, 0);
            if(i~=size(cleanedPoints1,1))
                fprintf(fp,'\n');
            end
        end
        fclose(fp);
       %% GT
%         addpath 'C:\Users\xym\Documents\MATLAB\Image-Registration-master\OSBenchMark\SARMatchTest\RPCprocess'
%         GTname1 = ['H:\MatchTest\BenchMark2\',name1,'-GT.txt'];
%         GTname2 = ['H:\MatchTest\BenchMark2\',name2,'-GT.txt'];
%         GT1=load(GTname1); GT2=load(GTname2);
%         gtx=[];gty=[];er=[];
%         for i = 1:size(GT1,1)
%             x1 = GT1(i,2); y1 = GT1(i,3);
%             if x1==0 || y1 ==0
%                 continue;
%             end
%             [lat1,lon1] = pix2latlon(srcR,y1,x1);
%             [imgy,imgx] = latlon2pix(R1,lat1,lon1);
%             x2 = GT2(i,2); y2 = GT2(i,3);
%             if x2==0 || y2 ==0
%                 continue;
%             end
%             Ts=H*[imgx;imgy;1];
%             [lat2,lon2] = pix2latlon(R2,Ts(2),Ts(1));
%             [demy, demx] = latlon2pix(dR,lat2,lon2);
%             demh = heightBilinear(demx, demy, DEMSRC);
%             [imgx3,imgy3] = GeoToImg(lon2, lat2, demh, RPC2);
%             er=[er,sqrt((imgx3-x2)^2+(imgy3-y2)^2)];
%         end
%         er=mean(er);
%         disp(['MeanER: ',num2str(er)]);
       %%
        X = ['Finish Match ',name1,' ',name2];
        disp(X)
    end
end