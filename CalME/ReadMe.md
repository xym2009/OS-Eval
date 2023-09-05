# OS-Eval --- Metric calculation code

Matches obtained by each method are stored in the folder "tempLink/RIFT/". 
For example, 
![1693893215259](https://github.com/xym2009/OS-Eval/assets/19380078/a9a298ae-78bb-48e7-83e4-8b8a08ddaa20)

Config.txt --> the configuration file 

    threshold:3  --> threshold in RANSAC
    Method:RIFT  --> method Name corresponds to the match link file
    use_dsm:0   -->  0 - dem 1 - dsm
    finematch:0  --> 0 - raw images  1 - refined by global methods 
    mode:RE    --> RE - relative dataset  AE - absolute dataset

# Acknowledgements
Some functions (RANSAC, LMS) are obtained from this repo ([https://github.com/LJY-RS/RIFT-multimodal-image-matching](https://github.com/ZeLianWen/Image-Registration)https://github.com/ZeLianWen/Image-Registration).
