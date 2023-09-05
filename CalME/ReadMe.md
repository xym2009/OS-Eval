# OS-Eval --- Metric calculation code

config.txt --> the configuration file
  threshold:3  --> threshold in RANSAC
  Method:RIFT  --> method Name corresponds to the match link file
  use_dsm:0   -->  0 - dem 1 - dsm
  finematch:0  --> 0 - raw images  1 - refined by global methods 
  mode:RE    --> RE - relative dataset  AE - absolute dataset

# Acknowledgements
Some functions (RANSAC, LMS) are obtained from this repo ([https://github.com/LJY-RS/RIFT-multimodal-image-matching](https://github.com/ZeLianWen/Image-Registration)https://github.com/ZeLianWen/Image-Registration).
