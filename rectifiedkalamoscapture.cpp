#include "rectifiedkalamoscapture.hpp"

#include <iostream>

using namespace std;

RectifiedKalamosCapture::RectifiedKalamosCapture(
		const string& intrinsics_file,
		const string& extrinsics_file) {
	cerr << "Loading calibration data from" << endl;
	cerr << intrinsics_file << endl;
	cerr << extrinsics_file << endl;
	// TODO load calibration data

	cerr << "Loading intrinsics... ";
	cv::FileStorage fs;
	if (!fs.open(intrinsics_file, cv::FileStorage::READ)) {
		cerr << "ERROR: could not open intrincs '" << intrinsics_file << "'!"
				<< endl;
		return;
	}
	fs["M1"] >> _Kl;
	fs["M2"] >> _Kr;
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

}

RectifiedKalamosCapture::~RectifiedKalamosCapture() {

}

/**
 * Retrieve rectified frames from Kalamos.
 * @param image
 * @param channel
 * @return
 */
bool RectifiedKalamosCapture::retrieve(cv::Mat& image, int channel) {
	cv::Mat raw;
	KalamosCapture::retrieve(raw, channel);
	// TODO Do rectification stuff...
	raw.copyTo(image);
	return true;
}
