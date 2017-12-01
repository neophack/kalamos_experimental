#include "kalamosrectify.hpp"

#include <iostream>

using namespace std;

KalamosRectify::KalamosRectify(
		const cv::Size& image_size,
		const string& intrinsics_file,
		const string& extrinsics_file) {
	cerr << "Loading calibration data from" << endl;
	cerr << intrinsics_file << endl;
	cerr << extrinsics_file << endl;

	cerr << "Loading intrinsics... ";
	cv::FileStorage fs;
	if (!fs.open(intrinsics_file, cv::FileStorage::READ)) {
		cerr << "ERROR: could not open intrincs '" << intrinsics_file << "'!"
				<< endl;
		return;
	}
	fs["M1"] >> _Kl;
	_Kl.at<double>(0, 2) += image_size.width / 2; // See http://developer.parrot.com/docs/slamdunk/#stereo
	_Kl.at<double>(1, 2) += image_size.height / 2;
	fs["M2"] >> _Kr;
	_Kr.at<double>(0, 2) += image_size.width / 2;
	_Kr.at<double>(1, 2) += image_size.height / 2;
	fs["D1"] >> _Dl;
	fs["D2"] >> _Dr;
	fs.release();
	cerr << "ok" << endl;

	cerr << "Loading extrinsics... ";
	if (!fs.open(extrinsics_file, cv::FileStorage::READ)) {
		cerr << "ERROR: could not open extrinsics '" << extrinsics_file << "'!"
				<< endl;
		return;
	}
	fs["R1"] >> _Rl;
	fs["R2"] >> _Rr;
	fs["R"] >> _R;
	fs["T"] >> _T;
	fs.release();
	cerr << "ok" << endl;

	cerr << "Find rectification mapping... ";
	cv::fisheye::stereoRectify(_Kl, _Dl, _Kr, _Dr, image_size, _R, _T, _Rl, _Rr,
			_Pl, _Pr, _Q, CV_CALIB_ZERO_DISPARITY, image_size, 0.0, 1.0);
	cv::fisheye::initUndistortRectifyMap(_Kl, _Dl, _Rl, _Pl, image_size, CV_32F,
			_map1l, _map2l);
	cv::fisheye::initUndistortRectifyMap(_Kr, _Dr, _Rr, _Pr, image_size, CV_32F,
			_map1r, _map2r);
	cerr << "ok" << endl;
}

void KalamosRectify::rectify(
		const cv::Mat& left,
		const cv::Mat& right,
		cv::Mat& left_rect,
		cv::Mat& right_rect) {
	cv::remap(left, left_rect, _map1l, _map2l, cv::INTER_NEAREST);
	cv::remap(right, right_rect, _map1r, _map2r, cv::INTER_NEAREST);
}
