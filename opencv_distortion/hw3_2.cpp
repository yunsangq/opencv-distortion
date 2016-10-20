#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

double fx = 0.0, fy = 0.0, cx = 0.0,
cy = 0.0, k1 = 0.0, k2 = 0.0, p1 = 0.0, p2 = 0.0;

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
		Mat output(row, col, CV_8UC3);

		int x, y;
		for (y = 0; y < row; ++y) {
			for (x = 0; x < col; ++x) {
				//Á¤±ÔÈ­ ¿Ö°îÀÌ ¾ø´Â ÁÂÇ¥
				double xn_u = (x - cx) / fx;
				double yn_u = (y - cy) / fy;

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
				uchar* out = (uchar*)output.data + output.step*y;

				out[ch*x] = in[ch*px];
				out[ch*x + 1] = in[ch*px + 1];
				out[ch*x + 2] = in[ch*px + 2];
			}
		}

		imshow("input cam", input);
		imshow("output cam", output);

		if (waitKey(fps) == 27) break;
	}
	destroyAllWindows();




	return 0;
}