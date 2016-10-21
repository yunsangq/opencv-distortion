#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

double fx = 0.0, fy = 0.0, cx = 0.0,
cy = 0.0, k1 = 0.0, k2 = 0.0, p1 = 0.0, p2 = 0.0;

double f_fx = 0.0, f_fy = 0.0, f_cx = 0.0, f_cy = 0.0, f_w = 0.0;

void init() {
	/*
	rms, fx, fy, cx, cy, k1, k2, p1, p2
	*/
	vector<double>paramlist;
	double param = 0.0;
	ifstream infile("hw3_2_Zhang.txt");
	while (infile >> param) {
		paramlist.push_back(param);
	}
	fx = paramlist[0];
	fy = paramlist[1];
	cx = paramlist[2];
	cy = paramlist[3];
	k1 = paramlist[4];
	k2 = paramlist[5];
	p1 = paramlist[6];
	p2 = paramlist[7];

	/* FOV
	f_fx, f_fy, f_cx, f_cy, f_w
	*/
	vector<double>f_paramlist;
	param = 0.0;
	ifstream f_infile("hw3_2_FOV.txt");
	while (f_infile >> param) {
		f_paramlist.push_back(param);
	}
	f_fx = f_paramlist[0];
	f_fy = f_paramlist[1];
	f_cx = f_paramlist[2];
	f_cy = f_paramlist[3];
	f_w = f_paramlist[4];
}

int main() {
	init();

	VideoCapture vc("samplevideo.avi");
	if (!vc.isOpened()) return 0;
	int fps = vc.get(CV_CAP_PROP_FPS);

	Mat input;
	while (1) {
		vc >> input;
		if (input.empty()) break;
		int col = input.cols;
		int row = input.rows;
		int ch = input.channels();
		Mat output_1(row, col, CV_8UC3);
		Mat output_2(row, col, CV_8UC3);
		//output_1
		for (int y = 0; y < row; ++y) {
			for (int x = 0; x < col; ++x) {
				//Á¤±ÔÈ­ ¿Ö°îÀÌ ¾ø´Â ÁÂÇ¥
				double xn_u = (x - cx) / fx;
				double yn_u = (y - cy) / fy;

				//¿Ö°î º¸Á¤
				double ru2 = xn_u*xn_u + yn_u*yn_u;
				double radial_d = 1 + k1*ru2 + k2*ru2*ru2;

				double xn_d = radial_d*xn_u + 2 * p1*xn_u*yn_u + p2*(ru2 + 2 * xn_u*xn_u);
				double yn_d = radial_d*yn_u + p1*(ru2 + 2 * yn_u*yn_u) + 2 * p2*xn_u*yn_u;

				//ÇÈ¼¿ ¿Ö°îµÈ ¿µ»ó ÁÂÇ¥
				double xp_d = fx*xn_d + cx;
				double yp_d = fy*yn_d + cy;

				int px = xp_d;
				int py = yp_d;

				uchar* in = (uchar*)input.data + input.step*py;
				uchar* out = (uchar*)output_1.data + output_1.step*y;

				out[ch*x] = in[ch*px];
				out[ch*x + 1] = in[ch*px + 1];
				out[ch*x + 2] = in[ch*px + 2];
			}
		}

		//output_2
		for (int y = 0; y < row; ++y) {
			for (int x = 0; x < col; ++x) {
				//Á¤±ÔÈ­ ¿Ö°îÀÌ ¾ø´Â ÁÂÇ¥
				double xn_u = (x - f_cx) / f_fx;
				double yn_u = (y - f_cy) / f_fy;

				//¿Ö°î º¸Á¤				
				double ru = sqrt(xn_u*xn_u + yn_u*yn_u);
				double rd = (1 / f_w)*atan(2 * ru * tan(f_w / 2));
				ru = tan(rd*f_w) / (2 * tan(f_w / 2));

				double xn_d = xn_u / (ru / rd);
				double yn_d = yn_u / (ru / rd);

				//ÇÈ¼¿ ¿Ö°îµÈ ¿µ»ó ÁÂÇ¥
				double xp_d = f_fx*xn_d + f_cx;
				double yp_d = f_fy*yn_d + f_cy;
				
				int px = xp_d;
				int py = yp_d;

				uchar* in = (uchar*)input.data + input.step*py;
				uchar* out = (uchar*)output_2.data + output_2.step*y;

				out[ch*x] = in[ch*px];
				out[ch*x + 1] = in[ch*px + 1];
				out[ch*x + 2] = in[ch*px + 2];
			}
		}		

		Mat disp(row * 2, col * 2, CV_8UC3);
		Mat imageRoI = disp(Rect(0, 0, col, row));
		Mat mask = input;
		input.copyTo(imageRoI, mask);
		imageRoI = disp(Rect(col, 0, col, row));
		mask = output_1;
		output_1.copyTo(imageRoI, mask);
		imageRoI = disp(Rect(0, row, col, row));
		mask = output_2;
		output_2.copyTo(imageRoI, mask);

		imshow("result", disp);

		if (waitKey(fps) == 27) break;
	}
	destroyAllWindows();




	return 0;
}