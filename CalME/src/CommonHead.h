#pragma once
#ifdef _WIN32
#include "ogr_geos.h"
#include "fftw3.h"
#include <Windows.h>
#include <io.h>
#include <stdio.h>
#include <nb30.h>
#pragma comment(lib,"Netapi32.lib")
#else
//#include "fftw/fftw3.h"
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
//#include "direct.h"
#define EVENT_SIZE (sizeof (struct inotify_event))
#define EVENT_BUF_LEN (1024*(EVENT_SIZE+16))
#endif
#include <string>
#include <stdio.h>
#include <fstream>
#include <cctype>
#include <limits.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <iomanip>//用于setprecision(16) 限定输出流的格式等
#include "map"
#include "numeric"
#include "stdio.h"
#include "stdarg.h"
#include "memory"
#include <vector>
#include <algorithm>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <ctime>
//GDAL
#include "gdalwarper.h"
#include "ogr_spatialref.h"
#include "ogr_geometry.h"//用于包含OGRGeometry类
#include "gdal_alg.h"
#include "ogr_api.h"
#include "cpl_string.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
//#include "LL2UTM.h"
//opencv
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//Eigen
//#include <Eigen/Dense>
//#include <opencv2/core/eigen.hpp>
#include "Logger.h"
