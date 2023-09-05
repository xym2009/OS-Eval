#include "CommonHead.h"
#include "matchFunc.h"
using namespace std;


int main(int argc, char** argv)
{
    string parafile = "config.txt"; 
	ifstream infile; infile.open(parafile.data()); 
	assert(infile.is_open());  
	string s; int TemplateSize = 200; double tt = 3.0; int test1 = -1; int test2 = -1; double DEMrate = 0; double scale = 1.0;
    string method = "OSPC"; double sigma = 2.0; int dilation = 1; int use_dsm = 0; int testN = 1; int use_ransac = 0; int finematch = 0;
	string mode = "RE";
	while (getline(infile, s))
	{
		vector<string> ss = split_string(s, ":");
		if (ss[0] == "TemplateSize") TemplateSize = atoi(ss[1].c_str());
		if (ss[0] == "threshold") tt = atof(ss[1].c_str());
        if (ss[0] == "Method")
        {
            method = ss[1];
            size_t n = method.find_last_not_of("\r\n\t");
            if (n != string::npos){
                method.erase(n + 1, method.size() - n);
            }
        }
		if (ss[0] == "mode")
        {
            mode = ss[1];
            size_t n = mode.find_last_not_of("\r\n\t");
            if (n != string::npos){
                mode.erase(n + 1, mode.size() - n);
            }
        }
        if (ss[0] == "reference") test1 = atoi(ss[1].c_str());
        if (ss[0] == "sensed") test2 = atoi(ss[1].c_str());
        if (ss[0] == "sigma") sigma = atof(ss[1].c_str());
        if (ss[0] == "dilation") dilation = atoi(ss[1].c_str());
        if (ss[0] == "use_dsm") use_dsm = atoi(ss[1].c_str());
        if (ss[0] == "use_ransac") use_ransac = atoi(ss[1].c_str());
        if (ss[0] == "testN") testN = atoi(ss[1].c_str());
        if (ss[0] == "DEMrate") DEMrate = atof(ss[1].c_str());
        if (ss[0] == "scale") scale = atof(ss[1].c_str());
		if (ss[0] == "finematch") finematch = atof(ss[1].c_str());
	}
	ULogger::Logger log; log.PrintToConsole = true; log.WriteWithTime = true;
	string logpath;
	
	#ifdef WIN32
		struct tm tm; SYSTEMTIME wtm;GetLocalTime(&wtm);
		tm.tm_year = wtm.wYear;tm.tm_mon = wtm.wMonth;tm.tm_mday = wtm.wDay;tm.tm_hour = wtm.wHour;tm.tm_min = wtm.wMinute;tm.tm_sec = wtm.wSecond;
		logpath = method + "-" + to_string(tm.tm_year) + "-" + to_string(tm.tm_mon) + "-" + to_string(tm.tm_mday) + "-" + 
			to_string(tm.tm_hour) + "-" + to_string(tm.tm_min) + "-" + to_string(tm.tm_sec) + ".log"; 
	#else
		time_t t; struct tm *tm; char buf2[64]; time(&t); tm = localtime(&t);
		logpath = method + "-" + to_string(tm->tm_year+1900) + "-" + to_string(tm->tm_mon+1) + "-" + to_string(tm->tm_mday) + "-" + 
			to_string(tm->tm_hour) + "-" + to_string(tm->tm_min) + "-" + to_string(tm->tm_sec) + ".log"; 
	#endif	
	
	log.Open(logpath.c_str()); log << " MainFunc: Start Matching Process by XYM!" << log.End;
	log << " MainFunc: Method:" << method << ";mode:" << mode << ";finematch:" << finematch << ";TemplateSize:" << TemplateSize << ";threshold:" << tt << ";use_dsm:" << use_dsm << ";use_ransac:" << use_ransac << ";DEMrate:" << DEMrate << log.End;
	infile.close();
    
	if(mode=="RE")
	{
		vector<string> optN(11); vector<string> sarN(13);
		optN[0] = "18APR24174447-P2AS-013202438020_01_P001L1";
		optN[1] = "18APR24174549-P2AS-013202438010_01_P001L1";
		optN[2] = "19JUN05173227-P2AS-013202438070_01_P001L1";
		optN[3] = "19JUN05173337-P2AS-013202438060_01_P001L1";
		optN[4] = "19NOV09172335-P2AS-013202438040_01_P001L1";
		optN[5] = "19NOV23174448-P2AS-013202438050_01_P001L1";
		optN[6] = "19OCT28173153-P2AS-013202438030_01_P001L1";
		optN[7] = "19SEP13172221-P2AS-011988822010_01_P002L1";
		optN[8] = "19SEP13172312-P2AS-011988822010_01_P002L1";
		optN[9] = "SV1-02_20170822_L1B0000899817_1012000210040001_01-PAN";
		optN[10] = "SV1-03_20181207_L1B0000899862_1012000210060001_01-PAN";
		sarN[0] = "GF3_KAS_SL_015323_W95.9_N41.3_20190709_L1B_HH_L10004102699";
		sarN[1] = "GF3_KAS_SL_015331_W95.9_N41.3_20190709_L1B_HH_L10004105495";
		sarN[2] = "GF3_KAS_SL_015396_W95.9_N41.3_20190714_L1B_HH_L10004114146";
		sarN[3] = "GF3_KAS_SL_015432_W95.9_N41.3_20190716_L1B_HH_L10004118720";
		sarN[4] = "GF3_KAS_SL_015504_W95.9_N41.3_20190721_L1B_HH_L10004131104";
		sarN[5] = "GF3_KAS_SL_015568_W95.9_N41.3_20190726_L1B_HH_L10004141210";
		sarN[6] = "GF3_KAS_SL_015576_W95.9_N41.3_20190726_L1B_HH_L10004142416";
		sarN[7] = "GF3_KAS_SL_015641_W95.9_N41.3_20190731_L1B_HH_L10004152322";
		sarN[8] = "GF3_KAS_SL_015712_W95.9_N41.3_20190805_L1B_HH_L10004160896";
		sarN[9] = "GF3_KAS_SL_015749_W95.9_N41.3_20190807_L1B_HH_L10004166478";
		sarN[10] = "GF3_KAS_SL_916996_W95.9_N41.3_20191102_L1B_HH_L10004355418";
		sarN[11] = "GF3_KAS_SL_016159_W95.9_N41.3_20190905_L1B_HH_L10004226574";
		sarN[12] = "GF3_MYN_SL_015467_W95.9_N41.3_20190719_L1B_HH_L10004125928";

		for(int k=0;k<optN.size();k++)
		{
			for(int kk=0;kk<sarN.size();kk++)
			{
				for(int kkk=1; kkk<=9; kkk++)
				{
					//if(k!=test1||kk!=test2||kkk!=testN) continue;
					string optTIF = "/mnt/OSEval/Relative-dataset/OPT+RPC+GT/"+optN[k]+"-" + to_string(kkk) +".tiff";
					string optRPC = "/mnt/OSEval/Relative-dataset/OPT+RPC+GT/"+optN[k]+"-" + to_string(kkk) +".rpb";
					//if(IsFileExist(optRPC)==false) optRPC = "/DiskArray/Data/OSB/BenchMark/"+optN[k]+"-" + to_string(kkk) +".rpb";
					if(finematch==1) 
					{
						if(use_dsm==1)
							optRPC = "/DiskArray/Data/OSB/ASSL2DSM/Relative-"+to_string(k+1)+"-"+to_string(kk+1)+"-"+to_string(kkk)+"/"+optN[k]+"-"+to_string(kkk) +".rpb";
						else
							optRPC = "/DiskArray/Data/OSB/ASSL2/Relative-"+to_string(k+1)+"-"+to_string(kk+1)+"-"+to_string(kkk)+"/"+optN[k]+"-"+to_string(kkk) +".rpb";
					}
					string sarTIF = "/mnt/OSEval/Relative-dataset/SAR+RPC+GT/"+sarN[kk]+"-" + to_string(kkk) +".tiff";
					string sarRPC = "/mnt/OSEval/Relative-dataset/SAR+RPC+GT/"+sarN[kk]+"-" + to_string(kkk) +".rpb";
					string optGT =  "/mnt/OSEval/Relative-dataset/OPT+RPC+GT/"+optN[k]+"-" + to_string(kkk) +"-GT.txt";
					string sarGT =  "/mnt/OSEval/Relative-dataset/SAR+RPC+GT/"+sarN[kk]+"-" + to_string(kkk) +"-GT.txt";
					string dempath;
					if(use_dsm==1)
						dempath = "/mnt/OSEval/Relative-dataset/OMHdsm.tif";
					else
						dempath = "/mnt/OSEval/Relative-dataset/OMH_USGS10m_WGS84.tif";
					if(IsFileExist(sarRPC)==false||IsFileExist(optRPC)==false)
					{
						log << "MainFunc: No Tiff/RPC, continue!"<< log.End;
						continue;
					}
					matchFunc* demo = new matchFunc();
					demo->optTIF = optTIF;demo->optRPC = optRPC;demo->optGT = optGT;
					demo->sarTIF = sarTIF;demo->sarRPC = sarRPC;demo->sarGT = sarGT;
					demo->dempath = dempath;
					demo->th = tt;
					demo->use_dsm = use_dsm;					
					if(use_dsm==true)
					{
						demo->EvalRE(method + "-dsm", log);
					}
					else
					{
						demo->EvalRE(method, log);
					}
					delete demo;
				}
			}
		}
	}
	
	if(mode=="AE")
	{
		vector<string> optN(1); vector<string> sarN;
		optN[0] = "ZZG-L20";
		sarN.push_back("GF3_KRN_SL_010497_W158.0_N21.4_20180808_L1B_HH_L10003377724");
		sarN.push_back("GF3_KRN_SL_010669_W157.9_N21.4_20180820_L1B_HH_L10003401490");
		sarN.push_back("GF3_KRN_SL_020284_W158.0_N21.4_20200617_L1B_HH_L10004876483");
		sarN.push_back("GF3_KRN_SL_020398_W157.9_N21.4_20200624_L1B_HH_L10004889000");
		sarN.push_back("GF3_KRN_SL_027880_W158.0_N21.4_20211126_L1B_HH_L10000000001");
		sarN.push_back("GF3_KRN_SL_028053_W157.9_N21.4_20211208_L1B_HH_L10000000001");
		sarN.push_back("GF3_KRN_SL_028125_W157.9_N21.4_20211213_L1B_HH_L10000000001");
		sarN.push_back("GF3_KRN_SL_028644_W157.9_N21.4_20220118_L1B_HH_L10000000001");
		sarN.push_back("GF3_KRN_SL_029235_W158.0_N21.4_20220228_L1B_HH_L10000000001");
		sarN.push_back("GF3_KRN_SL_029430_W158.0_N21.4_20220313_L1B_HH_L10000000001");
		sarN.push_back("GF3_KRN_SL_029848_W158.0_N21.4_20220411_L1B_HH_L10000000001");
		sarN.push_back("GF3_KRN_SL_919744_W158.0_N21.4_20200510_L1B_HH_L10004794605");
		sarN.push_back("GF3_MYN_SL_021144_W158.0_N21.4_20200815_L1B_HH_L10004993692");
		sarN.push_back("GF3_MYN_SL_025670_W157.9_N21.4_20210625_L1B_HH_L10000000001");
		sarN.push_back("GF3_MYN_SL_026924_W158.0_N21.4_20210920_L1B_HH_L10000000001");
		sarN.push_back("GF3_MYN_SL_027349_W158.0_N21.4_20211019_L1B_HH_L10000000001");
		sarN.push_back("GF3_MYN_SL_027414_W158.0_N21.4_20211024_L1B_HH_L10000000001");
		sarN.push_back("GF3_MYN_SL_028841_W157.9_N21.4_20220131_L1B_HH_L10000000001");
		sarN.push_back("GF3_MYN_SL_030268_W158.0_N21.4_20220510_L1B_HH_L10000000001");
		
		for(int k=0;k<optN.size();k++) 
		{
			for(int kk=0;kk<sarN.size();kk++)
			{
				for(int kkk=2; kkk<=8; kkk++)
				{ 
					//if(kkk!=testN) continue;
					string optTIF = "/mnt/OSEval/Absolute-dataset/Google+GT/"+optN[k]+"-" + to_string(kkk) +".tiff";
					string sarTIF = "/mnt/OSEval/Absolute-dataset/SLC+RPC+GT/"+sarN[kk]+"-" + to_string(kkk) +".tiff";
					string sarRPC = "/mnt/OSEval/Absolute-dataset/SLC+RPC+GT/"+sarN[kk]+"-" + to_string(kkk) +".rpb";
					if(finematch==1) 
					{
						if(use_dsm==1)
							sarRPC = "/DiskArray/Data/OSB/ASSL2DSM/Absolute-1-"+to_string(kk+1)+"-"+to_string(kkk)+"/"+sarN[kk]+"-" + to_string(kkk) +".rpb";
						else
							sarRPC = "/DiskArray/Data/OSB/ASSL2/Absolute-1-"+to_string(kk+1)+"-"+to_string(kkk)+"/"+sarN[kk]+"-" + to_string(kkk) +".rpb";
					}
					string optGT =  "/mnt/OSEval/Absolute-dataset/Google+GT/"+optN[k]+"-" + to_string(kkk) +"-GT.txt";
					string sarGT =  "/mnt/OSEval/Absolute-dataset/SLC+RPC+GT/"+sarN[kk]+"-" + to_string(kkk) +"-GT.txt";
					string dempath;
					if(use_dsm==1)
						dempath = "/mnt/OSEval/Absolute-dataset/ZZG_lidar_total.tif";
					else
						dempath = "/mnt/OSEval/Absolute-dataset/ZZG_USGS10m_wgs.tif";
					if(IsFileExist(sarRPC)==false)
					{
						log << "MainFunc: No Tiff/RPC, continue!"<< log.End;
						continue;
					}
					matchFunc* demo = new matchFunc();
					demo->optTIF = optTIF;demo->optGT = optGT;
					demo->sarTIF = sarTIF;demo->sarRPC = sarRPC; demo->sarGT = sarGT;
					demo->dempath = dempath;
					demo->th = tt;
					demo->use_dsm = use_dsm;
					if(use_dsm==1)
					{
						demo->EvalAE_DSM(method + "-dsm", log);
					}
					else
					{
						demo->EvalAE(method, log);
					}
					delete demo;
				}
			}
		}
		
	}
	
	log<<" Finish process!"<<log.End;
    return 0;
}

