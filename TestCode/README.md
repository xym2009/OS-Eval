# OS-Eval --- Test code

Here we present an example to generate matches used to evaluate the registration accuracy.

# Structure of the matches 
The matches are stored in txt format.
The first line is the number of all-matches. 
Then each line is stored in the following type, 

Num imgx1 imgy1 imgx2 imgy2 1.0 lon1 lat1 height1 lon2 lat2 height2 

A simple matlab code is given as follows,

    fp=fopen(matchName,'w+');
    fprintf(fp,'%d\n',size(matches,1));
    for i = 1:size(matches,1)
        x1 = matches(i,1); y1 = matches(i,2);
        [lat1,lon1] = pix2latlon(R1,y1,x1);
        x2 = cleanedPoints2(i,1); y2 = matches(i,2);
        [lat2,lon2] = pix2latlon(R2,y2,x2);
        fprintf(fp, '%d\t%10f\t%10f\t%10f\t%10f\t%f\t%15f\t%15f\t%10f\t%15f\t%15f\t%10f', i, x1, y1, x2, y2, 1.0, lon1, lat1, 0, lon2, lat2, 0);
        if(i~=size(matches,1))
            fprintf(fp,'\n');
        end
    end
    fclose(fp);

# Acknowledgements
The example code is based on the RIFT algorithm, we thank Dr.Jiayuan Li for the open source of RIFT(https://github.com/LJY-RS/RIFT-multimodal-image-matching).
