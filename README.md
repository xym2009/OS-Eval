# OS-Eval

This repo contains an evaluation dataset consisting of hundreds of pairs of sub-meter resolution optical and SAR images, and use meta poles, which have clear structures in both optical and SAR images, as ground truth to quantitatively evaluate the optical and SAR registration algorithms. We hope this evaluation dataset can provide an equitable and highly precise comparison of hand-crafted algorithms, as well as a generalization evaluation for deep learning networks.

# Structure of the dataset
    Relative-dataset -->
        OPT+RPC+GT --> contains tiffs, RPCs, and corresponding GTs (without geocoding)
        SAR+RPC+GT --> contains tiffs, RPCs, and corresponding GTs (slant range, without geocoding)
        OMH_USGS10m_WGS84.tif --> DEM with 10m resolution (WGS84)
        OMHdsm.tif  --> DSM with 0.5m resolution (WGS84)
        Processed --> Geocoded images using DEM/DSM
            OPT-ORG-DEM --> geocoded tiffs using DEM
            SAR-ORG-DEM --> geocoded tiffs using DEM
            OPT-ORG-DSM --> geocoded tiffs using DSM
            SAR-ORG-DSM --> geocoded tiffs using DSM
    Absolute-dataset -->
        Google+GT --> contains ortho tiffs and corresponding GTs (Ortho images from Google Earth)
        SAR+RPC+GT --> contains tiffs, RPCs, and corresponding GTs (slant range, without geocoding)
        ZZG_USGS10m_WGS84.tif --> DEM with 10m resolution (WGS84)
        ZZGdsm.tif  --> DSM with 0.5m resolution (WGS84)
        Processed --> Geocoded images using DEM/DSM
            OPT --> ortho tiffs resized to 1m GSD
            SAR-ORG-DEM --> geocoded tiffs using DEM
            SAR-ORG-DSM --> geocoded tiffs using DSM

Some examples are given as follows,
![image](https://github.com/xym2009/OS-Eval/assets/19380078/c7e9c4f3-43b7-4d1b-96e6-2127b3b13459)![image](https://github.com/xym2009/OS-Eval/assets/19380078/c4a1eed8-cbbd-414f-bd31-ea6fdf3f4c6c)

The download link of the dataset will be released in a few days. Besides, we also provide the evaluation method and several public avaiable SOTA algorithms slightly modified to fit this dataset. 
