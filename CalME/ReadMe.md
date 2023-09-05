# OS-Eval --- Metric calculation code

Here we present an example to generate matches, which are used to evaluate the registration accuracy.

The code is tested on ubuntu 18.04 with gdal1.11 & opencv460. 

Config.txt is the configuration file: 

  threshold:3   //  threshold for RANSAC
  Method:RIFT   //  test method to load matches TXT
  reference:0   //  test image
  sensed:0      //  test image
  use_dsm:0     //  0 -- dem 1 -- dsm
  testN:4       //  test patch
  finematch:0   //  0 -- raw image 1 -- refine match by ASS
  mode:RE       //  RE -- relative dataset AE -- absolute dataset

# Acknowledgements
Some functions are obtained from this repo (https://github.com/LJY-RS/RIFT-multimodal-image-matching).
