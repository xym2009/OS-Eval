#include "matchFunc.h"

matchFunc::matchFunc()
{}

matchFunc::~matchFunc()
{}

/*void removedupes(std::vector<Point2f> & vec1, std::vector<Point2f> & vec2)
{
    std::unordered_set<Point2f> pointset;

    auto itor1 = vec1.begin(); auto itor2 = vec2.begin();
    while (itor1 != vec1.end())
    {
        if (pointset.find(*itor1) != pointset.end())
        {
            itor1 = vec1.erase(itor1); itor2 = vec2.erase(itor2);
        }
        else
        {
            pointset.insert(*itor1);
            itor1++; itor2++;
        }
    }
}*/

static Mat LMS(const Mat& match1_xy, const Mat& match2_xy, string model, float& rmse)
{
	const int N = match1_xy.rows;
	Mat match2_xy_trans, match1_xy_trans;
	transpose(match1_xy, match1_xy_trans);
	transpose(match2_xy, match2_xy_trans);
	Mat change = Mat::zeros(3, 3, CV_32FC1);
	Mat A = Mat::zeros(2 * N, 6, CV_32FC1);
	for (int i = 0; i < N; ++i)
	{
		A.at<float>(2 * i, 0) = match1_xy.at<float>(i, 0);//x
		A.at<float>(2 * i, 1) = match1_xy.at<float>(i, 1);//y
		A.at<float>(2 * i, 4) = 1.f;
		A.at<float>(2 * i + 1, 2) = match1_xy.at<float>(i, 0);
		A.at<float>(2 * i + 1, 3) = match1_xy.at<float>(i, 1);
		A.at<float>(2 * i + 1, 5) = 1.f;
	}
	Mat B(2 * N, 1, CV_32FC1);
	for (int i = 0; i < N; ++i)
	{
		B.at<float>(2 * i, 0) = match2_xy.at<float>(i, 0);//x
		B.at<float>(2 * i + 1, 0) = match2_xy.at<float>(i, 1);//y
	}
	if (model == string("affine"))
	{
		Vec6f values;
		solve(A, B, values, DECOMP_QR);
		change = (Mat_<float>(3, 3) << values(0), values(1), values(4),
			values(2), values(3), values(5),
			+0.0f, +0.0f, 1.0f);

		Mat temp_1 = change(Range(0, 2), Range(0, 2));
		Mat temp_2 = change(Range(0, 2), Range(2, 3));
		Mat match1_xy_change = temp_1 * match1_xy_trans + repeat(temp_2, 1, N);
		Mat diff = match1_xy_change - match2_xy_trans;
		pow(diff, 2.f, diff);
		rmse = (float)sqrt(sum(diff)(0) * 1.0 / N);
	}
	else if (model == string("perspective"))
	{
		Mat A2(2 * N, 2, CV_32FC1);
		for (int i = 0; i < N; ++i)
		{
			A2.at<float>(2 * i, 0) = match1_xy.at<float>(i, 0) * match2_xy.at<float>(i, 0) * (-1.f);
			A2.at<float>(2 * i, 1) = match1_xy.at<float>(i, 0) * match2_xy.at<float>(i, 1) * (-1.f);
			A2.at<float>(2 * i + 1, 0) = match1_xy.at<float>(i, 1) * match2_xy.at<float>(i, 0) * (-1.f);
			A2.at<float>(2 * i + 1, 1) = match1_xy.at<float>(i, 1) * match2_xy.at<float>(i, 1) * (-1.f);
		}
		Mat A1(2 * N, 8, CV_32FC1);
		A.copyTo(A1(Range::all(), Range(0, 6)));
		A2.copyTo(A1(Range::all(), Range(6, 8)));
		Mat values; solve(A1, B, values, DECOMP_QR);
		change.at<float>(0, 0) = values.at<float>(0);
		change.at<float>(0, 1) = values.at<float>(1);
		change.at<float>(0, 2) = values.at<float>(4);
		change.at<float>(1, 0) = values.at<float>(2);
		change.at<float>(1, 1) = values.at<float>(3);
		change.at<float>(1, 2) = values.at<float>(5);
		change.at<float>(2, 0) = values.at<float>(6);
		change.at<float>(2, 1) = values.at<float>(7);
		change.at<float>(2, 2) = 1.f;
		Mat temp1 = Mat::ones(1, N, CV_32FC1);
		Mat temp2(3, N, CV_32FC1);
		match1_xy_trans.copyTo(temp2(Range(0, 2), Range::all()));
		temp1.copyTo(temp2(Range(2, 3), Range::all()));
		Mat match1_xy_change = change * temp2;
		Mat match1_xy_change_12 = match1_xy_change(Range(0, 2), Range::all());
		float* temp_ptr = match1_xy_change.ptr<float>(2);
		for (int i = 0; i < N; ++i)
		{
			float div_temp = temp_ptr[i];
			match1_xy_change_12.at<float>(0, i) = match1_xy_change_12.at<float>(0, i) / div_temp;
			match1_xy_change_12.at<float>(1, i) = match1_xy_change_12.at<float>(1, i) / div_temp;
		}
		Mat diff = match1_xy_change_12 - match2_xy_trans;
		pow(diff, 2, diff);
		rmse = (float)sqrt(sum(diff)(0) * 1.0 / N);
	}
	else if (model == string("similarity"))
	{
		/*[x, y, 1, 0;
		  y, -x, 0, 1] * [a, b, c, d]'=[u,v]*/
		Mat A3(2 * N, 4, CV_32FC1);
		for (int i = 0; i < N; ++i)
		{
			A3.at<float>(2 * i, 0) = match1_xy.at<float>(i, 0);
			A3.at<float>(2 * i, 1) = match1_xy.at<float>(i, 1);
			A3.at<float>(2 * i, 2) = 1.f;
			A3.at<float>(2 * i, 3) = 0.f;
			A3.at<float>(2 * i + 1, 0) = match1_xy.at<float>(i, 1);
			A3.at<float>(2 * i + 1, 1) = match1_xy.at<float>(i, 0) * (-1.f);
			A3.at<float>(2 * i + 1, 2) = 0.f;
			A3.at<float>(2 * i + 1, 3) = 1.f;
		}
		Vec4f values; solve(A3, B, values, DECOMP_QR);
		change = (Mat_<float>(3, 3) << values(0), values(1), values(2),
			values(1) * (-1.0f), values(0), values(3),
			+0.f, +0.f, 1.f);

		Mat temp_1 = change(Range(0, 2), Range(0, 2));
		Mat temp_2 = change(Range(0, 2), Range(2, 3));
		Mat match1_xy_change = temp_1 * match1_xy_trans + repeat(temp_2, 1, N);
		Mat diff = match1_xy_change - match2_xy_trans;
		pow(diff, 2, diff);
		rmse = (float)sqrt(sum(diff)(0) * 1.0 / N);
	}
	return change;
}

Mat ransac(vector<Point2f> Lxy, vector<Point2f> Rxy, string model, float threshold, vector<bool>& inliers, float& rmse)
{
	int n; size_t max_iteration, iterations; int N = Lxy.size(); //srand(time(0));
	if (model == string("similarity")) {
		n = 2;
		max_iteration = N * (N - 1) / 2;
	}
	else if (model == string("affine")) {
		n = 3;
		max_iteration = N * (N - 1) * (N - 2) / (2 * 3);
	}
	else if (model == string("perspective")) {
		n = 4;
		max_iteration = N * (N - 1) * (N - 2) / (2 * 3);
	}
	if (max_iteration > 10000)
		iterations = 10000;
	else
		iterations = max_iteration;
	
	Mat arr_1(3, (int)N, CV_32FC1); Mat arr_2(3, (int)N, CV_32FC1);
	float* p10 = arr_1.ptr<float>(0), * p11 = arr_1.ptr<float>(1), * p12 = arr_1.ptr<float>(2);
	float* p20 = arr_2.ptr<float>(0), * p21 = arr_2.ptr<float>(1), * p22 = arr_2.ptr<float>(2);
	for (size_t i = 0; i < N; ++i)
	{
		p10[i] = Lxy[i].x; p11[i] = Lxy[i].y; p12[i] = 1.f;
		p20[i] = Rxy[i].x; p21[i] = Rxy[i].y; p22[i] = 1.f;
	}
	Mat rand_mat(1, n, CV_32SC1); int* p = rand_mat.ptr<int>(0);
	Mat sub_arr1(n, 2, CV_32FC1); Mat sub_arr2(n, 2, CV_32FC1);
	Mat T; int most_consensus_num = 0;
	vector<bool> right; right.resize(N); inliers.resize(N);
	for (size_t i = 0; i < iterations; ++i)
	{
		while (1)
		{
			randu(rand_mat, 0, double(N - 1));
			if (n == 2 && p[0] != p[1] &&
				(p10[p[0]] != p10[p[1]] || p11[p[0]] != p11[p[1]]) &&
				(p20[p[0]] != p20[p[1]] || p21[p[0]] != p21[p[1]]))
				break;

			if (n == 3 && p[0] != p[1] && p[0] != p[2] && p[1] != p[2] &&
				(p10[p[0]] != p10[p[1]] || p11[p[0]] != p11[p[1]]) &&
				(p10[p[0]] != p10[p[2]] || p11[p[0]] != p11[p[2]]) &&
				(p10[p[1]] != p10[p[2]] || p11[p[1]] != p11[p[2]]) &&
				(p20[p[0]] != p20[p[1]] || p21[p[0]] != p21[p[1]]) &&
				(p20[p[0]] != p20[p[2]] || p21[p[0]] != p21[p[2]]) &&
				(p20[p[1]] != p20[p[2]] || p21[p[1]] != p21[p[2]]))
				break;

			if (n == 4 && p[0] != p[1] && p[0] != p[2] && p[0] != p[3] &&
				p[1] != p[2] && p[1] != p[3] && p[2] != p[3] &&
				(p10[p[0]] != p10[p[1]] || p11[p[0]] != p11[p[1]]) &&
				(p10[p[0]] != p10[p[2]] || p11[p[0]] != p11[p[2]]) &&
				(p10[p[0]] != p10[p[3]] || p11[p[0]] != p11[p[3]]) &&
				(p10[p[1]] != p10[p[2]] || p11[p[1]] != p11[p[2]]) &&
				(p10[p[1]] != p10[p[3]] || p11[p[1]] != p11[p[3]]) &&
				(p10[p[2]] != p10[p[3]] || p11[p[2]] != p11[p[3]]) &&
				(p20[p[0]] != p20[p[1]] || p21[p[0]] != p21[p[1]]) &&
				(p20[p[0]] != p20[p[2]] || p21[p[0]] != p21[p[2]]) &&
				(p20[p[0]] != p20[p[3]] || p21[p[0]] != p21[p[3]]) &&
				(p20[p[1]] != p20[p[2]] || p21[p[1]] != p21[p[2]]) &&
				(p20[p[1]] != p20[p[3]] || p21[p[1]] != p21[p[3]]) &&
				(p20[p[2]] != p20[p[3]] || p21[p[2]] != p21[p[3]]))
				break;
		}
		for (int i = 0; i < n; ++i)
		{
			sub_arr1.at<float>(i, 0) = p10[p[i]]; sub_arr1.at<float>(i, 1) = p11[p[i]];
			sub_arr2.at<float>(i, 0) = p20[p[i]]; sub_arr2.at<float>(i, 1) = p21[p[i]];
		}
		T = LMS(sub_arr1, sub_arr2, model, rmse);
		int consensus_num = 0;
		if (model == string("perspective"))
		{
			Mat match1_xy_change = T * arr_1;
			Mat match1_xy_change_12 = match1_xy_change(Range(0, 2), Range::all());
			float* temp_ptr = match1_xy_change.ptr<float>(2);
			for (size_t i = 0; i < N; ++i)
			{
				float div_temp = temp_ptr[i];
				match1_xy_change_12.at<float>(0, (int)i) = match1_xy_change_12.at<float>(0, (int)i) / div_temp;
				match1_xy_change_12.at<float>(1, (int)i) = match1_xy_change_12.at<float>(1, (int)i) / div_temp;
			}
			Mat diff = match1_xy_change_12 - arr_2(Range(0, 2), Range::all());
			pow(diff, 2, diff);
			Mat add = diff(Range(0, 1), Range::all()) + diff(Range(1, 2), Range::all());
			sqrt(add, add);
			float* p_add = add.ptr<float>(0);
			for (size_t i = 0; i < N; ++i)
			{
				if (p_add[i] < threshold) {
					right[i] = true;
					++consensus_num;
				}
				else
					right[i] = false;
			}
			match1_xy_change.release(); match1_xy_change_12.release(); diff.release(); add.release();
		}
		else if (model == string("affine") || model == string("similarity"))
		{
			Mat match1_xy_change = T * arr_1;
			Mat diff = match1_xy_change - arr_2;
			pow(diff, 2, diff);
			Mat add = diff(Range(0, 1), Range::all()) + diff(Range(1, 2), Range::all());
			sqrt(add, add);
			float* p_add = add.ptr<float>(0);
			for (size_t i = 0; i < N; ++i)
			{
				if (p_add[i] < threshold) {
					right[i] = true;
					++consensus_num;
				}
				else
					right[i] = false;
			}
			match1_xy_change.release(); diff.release(); add.release();
		}
		if (consensus_num > most_consensus_num) {
			most_consensus_num = consensus_num;
			for (size_t i = 0; i < N; ++i)
				inliers[i] = right[i];
		}
	}
	for (size_t i = 0; i < N - 1; ++i)
	{
		for (size_t j = i + 1; j < N; ++j)
		{
			if (inliers[i] && inliers[j])
			{
				if (p10[i] == p10[j] && p11[i] == p11[j])
				{
					inliers[j] = false;
					--most_consensus_num;
				}
				else if (p20[i] == p20[j] && p21[i] == p21[j])
				{
					inliers[j] = false;
					--most_consensus_num;
				}
			}
		}
	}
	Mat consensus_arr1(most_consensus_num, 2, CV_32FC1); Mat consensus_arr2(most_consensus_num, 2, CV_32FC1);
	int k = 0;
	for (size_t i = 0; i < N; ++i)
	{
		if (inliers[i])
		{
			consensus_arr1.at<float>(k, 0) = p10[i];
			consensus_arr1.at<float>(k, 1) = p11[i];
			consensus_arr2.at<float>(k, 0) = p20[i];
			consensus_arr2.at<float>(k, 1) = p21[i];
			++k;
		}
	}
	T = LMS(consensus_arr1, consensus_arr2, model, rmse);
	consensus_arr1.release(); consensus_arr2.release(); arr_1.release(); arr_2.release(); sub_arr1.release(); sub_arr2.release();
	return T;
}


void matchFunc::EvalRE(string method, ULogger::Logger &log)
{
	vector<string> v_str = split_string(optTIF, "/"); name1 = v_str[v_str.size() - 1];
	vector<string> v_str2 = split_string(sarTIF, "/"); name2 = v_str2[v_str2.size() - 1];
	log << " " << method << ": Start eval " << name1 << " " << name2 << log.End;
	string linkpath = "tempLink/" + method + "/" + name1 + "_Link_" + name2 + ".txt";
	ifstream data; data.open(linkpath); string data_temp;
	if (!data || data.eof())
	{
		log << " open file failed " << linkpath << "-- file is empty!" << log.End;
	}
	data >> data_temp;
	if (atoi(data_temp.c_str()) == 0){
		log << " No matches, continue!" << linkpath << log.End;
		return;
	}
	vector<Point2f> lxy, rxy; vector<Point2f> gx, gy;
	while (!data.eof())
	{
		Pt_Info_Match pti1, pti2;
		data >> data_temp;
		if (data.fail() || (data_temp == "end"))
			continue;
		data >> data_temp;			pti1.imgx = atof(data_temp.c_str());
		data >> data_temp;			pti1.imgy = atof(data_temp.c_str());
		lxy.push_back(Point2f(pti1.imgx,pti1.imgy));
		data >> data_temp;			pti2.imgx = atof(data_temp.c_str());
		data >> data_temp;			pti2.imgy = atof(data_temp.c_str());
		rxy.push_back(Point2f(pti2.imgx,pti2.imgy));
		data >> data_temp;
		pti1.PiPeiDu = atof(data_temp.c_str());			pti2.PiPeiDu = atof(data_temp.c_str());
		data >> data_temp;			pti1.geox = atof(data_temp.c_str());
		data >> data_temp;			pti1.geoy = atof(data_temp.c_str());
		data >> data_temp;			pti1.geoz = atof(data_temp.c_str());
		gx.push_back(Point2f(pti1.geox,pti1.geoy));
		data >> data_temp;			pti2.geox = atof(data_temp.c_str());
		data >> data_temp;			pti2.geoy = atof(data_temp.c_str());
		data >> data_temp;			pti2.geoz = atof(data_temp.c_str());
		gy.push_back(Point2f(pti2.geox,pti2.geoy));
	}
	data.close();
	Stereo* demo = new Stereo(); demo->readDEMData(dempath); demo->ReadLRPC(optRPC); demo->ReadRRPC(sarRPC);
	if(method=="CFOG"||method=="OSMNet"||method=="OSMNetf"||method=="CFOGf"||method=="CFOG-dsm"||method=="CFOGf-dsm"||method=="OSMNet-dsm"||method=="OSMNetf-dsm")
	{
		//repeat 10 times and calculate avearage 
		double sumR = 0; double sumN = 0; double tpN = 0; int rN = 20; double SE; 
		for(int cn = 0; cn < rN; cn++)
		{
			vector<Point2f> ImgX, ImgY;	vector<bool> inliers; float rmse; 
			Mat homo = ransac(gx, gy, "affine", th, inliers, rmse);
			for (int kk = 0; kk < gx.size(); kk++)
			{
				if(inliers[kk])
				{
					ImgX.push_back(Point2f(lxy[kk].x,lxy[kk].y)); 
					ImgY.push_back(Point2f(rxy[kk].x,rxy[kk].y));
				}
			}
			SE = IMGGTevaluate(ImgX, ImgY, optGT, sarGT, demo, log);
			sumR += SE; sumN += (double)ImgX.size()/(double)gx.size(); tpN++;
		}
		log<<" " << method << ": Final ME:" << sumR/rN << "\tCMR " << sumN/(double) (rN) << log.End;
	}
	else if(method=="RIFT"||method=="RIFT-dsm"||method=="MatchosNet"||method=="MatchosNet-dsm")
	{
		if(gx.size()<=20) 
			log<<" " << method << ": Final ME:" << 999 << "\tCMR " << 0 << log.End;
		else
		{
			//repeat 10 times and calculate avearage 
			double sumR = 0; double sumN = 0; double tpN = 0; int rN = 20; double SE; 
			for(int cn = 0; cn < rN; cn++)
			{
				vector<Point2f> ImgX, ImgY;	vector<bool> inliers; float rmse; 
				Mat homo = ransac(gx, gy, "affine", th, inliers, rmse);
				for (int kk = 0; kk < gx.size(); kk++)
				{
					if(inliers[kk])
					{
						ImgX.push_back(Point2f(lxy[kk].x,lxy[kk].y)); 
						ImgY.push_back(Point2f(rxy[kk].x,rxy[kk].y));
					}
				}
				SE = IMGGTevaluate(ImgX, ImgY, optGT, sarGT, demo, log);
				sumR += SE; sumN += (double)ImgX.size()/(double)gx.size(); tpN++;
			}
			log<<" " << method << ": Final ME:" << sumR/tpN << "\tCMR " << sumN/(double) (tpN) << log.End;
		}		
	}
	else 
	{
		//removedupes(lxy, rxy);
		double SE = IMGGTevaluate(lxy, rxy, optGT, sarGT, demo, log);
		log<<" " << method << ": Final ME:" << SE << "\tNCM " << (int) lxy.size() << log.End;
	}
	delete demo;
}

void matchFunc::EvalAE(string method, ULogger::Logger &log)
{
	vector<string> v_str = split_string(optTIF, "/"); name1 = v_str[v_str.size() - 1];
	vector<string> v_str2 = split_string(sarTIF, "/"); name2 = v_str2[v_str2.size() - 1];
	log << " " << method << ": Start match " << name1 << " " << name2 << log.End;
	string linkpath = "tempLink/" + method + "/" + name1 + "_Link_" + name2 + ".txt";
	ifstream data; data.open(linkpath); string data_temp;
	if (!data || data.eof())
	{
		log << " open file failed " << linkpath << "-- file is empty!" << log.End;
	}
	data >> data_temp;
	if (atoi(data_temp.c_str()) == 0){
		log << " " << method << " No matches, continue!" << linkpath << log.End;
		return;
	}
	vector<Point2f> lxy, rxy; vector<Point2f> geo1, geo2;
	while (!data.eof())
	{
		Pt_Info_Match pti1, pti2;
		data >> data_temp;
		if (data.fail() || (data_temp == "end"))
			continue;
		data >> data_temp;			pti1.imgx = atof(data_temp.c_str());
		data >> data_temp;			pti1.imgy = atof(data_temp.c_str());
		lxy.push_back(Point2f(pti1.imgx,pti1.imgy));
		data >> data_temp;			pti2.imgx = atof(data_temp.c_str());
		data >> data_temp;			pti2.imgy = atof(data_temp.c_str());
		rxy.push_back(Point2f(pti2.imgx,pti2.imgy));
		data >> data_temp;
		pti1.PiPeiDu = atof(data_temp.c_str());			pti2.PiPeiDu = atof(data_temp.c_str());
		data >> data_temp;			pti1.geox = atof(data_temp.c_str());
		data >> data_temp;			pti1.geoy = atof(data_temp.c_str());
		data >> data_temp;			pti1.geoz = atof(data_temp.c_str());
		geo1.push_back(Point2f(pti1.geox,pti1.geoy));
		data >> data_temp;			pti2.geox = atof(data_temp.c_str());
		data >> data_temp;			pti2.geoy = atof(data_temp.c_str());
		data >> data_temp;			pti2.geoz = atof(data_temp.c_str());
		geo2.push_back(Point2f(pti2.geox,pti2.geoy));
	}
	data.close();
	ifstream infile; infile.open(optGT.data()); assert(infile.is_open()); string s; vector<Point2f> gxy;
	while (getline(infile, s))
	{
		vector<string> ss = split_string(s, "\t");
		int ind = atoi(ss[0].c_str());
		float gx = atof(ss[1].c_str());
		float gy = atof(ss[2].c_str());
		gxy.push_back(Point2f(gx, gy));
	}
	ifstream infile2; infile2.open(sarGT.data()); vector<Point2f> gxy2;
	while (getline(infile2, s))
	{
		vector<string> ss = split_string(s, "\t");
		int ind = atoi(ss[0].c_str());
		float gx = atof(ss[1].c_str());
		float gy = atof(ss[2].c_str());
		gxy2.push_back(Point2f(gx, gy));
	}
	Stereo* demo = new Stereo(); demo->readDEMData(dempath); demo->ReadRRPC(sarRPC);
	GDALDataset* optSRCDataset = (GDALDataset*)GDALOpen(optTIF.c_str(), GA_ReadOnly);
	double optSRCTrans[6]; optSRCDataset->GetGeoTransform(optSRCTrans); 
	GDALClose((GDALDatasetH)optSRCDataset); 
	auto pos1 = optGT.find_last_of("/"); string optf = optGT.substr(0, pos1);
	auto pos11 = optf.find_last_of("/"); string optff = optf.substr(0, pos11);
	vector<string> str1 = split_string(name1, "."); 
	vector<string> str2 = split_string(name2, ".");
	string optL2 = optff + "/Processed/OPT/" + str1[0]+"-resize.tiff";
	string sarL2 = optff + "/Processed/SAR-ORG-DEM/";
	for(int st=0; st<str2.size()-1; st++)
	{
		sarL2+=str2[st]; 
		if(st!=(str2.size()-2)) sarL2+=".";
	}
	sarL2+="-ORG.tiff";
	GDALDataset* optDataset = (GDALDataset*)GDALOpen(optL2.c_str(), GA_ReadOnly);
	double optForm2[6]; optDataset->GetGeoTransform(optForm2); GDALClose((GDALDatasetH)optDataset); 
	GDALDataset* sarDataset = (GDALDataset*)GDALOpen(sarL2.c_str(), GA_ReadOnly);
	double sarForm2[6]; sarDataset->GetGeoTransform(sarForm2); GDALClose((GDALDatasetH)sarDataset); 
	vector<Point2f> ImgX, ImgY; 
	for (int ks = 0; ks < geo1.size(); ks++)
	{
		double gx1, gy1, gx2, gy2;
		demo->GeoToImgPointDouble(geo1[ks].x, geo1[ks].y, gx1, gy1, optForm2);
		demo->GeoToImgPointDouble(geo2[ks].x, geo2[ks].y, gx2, gy2, sarForm2);
		ImgX.push_back(Point2f(gx1, gy1)); ImgY.push_back(Point2f(gx2, gy2));
	}
	if(method=="CFOG"||method=="OSMNet"||method=="RIFT"||method=="CFOGf"||method=="OSMNetf"||method=="MatchosNet")
	{
		//repeat 10 times and calculate avearage 
		double sumR = 0; double sumN = 0; int rN = 20;
		for(int cn = 0; cn < rN; cn++)
		{
			vector<bool> inliers; float rmse; Mat homo = ransac(ImgX, ImgY, "affine", th, inliers, rmse); 
			int sum=accumulate(inliers.begin(),inliers.end(),0); double sumE = 0; int count = 0;
			for (int ks = 0; ks < min(gxy.size(), gxy2.size()); ks++)
			{
				double glon, glat, gh, glon2, glat2, ggx, ggy, ggx2, ggy2, dx, dy, demx, demy, demh;
				float gx = gxy[ks].x; float gy = gxy[ks].y;	float gx3 = gxy2[ks].x; float gy3 = gxy2[ks].y;
				if (gx == 0 || gy == 0 || gx3 == 0 || gy3 == 0) continue;
				demo->img2Geopoint(gx, gy, glon, glat, optSRCTrans); demo->GeoToImgPointDouble(glon, glat, ggx, ggy, optForm2);
				Mat arr_1(3, 1, CV_32FC1); arr_1.at<float>(0,0) = ggx; arr_1.at<float>(1,0) = ggy; arr_1.at<float>(2,0) = 1;
				Mat match2_xy_change = homo * arr_1; float gx_new = match2_xy_change.at<float>(0,0); float gy_new = match2_xy_change.at<float>(1,0); 
				demo->img2Geopoint(gx_new, gy_new, glon2, glat2, sarForm2);
				demo->GeoToImgPointDouble(glon2, glat2, demx, demy, demo->_geotransform);
				demo->heightBilinearInter(demh, demx, demy, demo->demwidth, demo->demheight, demo->demdata);
				demo->Geo2img(glon2, glat2, ggx2, ggy2, demh, 1);
				dx = ggx2 - gx3; dy = ggy2 - gy3; sumE+=sqrt(dx*dx+dy*dy); count++;
			}
			sumR += sumE/count;	sumN += (double)sum/ImgX.size(); homo.release();
			log<<" " << cn << "\t" << sumE/count << "\t" << sum << log.End;
		}
		log<<" " << method << ": Final ME:" << sumR/rN << "\tNCM " << sumN/rN << log.End;
	}
	else 
	{
		//removedupes(ImgX, ImgY); 
		int n = ImgX.size(); float rmse1; Mat match1_xy(n, 2, CV_32FC1); Mat match2_xy(n, 2, CV_32FC1);
		for (int i = 0; i < n; ++i)
		{
			match1_xy.at<float>(i, 0) = ImgX[i].x; match1_xy.at<float>(i, 1) = ImgX[i].y;
			match2_xy.at<float>(i, 0) = ImgY[i].x; match2_xy.at<float>(i, 1) = ImgY[i].y;
		}
		Mat homo = LMS(match1_xy, match2_xy, "affine", rmse1); double sumE = 0; int count = 0;
		for (int ks = 0; ks < min(gxy.size(), gxy2.size()); ks++)
		{
			double glon, glat, gh, glon2, glat2, ggx, ggy, ggx2, ggy2, dx, dy, demx, demy, demh;
			float gx = gxy[ks].x; float gy = gxy[ks].y;	float gx3 = gxy2[ks].x; float gy3 = gxy2[ks].y;
			if (gx == 0 || gy == 0 || gx3 == 0 || gy3 == 0) continue;
			demo->img2Geopoint(gx, gy, glon, glat, optSRCTrans); demo->GeoToImgPointDouble(glon, glat, ggx, ggy, optForm2);
			Mat arr_1(3, 1, CV_32FC1); arr_1.at<float>(0,0) = ggx; arr_1.at<float>(1,0) = ggy; arr_1.at<float>(2,0) = 1;
			Mat match2_xy_change = homo * arr_1; float gx_new = match2_xy_change.at<float>(0,0); float gy_new = match2_xy_change.at<float>(1,0); 
			demo->img2Geopoint(gx_new, gy_new, glon2, glat2, sarForm2);
			demo->GeoToImgPointDouble(glon2, glat2, demx, demy, demo->_geotransform);
			demo->heightBilinearInter(demh, demx, demy, demo->demwidth, demo->demheight, demo->demdata);
			demo->Geo2img(glon2, glat2, ggx2, ggy2, demh, 1);
			dx = ggx2 - gx3; dy = ggy2 - gy3; sumE+=sqrt(dx*dx+dy*dy); count++;
		}
		log<<" " << method << ": Final ME:" << sumE/count << "\tNCM " << (int)ImgX.size() << log.End;
	}
	delete demo;
}

void matchFunc::EvalAE_DSM(string method, ULogger::Logger &log)
{
	vector<string> v_str = split_string(optTIF, "/"); name1 = v_str[v_str.size() - 1];
	vector<string> v_str2 = split_string(sarTIF, "/"); name2 = v_str2[v_str2.size() - 1];
	log << " " << method << ": Start match " << name1 << " " << name2 << log.End;
	string linkpath = "tempLink/" + method + "/" + name1 + "_Link_" + name2 + ".txt";
	ifstream data; data.open(linkpath); string data_temp;
	if (!data || data.eof())
	{
		log << " open file failed " << linkpath << "-- file is empty!" << log.End;
	}
	data >> data_temp;
	if (atoi(data_temp.c_str()) == 0){
		log << " " << method << " No matches, continue!" << linkpath << log.End;
		return;
	}
	vector<Point2f> lxy, rxy; vector<Point2f> geo1, geo2;
	while (!data.eof())
	{
		Pt_Info_Match pti1, pti2;
		data >> data_temp;
		if (data.fail() || (data_temp == "end"))
			continue;
		data >> data_temp;			pti1.imgx = atof(data_temp.c_str());
		data >> data_temp;			pti1.imgy = atof(data_temp.c_str());
		lxy.push_back(Point2f(pti1.imgx,pti1.imgy));
		data >> data_temp;			pti2.imgx = atof(data_temp.c_str());
		data >> data_temp;			pti2.imgy = atof(data_temp.c_str());
		rxy.push_back(Point2f(pti2.imgx,pti2.imgy));
		data >> data_temp;
		pti1.PiPeiDu = atof(data_temp.c_str());			pti2.PiPeiDu = atof(data_temp.c_str());
		data >> data_temp;			pti1.geox = atof(data_temp.c_str());
		data >> data_temp;			pti1.geoy = atof(data_temp.c_str());
		data >> data_temp;			pti1.geoz = atof(data_temp.c_str());
		geo1.push_back(Point2f(pti1.geox,pti1.geoy));
		data >> data_temp;			pti2.geox = atof(data_temp.c_str());
		data >> data_temp;			pti2.geoy = atof(data_temp.c_str());
		data >> data_temp;			pti2.geoz = atof(data_temp.c_str());
		geo2.push_back(Point2f(pti2.geox,pti2.geoy));
	}
	data.close();
	ifstream infile; infile.open(optGT.data()); assert(infile.is_open()); string s; vector<Point2f> gxy;
	while (getline(infile, s))
	{
		vector<string> ss = split_string(s, "\t");
		int ind = atoi(ss[0].c_str());
		float gx = atof(ss[1].c_str());
		float gy = atof(ss[2].c_str());
		gxy.push_back(Point2f(gx, gy));
	}
	ifstream infile2; infile2.open(sarGT.data()); vector<Point2f> gxy2;
	while (getline(infile2, s))
	{
		vector<string> ss = split_string(s, "\t");
		int ind = atoi(ss[0].c_str());
		float gx = atof(ss[1].c_str());
		float gy = atof(ss[2].c_str());
		gxy2.push_back(Point2f(gx, gy));
	}
	Stereo* demo = new Stereo(); demo->readDEMData(dempath); demo->ReadRRPC(sarRPC);
	GDALDataset* optSRCDataset = (GDALDataset*)GDALOpen(optTIF.c_str(), GA_ReadOnly);
	double optSRCTrans[6]; optSRCDataset->GetGeoTransform(optSRCTrans); 
	GDALClose((GDALDatasetH)optSRCDataset); 
	auto pos1 = optGT.find_last_of("/"); string optf = optGT.substr(0, pos1);
	auto pos11 = optf.find_last_of("/"); string optff = optf.substr(0, pos11);
	vector<string> str1 = split_string(name1, "."); 
	vector<string> str2 = split_string(name2, ".");
	string optL2 = optff + "/Processed/OPT/" + str1[0]+"-resize.tiff";
	string sarL2 = optff + "/Processed/SAR-ORG-DSM/";
	for(int st=0; st<str2.size()-1; st++)
	{
		sarL2+=str2[st]; 
		if(st!=(str2.size()-2)) sarL2+=".";
	}
	sarL2+="-ORG.tiff";
	GDALDataset* optDataset = (GDALDataset*)GDALOpen(optL2.c_str(), GA_ReadOnly);
	double optForm2[6]; optDataset->GetGeoTransform(optForm2); GDALClose((GDALDatasetH)optDataset); 
	GDALDataset* sarDataset = (GDALDataset*)GDALOpen(sarL2.c_str(), GA_ReadOnly);
	double sarForm2[6]; sarDataset->GetGeoTransform(sarForm2); GDALClose((GDALDatasetH)sarDataset); 
	vector<Point2f> ImgX, ImgY; 
	for (int ks = 0; ks < geo1.size(); ks++)
	{
		double gx1, gy1, gx2, gy2;
		demo->GeoToImgPointDouble(geo1[ks].x, geo1[ks].y, gx1, gy1, optForm2);
		demo->GeoToImgPointDouble(geo2[ks].x, geo2[ks].y, gx2, gy2, sarForm2);
		ImgX.push_back(Point2f(gx1, gy1)); ImgY.push_back(Point2f(gx2, gy2));
	}
	if(method=="CFOG-dsm"||method=="OSMNet-dsm"||method=="RIFT-dsm"||method=="CFOGf-dsm"||method=="OSMNetf-dsm"||method=="MatchosNet-dsm")
	{
		//repeat 10 times and calculate avearage 
		double sumR = 0; double sumN = 0; int rN = 20;
		for(int cn = 0; cn < rN; cn++)
		{
			vector<bool> inliers; float rmse; Mat homo = ransac(ImgX, ImgY, "affine", th, inliers, rmse); 
			int sum=accumulate(inliers.begin(),inliers.end(),0); double sumE = 0; int count = 0;
			for (int ks = 0; ks < min(gxy.size(), gxy2.size()); ks++)
			{
				double glon, glat, gh, glon2, glat2, ggx, ggy, ggx2, ggy2, dx, dy, demx, demy, demh;
				float gx = gxy[ks].x; float gy = gxy[ks].y;	float gx3 = gxy2[ks].x; float gy3 = gxy2[ks].y;
				if (gx == 0 || gy == 0 || gx3 == 0 || gy3 == 0) continue;
				demo->img2Geopoint(gx, gy, glon, glat, optSRCTrans); demo->GeoToImgPointDouble(glon, glat, ggx, ggy, optForm2);
				Mat arr_1(3, 1, CV_32FC1); arr_1.at<float>(0,0) = ggx; arr_1.at<float>(1,0) = ggy; arr_1.at<float>(2,0) = 1;
				Mat match2_xy_change = homo * arr_1; float gx_new = match2_xy_change.at<float>(0,0); float gy_new = match2_xy_change.at<float>(1,0); 
				demo->img2Geopoint(gx_new, gy_new, glon2, glat2, sarForm2);
				demo->GeoToImgPointDouble(glon2, glat2, demx, demy, demo->_geotransform);
				demo->heightBilinearInter(demh, demx, demy, demo->demwidth, demo->demheight, demo->demdata);
				demo->Geo2img(glon2, glat2, ggx2, ggy2, demh, 1);
				dx = ggx2 - gx3; dy = ggy2 - gy3; sumE+=sqrt(dx*dx+dy*dy); count++;
			}
			sumR += sumE/count;	sumN += (double)sum/ImgX.size(); homo.release();
			log<<" " << cn << "\t" << sumE/count << "\t" << sum << log.End;
		}
		log<<" " << method << ": Final ME:" << sumR/rN << "\tCMR " << sumN/rN << log.End;
	}
	else 
	{
		//removedupes(ImgX, ImgY); 
		int n = ImgX.size(); float rmse1; Mat match1_xy(n, 2, CV_32FC1); Mat match2_xy(n, 2, CV_32FC1);
		for (int i = 0; i < n; ++i)
		{
			match1_xy.at<float>(i, 0) = ImgX[i].x; match1_xy.at<float>(i, 1) = ImgX[i].y;
			match2_xy.at<float>(i, 0) = ImgY[i].x; match2_xy.at<float>(i, 1) = ImgY[i].y;
		}
		Mat homo = LMS(match1_xy, match2_xy, "affine", rmse1); double sumE = 0; int count = 0;
		for (int ks = 0; ks < min(gxy.size(), gxy2.size()); ks++)
		{
			double glon, glat, gh, glon2, glat2, ggx, ggy, ggx2, ggy2, dx, dy, demx, demy, demh;
			float gx = gxy[ks].x; float gy = gxy[ks].y;	float gx3 = gxy2[ks].x; float gy3 = gxy2[ks].y;
			if (gx == 0 || gy == 0 || gx3 == 0 || gy3 == 0) continue;
			demo->img2Geopoint(gx, gy, glon, glat, optSRCTrans); demo->GeoToImgPointDouble(glon, glat, ggx, ggy, optForm2);
			Mat arr_1(3, 1, CV_32FC1); arr_1.at<float>(0,0) = ggx; arr_1.at<float>(1,0) = ggy; arr_1.at<float>(2,0) = 1;
			Mat match2_xy_change = homo * arr_1; float gx_new = match2_xy_change.at<float>(0,0); float gy_new = match2_xy_change.at<float>(1,0); 
			demo->img2Geopoint(gx_new, gy_new, glon2, glat2, sarForm2);
			demo->GeoToImgPointDouble(glon2, glat2, demx, demy, demo->_geotransform);
			demo->heightBilinearInter(demh, demx, demy, demo->demwidth, demo->demheight, demo->demdata);
			demo->Geo2img(glon2, glat2, ggx2, ggy2, demh, 1);
			dx = ggx2 - gx3; dy = ggy2 - gy3; sumE+=sqrt(dx*dx+dy*dy); count++;
		}
		log<<" " << method << ": Final ME:" << sumE/count << "\tNCM " << (int)ImgX.size() << log.End;
	}
	delete demo;
}

double matchFunc::IMGGTevaluate(vector<Point2f> ImgX, vector<Point2f> ImgY, string GT1, string GT2, Stereo* demo, ULogger::Logger &log)
{
	log<<" GTEval: IMG inliers: " << (int)ImgX.size() << "\t";
	int n = ImgX.size(); float rmse1; Mat match1_xy(n, 2, CV_32FC1); Mat match2_xy(n, 2, CV_32FC1);
	for (int i = 0; i < n; ++i)
	{
		match1_xy.at<float>(i, 0) = ImgX[i].x; match1_xy.at<float>(i, 1) = ImgX[i].y;
		match2_xy.at<float>(i, 0) = ImgY[i].x; match2_xy.at<float>(i, 1) = ImgY[i].y;
	}
	Mat T1 = LMS(match1_xy, match2_xy, "affine", rmse1); log <<"Fitting err: " <<rmse1<<"\t";
	//read paras from config.txt
    ifstream infile; infile.open(GT1.data()); assert(infile.is_open()); string s; vector<Point2f> gxy; 
    while (getline(infile, s))
    {
        vector<string> ss = split_string(s, "\t");
        int ind = atoi(ss[0].c_str());
        float gx = atof(ss[1].c_str()); 
        float gy = atof(ss[2].c_str());
        gxy.push_back(Point2f(gx,gy));
    }
    ifstream infile2; infile2.open(GT2.data()); vector<Point2f> gxy2;
    while (getline(infile2, s))
    {
        vector<string> ss = split_string(s, "\t");
        int ind = atoi(ss[0].c_str());
        float gx = atof(ss[1].c_str()); 
        float gy = atof(ss[2].c_str());
        gxy2.push_back(Point2f(gx,gy));
    }
	double sumE = 0; double count=0;
	for (int k = 0; k < min(gxy.size(), gxy2.size()); k++)
	{
		float gx = gxy[k].x; float gy = gxy[k].y;
		float gx2 = gxy2[k].x; float gy2 = gxy2[k].y;
		if (gx == 0 || gy == 0 || gx2 == 0 || gy2 == 0) continue;
		Mat arr_1(3, 1, CV_32FC1); arr_1.at<float>(0,0) = gx; arr_1.at<float>(1,0) = gy; arr_1.at<float>(2,0) = 1;
		Mat match2_xy_change = T1 * arr_1; float gx_new = match2_xy_change.at<float>(0,0); float gy_new = match2_xy_change.at<float>(1,0); 
		double plon, plat, ph, px, py, dx, dy; 
		demo->Img2GeoDEM(gx_new, gy_new, plon, plat, ph, 0); demo->Geo2img(plon, plat, px, py, ph, 1);
		dx = px - gx2; dy = py - gy2; sumE+=sqrt(dx*dx+dy*dy); count++;
		//cout<<plon<<","<<plat<<"; "<<px<<","<<py<<"\t"<<gx2<<","<<gy2<<endl;
	}
	log <<" MeanErr:"<<sumE/count<<log.End;
	return sumE/count;
}

Stereo::Stereo(){
	GDALAllRegister(); CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
}

Stereo::~Stereo(){
	if(demdata!=NULL) delete demdata;
}

int Stereo::ReadLRPC(string rpbpath)
{
	char **papszRPCInfo=GDALLoadRPBFile(rpbpath.c_str(),NULL); 
	if (papszRPCInfo == NULL)
	{
		cout<<"Left RPB read failed!"<<endl;
		exit(-1);
	}
	GDALExtractRPCInfo(papszRPCInfo, &sRPC );
	return 0;
}

int Stereo::ReadRRPC(string rpbpath)
{
	char **papszRPCInfo=GDALLoadRPBFile(rpbpath.c_str(),NULL);
	if (papszRPCInfo == NULL)
	{
		cout<<"Right RPB read failed!"<<endl;
		exit(-1);
	}
	GDALExtractRPCInfo(papszRPCInfo, &sRPC2 );
	return 0;
}

int Stereo::readDEMData(string path)
{
	GDALDataset *poDataset=(GDALDataset*) GDALOpen(path.c_str(),GA_ReadOnly);
	if (poDataset == NULL)
	{
		cout<<"Open DEM failed: "<<path<<endl;
		return -1;
	}
	GDALRasterBand *pBand=poDataset->GetRasterBand(1);
	demwidth = GDALGetRasterXSize(poDataset);
	demheight = GDALGetRasterYSize(poDataset);
	GDALGetGeoTransform(poDataset, _geotransform);
	demdata=new float[(size_t)demwidth*demheight];
	double xRes, yRes; 
	GeoToMeter(_geotransform[1], -_geotransform[5], _geotransform[3], xRes, yRes);
	if(xRes>15&&yRes>15)
	{
		short* dem16U = new short[demwidth*demheight];
		pBand->RasterIO(GF_Read,0,0, demwidth, demheight, dem16U, demwidth, demheight, GDT_Int16, 0, 0);
		for(size_t k=0; k<(size_t)demwidth*demheight; k++)
		{
			demdata[k] = dem16U[k];
		}
		delete dem16U; dem16U=NULL;
	}
	else
	{
		pBand->RasterIO(GF_Read, 0, 0, demwidth, demheight, demdata, demwidth, demheight, GDT_Float32,0,0);
	}
	GDALClose(poDataset);
	for(size_t k=0; k<(size_t)demwidth*demheight; k++)
	{
		if(demdata[k]<0) demdata[k] = 0;
	}
	return 0;
}

void Stereo::GeoToMeter(double lon,double lat,double latpos,double &ratiox,double &ratioy)
{
	double a0=6378137;
	double b0=6356752.3142;
	ratiox = cos(latpos * CV_PI / 180.0) * lon * 3.1415926 * a0 / 180.0;
	ratioy = lat * CV_PI * b0 / 180.0;
}

void Stereo::meterTogeo(double ratiox,double ratioy,double lat,double &ratioLong,double &ratioLat)
{
	double a=6378137;
	double b=6356752.3142;
	ratioLat=(ratioy/(2*3.14159265359*b)*360);
	ratioLong=(ratiox/(2*3.14159265359*a*cos(lat*3.14159265359/180))*360);
}

void Stereo::Geo2img(double longi, double lat,double &imgx,double &imgy,double height, int flag)
{
	char** papszTransOption = NULL; int nSuccess;
	if(flag == 0) //left
	{
		void* pRPCTransform = GDALCreateRPCTransformer(&sRPC, FALSE, 0, papszTransOption);
		GDALRPCTransform(pRPCTransform, TRUE, 1, &longi, &lat, &height, &nSuccess);
		imgx = longi; imgy = lat;
		GDALDestroyRPCTransformer(pRPCTransform);
	}
	else if (flag == 1) //right
	{
		void* pRPCTransform = GDALCreateRPCTransformer(&sRPC2, FALSE, 0, papszTransOption);
		GDALRPCTransform(pRPCTransform, TRUE, 1, &longi, &lat, &height, &nSuccess);
		imgx = longi; imgy = lat;
		GDALDestroyRPCTransformer(pRPCTransform);
	}
	CSLDestroy(papszTransOption);
}

int Stereo::Img2Geo(double imgx, double imgy,double &longi,double &lat, double height, int flag)
{
	char** papszTransOption = NULL; int nSuccess = 0;
	if(flag==0) //left
	{
		void* pRPCTransform = GDALCreateRPCTransformer(&sRPC, TRUE, 0, papszTransOption);
		GDALRPCTransform(pRPCTransform, TRUE, 1, &imgx, &imgy, &height, &nSuccess);
		longi = imgx; lat = imgy;
		GDALDestroyRPCTransformer(pRPCTransform);
	}
	else if(flag==1)
	{
		void* pRPCTransform = GDALCreateRPCTransformer(&sRPC2, TRUE, 0, papszTransOption);
		GDALRPCTransform(pRPCTransform, TRUE, 1, &imgx, &imgy, &height, &nSuccess);
		longi = imgx; lat = imgy;
		GDALDestroyRPCTransformer(pRPCTransform);
	}
	CSLDestroy(papszTransOption); 
	return nSuccess;
}

int Stereo::Img2GeoDEM(double imgx, double imgy, double& longi, double& lat, double& height, int flag)
{
	if (demdata == NULL || demwidth < 0 || demheight < 0)
	{
		cout << "Error:No dem!" << endl;
		return false;
	}
	double demx, demy, H1 = 0, H2 = 1, nlong1, nlat1; int nSuccess;
	if (flag == 0) //left
	{
		Img2Geo(imgx, imgy, nlong1, nlat1, sRPC.dfHEIGHT_OFF, 0);
		int iter = 0;
		while (abs(H2 - H1) > 0.01 && iter < 1000)
		{
			H1 = H2;
			GeoToImgPointDouble(nlong1, nlat1, demx, demy, _geotransform);
			heightBilinearInter(H2, demx, demy, demwidth, demheight, demdata);
			Img2Geo(imgx, imgy, nlong1, nlat1, H2, 0);
			iter++;
		}
		lat = nlat1; longi = nlong1; height = H2;
	}
	else if (flag == 1)
	{
		Img2Geo(imgx, imgy, nlong1, nlat1, sRPC2.dfHEIGHT_OFF, 1);
		int iter = 0;
		while (abs(H2 - H1) > 0.01 && iter < 1000)
		{
			H1 = H2;
			GeoToImgPointDouble(nlong1, nlat1, demx, demy, _geotransform);
			heightBilinearInter(H2, demx, demy, demwidth, demheight, demdata);
			Img2Geo(imgx, imgy, nlong1, nlat1, H2, 1);
			iter++;
		}
		lat = nlat1; longi = nlong1; height = H2;
	}
	return nSuccess;
}

void Stereo::heightBilinearInter(double &height, double imgx, double imgy, int tifWidth, int tifHeight, float *buf)
{
	//Point2d imgInter;
	int imagextemp_large, imagextemp_small, imageytemp_large, imageytemp_small;
	double f11,f12,f21,f22; double imagextemp = imgx; double imageytemp = imgy;
	imagextemp_large = (int)ceil(imgx);
	imagextemp_small = (int)floor(imgx);
	imageytemp_large = (int)ceil(imgy);
	imageytemp_small = (int)floor(imgy);
	if (imagextemp_large < 0 || imageytemp_large < 0 || imagextemp_small < 0 || imageytemp_small < 0)
	{
		height = 0;	return;
	}
	if (imagextemp_large>=tifWidth || imagextemp_small>=tifWidth || imageytemp_large >=tifHeight || imageytemp_small >=tifHeight)
	{
		height = 0;	return;
	}
	f11 = buf[(size_t)imageytemp_small*tifWidth+imagextemp_small];
	f12 = buf[(size_t)imageytemp_large*tifWidth+imagextemp_small];
	f21 = buf[(size_t)imageytemp_small*tifWidth+imagextemp_large];
	f22 = buf[(size_t)imageytemp_large*tifWidth+imagextemp_large];
	if (imagextemp_small == imagextemp_large && imageytemp_small == imageytemp_large)
	{
		height = f11;
	}
	else if (imagextemp_small != imagextemp_large && imageytemp_small == imageytemp_large)
	{
		height = f11*(imagextemp_large-imagextemp)/(imagextemp_large - imagextemp_small)
			+ f21*(imagextemp-imagextemp_small)/(imagextemp_large - imagextemp_small);
	}
	else if (imagextemp_small == imagextemp_large && imageytemp_small != imageytemp_large)
	{
		height = f11*(imageytemp_large-imageytemp)/(imageytemp_large - imageytemp_small)
			+ f12*(imageytemp-imageytemp_small)/(imageytemp_large - imageytemp_small);
	}
	else //(imagextemp_small != imagextemp_large && imageytemp_small != imageytemp_large)
	{
		height = f11*(imagextemp_large-imagextemp)*(imageytemp_large-imageytemp)
			+f21*(imagextemp-imagextemp_small)*(imageytemp_large-imageytemp)
			+f12*(imagextemp_large-imagextemp)*(imageytemp-imageytemp_small)
			+f22*(imagextemp-imagextemp_small)*(imageytemp-imageytemp_small);
	}
}

void Stereo::GeoToImgPointDouble(double geox, double geoy, double& imgx, double& imgy, double* padfTransform)
{
	double dTemp = padfTransform[1] * padfTransform[5] - padfTransform[2] * padfTransform[4];
	imgx = ((padfTransform[5] * (geox - padfTransform[0]) - padfTransform[2] * (geoy - padfTransform[3])) / dTemp);
	imgy = ((padfTransform[1] * (geoy - padfTransform[3]) - padfTransform[4] * (geox - padfTransform[0])) / dTemp);
}

void Stereo::img2Geopoint(int imgx, int imgy, double& geox, double& geoy, double* padfTransform)
{
	geox = padfTransform[0] + imgx * padfTransform[1] + imgy * padfTransform[2];
	geoy = padfTransform[3] + imgx * padfTransform[4] + imgy * padfTransform[5];
}