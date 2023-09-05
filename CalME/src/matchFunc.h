#pragma once
#include "CommonHead.h"

using namespace cv;
using namespace std;

inline vector<string> split_string(const string& str, const string& pattern)
{
    vector<string> ret;
    if (pattern.empty()) return ret;
    size_t start = 0, index = str.find_first_of(pattern, 0);
    while (index != str.npos)
    {
        if (start != index)
            ret.push_back(str.substr(start, index - start));
        start = index + 1;
        index = str.find_first_of(pattern, start);
    }
    if (!str.substr(start).empty())
        ret.push_back(str.substr(start));
    return ret;
}

inline bool IsFileExist(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

typedef struct Pt_Info_Match
{
	double imgx;
	double imgy;
	double PiPeiDu;
	double geox;
	double geoy;
	double geoz;
	int Regist_index;
	int flag_abnormal = 0;
}Pt_Info_Match;

class Stereo{

public:
	Stereo();
	~Stereo();

	//rpc
	GDALRPCInfo sRPC;
	GDALRPCInfo sRPC2;
	//dem para
	double	_geotransform[6];
	float* demdata;
	int  demwidth, demheight;

	int ReadLRPC(string rpc_path);
	int ReadRRPC(string rpc_path);
	int readDEMData(string path);
	void GeoToMeter(double lon, double lat, double latpos, double &ratiox, double &ratioy);
	void meterTogeo(double ratiox,double ratioy,double lat,double &ratioLong,double &ratioLat);
	void Geo2img(double longi,double lat,double &imgx,double &imgy,double height, int flag);
	int Img2Geo(double imgx,double imgy,double &longi,double &lat,double height, int flag);
	int Img2GeoDEM(double imgx, double imgy, double& longi, double& lat, double& height, int flag);
	void GeoToImgPointDouble(double geox, double geoy, double &imgx,double &imgy,double *padfTransform);
	void img2Geopoint(int imgx, int imgy, double& geox, double& geoy, double* padfTransform);
	void heightBilinearInter(double &height, double imgx, double imgy, int tifWidth, int tifHeight, float *buf);
};

class matchFunc
{
public:
	matchFunc();
	~matchFunc();

    string optTIF;
    string sarTIF;
    string optRPC;
    string sarRPC;
    string dempath;
	string dsmpath;
    string shpTIF;
    string optGT;
    string sarGT;
    int templateSize;
    float th;
    int use_dsm;
    int use_ransac;
    double sigma;
    int dilation;
    double DEMrate;
    double scale;

    void EvalRE(string method, ULogger::Logger &log);
	void EvalAE(string method, ULogger::Logger &log);
	void EvalAE_DSM(string method, ULogger::Logger &log);

private:
    string name1; 
    string name2;
    double IMGGTevaluate(vector<Point2f> lxy, vector<Point2f> rxy, string GT1, string GT2, Stereo* demo, ULogger::Logger &log);
};
