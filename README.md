# OS-Eval

This repo contains an evaluation dataset consisting of hundreds of pairs of sub-meter resolution optical and SAR images, and uses meta poles, which have clear structures in both optical and SAR images, as ground truth to quantitatively evaluate the optical and SAR registration algorithms. We hope this evaluation dataset can provide an equitable and highly precise comparison of hand-crafted algorithms, as well as a generalization evaluation for deep learning networks. The download link is：https://pan.baidu.com/s/1P0b14oEercCHLTk4m5THKw?pwd=ikaa. Another link to Google Drive is updated as https://drive.google.com/file/d/1QPrp6kKG9eO9lwUvE5bjpahdlCVmfoal/view?usp=sharing. 

The contents of this dataset are subject to CC BY-NC-ND (Creative Commons Attribution-NoDerivatives 4.0 International).

# Structure of the dataset
    Relative-dataset -->
        OPT+RPC+GT --> contains tiffs, RPCs, and corresponding GTs (without geocoding)
        SAR+RPC+GT --> contains tiffs, RPCs, and corresponding GTs (slant range, without geocoding)
        OMH_USGS10m_WGS84.tif --> DEM with 10m resolution (WGS84)
        OMHdsm.tif  --> DSM with 0.5m resolution (WGS84)
        Processed --> Geocoded images using DEM/DSM
            OPT-ORG-DEM --> geocoded tiffs using DEM (1m GSD, WGS84)
            SAR-ORG-DEM --> geocoded tiffs using DEM (1m GSD, WGS84)
            OPT-ORG-DSM --> geocoded tiffs using DSM (1m GSD, WGS84)
            SAR-ORG-DSM --> geocoded tiffs using DSM (1m GSD, WGS84)
        shp --> Building shapefile from OpenStreetMap (1m GSD, WGS84)
        Meta --> metafiles of 11 optical products and 13 SAR products
    Absolute-dataset -->
        Google+GT --> contains ortho tiffs and corresponding GTs (Ortho images from Google Earth, WGS84)
        SAR+RPC+GT --> contains tiffs, RPCs, and corresponding GTs (slant range, without geocoding)
        ZZG_USGS10m_WGS84.tif --> DEM with 10m resolution (WGS84)
        ZZGdsm.tif  --> DSM with 0.5m resolution (WGS84)
        Processed --> Geocoded images using DEM/DSM
            OPT --> ortho tiffs resized to 1m GSD (WGS84)
            SAR-ORG-DEM --> geocoded tiffs using DEM (1m GSD, WGS84)
            SAR-ORG-DSM --> geocoded tiffs using DSM (1m GSD, WGS84)
        shp --> Building shapefile from OpenStreetMap (1m GSD, WGS84)
        Meta --> metafiles of 19 SAR products

All the images are stored in uint16 tiff format. Some examples are given as follows,
![image](https://github.com/xym2009/OS-Eval/assets/19380078/c7e9c4f3-43b7-4d1b-96e6-2127b3b13459)![image](https://github.com/xym2009/OS-Eval/assets/19380078/c4a1eed8-cbbd-414f-bd31-ea6fdf3f4c6c)

# Contact Us
If you have any questions or suggestions on improving the usability of the dataset, please contact Yuming Xiang (z199208081010@163.com/xiangym@aircas.ac.cn).

For more details, please refer to the corresponding paper: "A Global-to-local Algorithm for High-resolution Optical and SAR Image Registration" ([https://ieeexplore.ieee.org/abstract/document/9204802](https://ieeexplore.ieee.org/abstract/document/10233894)).

If you are using this dataset in your project, we kindly ask you to cite:

    @article{xiang2023global, 
    title={A Global-to-local Algorithm for High-resolution Optical and SAR Image Registration}, 
    author={Xiang, Yuming and Wang, Xuanqi and Wang, Feng and You, Hongjian and Qiu, Xiaolan and Fu, Kun}, 
    journal={IEEE Transactions on Geoscience and Remote Sensing}, 
    year={2023}, 
    publisher={IEEE}
    }
