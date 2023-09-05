clear;close all;clc;

geocodedL2P = 'C:\Users\xym\source\repos\OSBenchMark\x64\Release\Preprocess-GecodedL2P.exe';

optN{1} = '18APR24174447-P2AS-013202438020_01_P001L1';
optN{2} = '18APR24174549-P2AS-013202438010_01_P001L1';
optN{3} = '19JUN05173227-P2AS-013202438070_01_P001L1';
optN{4} = '19JUN05173337-P2AS-013202438060_01_P001L1';
optN{5} = '19NOV09172335-P2AS-013202438040_01_P001L1';
optN{6} = '19NOV23174448-P2AS-013202438050_01_P001L1';
optN{7} = '19OCT28173153-P2AS-013202438030_01_P001L1';
optN{8} = '19SEP13172221-P2AS-011988822010_01_P002L1';
optN{9} = '19SEP13172312-P2AS-011988822010_01_P002L1';
optN{10} = 'SV1-02_20170822_L1B0000899817_1012000210040001_01-PAN';
optN{11} = 'SV1-03_20181207_L1B0000899862_1012000210060001_01-PAN';
sarN{1}='GF3_KAS_SL_015323_W95.9_N41.3_20190709_L1B_HH_L10004102699';
sarN{2}='GF3_KAS_SL_015331_W95.9_N41.3_20190709_L1B_HH_L10004105495';
sarN{3}='GF3_KAS_SL_015396_W95.9_N41.3_20190714_L1B_HH_L10004114146';
sarN{4}='GF3_KAS_SL_015432_W95.9_N41.3_20190716_L1B_HH_L10004118720';
sarN{5}='GF3_KAS_SL_015504_W95.9_N41.3_20190721_L1B_HH_L10004131104';
sarN{6}='GF3_KAS_SL_015568_W95.9_N41.3_20190726_L1B_HH_L10004141210';
sarN{7}='GF3_KAS_SL_015576_W95.9_N41.3_20190726_L1B_HH_L10004142416';
sarN{8}='GF3_KAS_SL_015641_W95.9_N41.3_20190731_L1B_HH_L10004152322';
sarN{9}='GF3_KAS_SL_015712_W95.9_N41.3_20190805_L1B_HH_L10004160896';
sarN{10}='GF3_KAS_SL_015749_W95.9_N41.3_20190807_L1B_HH_L10004166478';
sarN{11}='GF3_KAS_SL_916996_W95.9_N41.3_20191102_L1B_HH_L10004355418';
sarN{12}='GF3_KAS_SL_016159_W95.9_N41.3_20190905_L1B_HH_L10004226574';
sarN{13}='GF3_MYN_SL_015467_W95.9_N41.3_20190719_L1B_HH_L10004125928';

[DEMSRC,dR]=geotiffread('Relative-dataset\OMH_USGS10m_WGS84.tif');
outRes = 1.0; outLat = 41.293983;
for k=1:11
    for kk=1:13
        for kkk=4:9
            name1=[optN{k},'-',num2str(kkk)]; name2=[sarN{kk},'-',num2str(kkk)];
            X = ['Match ',name1,' ',name2];disp(X)
            file_image1=['Relative-dataset\Processed\OPT-ORG-DEM\',name1,'-L2.tiff'];
            file_image2=['Relative-dataset\Processed\SAR-ORG-DEM\',name2,'-L2.tiff'];
            %% geocodedL2P
            inRPC1 = [' Relative-dataset\OPT+RPC+GT\',name1,'.rpb'];
			inTIF1 = [' Relative-dataset\OPT+RPC+GT\',name1,'.tiff'];
            DEM = ' Relative-dataset\OMH_USGS10m_WGS84.tif'; outTIF1 = [' ',file_image1];
            if(~exist(inRPC1,'file'))
                continue;
            end
            if(~exist(file_image1,'file'))
                CMD = [geocodedL2P, inTIF1, inRPC1, DEM, outTIF1, ' ', num2str(outRes), ' ', num2str(outLat)];system(CMD);
            end
            inRPC2 = [' Relative-dataset\SAR+RPC+GT\',name2,'.rpb']; 
			inTIF2 = [' Relative-dataset\SAR+RPC+GT\',name2,'.tiff'];
            if(~exist(inRPC2,'file'))
                continue;
            end
            outTIF2 = [' ',file_image2];
            if(~exist(file_image2,'file'))
                CMD = [geocodedL2P, inTIF2, inRPC2, DEM, outTIF2, ' ', num2str(outRes), ' ', num2str(outLat)];system(CMD);
            end
            %% read geotiff
            matchName = ['./Links/',name1,'.tiff_Link_',name2,'.tiff.txt'];
            if(exist(matchName,'file'))
                 continue;
            end
            [image_1,R1]=geotiffread(file_image1);
            [image_2,R2]=geotiffread(file_image2);
            im1=imadjust(im2double(image_1));
            im2=imadjust(im2double(image_2));
            %% match geotiff
            tic
            [des_m1,des_m2] = RIFT_no_rotation_invariance(im1,im2,4,6,96);
            [indexPairs,matchmetric] = matchFeatures(des_m1.des,des_m2.des,'MaxRatio',1,'MatchThreshold', 100);
            matchedPoints1 = des_m1.kps(indexPairs(:, 1), :);
            matchedPoints2 = des_m2.kps(indexPairs(:, 2), :);
            toc
            %% save links
            addpath '..\RPCprocess'
            rpc_image1=['Relative-dataset\OPT+RPC+GT\',name1,'.rpb']; RPC1=readRPC(rpc_image1);
            cleanedPoints1 = matchedPoints1; cleanedPoints2 = matchedPoints2;
            fp=fopen(matchName,'w+');
            fprintf(fp,'%d\n',size(cleanedPoints1,1));
            for i = 1:size(cleanedPoints1,1)
                x1 = cleanedPoints1(i,1); y1 = cleanedPoints1(i,2);
                [lat1,lon1] = pix2latlon(R1,y1,x1);
                [demy,demx] = latlon2pix(dR,lat1,lon1);
                h1 = heightBilinear(demx,demy,DEMSRC);
                [imgx1,imgy1]=GeoToImg(lon1,lat1,h1,RPC1);
                x2 = cleanedPoints2(i,1); y2 = cleanedPoints2(i,2);
                [lat2,lon2] = pix2latlon(R2,y2,x2);
                [demy,demx] = latlon2pix(dR,lat2,lon2);
                h2 = heightBilinear(demx,demy,DEMSRC);
                [imgx2,imgy2]=GeoToImg(lon2,lat2,h2,RPC1);
                fprintf(fp, '%d\t%10f\t%10f\t%10f\t%10f\t%f\t%15f\t%15f\t%10f\t%15f\t%15f\t%10f', i, imgx1, imgy1, imgx2, imgy2, 1.0, x1, y1, h1, x2, y2, h2);
                if(i~=size(cleanedPoints1,1))
                    fprintf(fp,'\n');
                end
            end
            fclose(fp);
            X = ['Finish Match ',name1,' ',name2];
            disp(X)
        end
    end
end

