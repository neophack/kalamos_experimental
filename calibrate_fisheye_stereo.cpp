// TODO
// Get object points
// Get image points
// cv::fisheye::stereoCalibrate()
//
// cv::fisheye::stereoRectify()

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

static void get_calibration_points(
		cv::VideoCapture& cap_l,
		cv::VideoCapture& cap_r,
		cv::Size const& pattern_size,
		float const& square_size,
		vector<vector<cv::Point2d> >& left_image_points_out,
		vector<vector<cv::Point2d> >& right_image_points_out,
		vector<vector<cv::Point3d> >& object_points_out) {

	vector<cv::Point3d> object_points;
	for (int r = 0; r < pattern_size.height; r++) {
		for (int c = 0; c < pattern_size.width; c++) {
			object_points.push_back(
					cv::Point3d(c * square_size, r * square_size, 0.0));
		}
	}

	cv::Mat left, right;
	while (cap_l.read(left) && cap_r.read(right)) {
		cv::Mat gray_l, gray_r;
		bool found_l, found_r;
		vector<cv::Point2f> corners_l, corners_r;
		cv::cvtColor(left, gray_l, CV_BGR2GRAY);
		cv::cvtColor(right, gray_r, CV_BGR2GRAY);
		found_l = cv::findChessboardCorners(gray_l, pattern_size, corners_l,
				cv::CALIB_CB_ADAPTIVE_THRESH +
						cv::CALIB_CB_NORMALIZE_IMAGE +
						cv::CALIB_CB_FAST_CHECK);
		found_r = cv::findChessboardCorners(gray_r, pattern_size, corners_r,
				cv::CALIB_CB_ADAPTIVE_THRESH +
						cv::CALIB_CB_NORMALIZE_IMAGE +
						cv::CALIB_CB_FAST_CHECK);
		if (found_l && found_r) {
			cv::cornerSubPix(gray_l, corners_l, cv::Size(11, 11),
					cv::Size(-1, -1),
					cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30,
							0.1));
			cv::cornerSubPix(gray_r, corners_r, cv::Size(11, 11),
					cv::Size(-1, -1),
					cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30,
							0.1));
			vector<cv::Point2d> out_l;
			vector<cv::Point2d> out_r;
			for (int i = 0; i < corners_l.size(); i++) {
				out_l.push_back(corners_l[i]);
				out_r.push_back(corners_r[i]);
			}
			object_points_out.push_back(object_points);
			left_image_points_out.push_back(out_l);
			right_image_points_out.push_back(out_r);
		}
	}
}

int main(int argc, char **argv) {
	// Load images
	cv::VideoCapture left(string(argv[1]) + "left_%d.png");
	cv::VideoCapture right(string(argv[1]) + "right_%d.png");
	cerr << "Number of left frames:\t" << left.get(CV_CAP_PROP_FRAME_COUNT)
			<< endl;
	cerr << "Number of right frames:\t" << right.get(CV_CAP_PROP_FRAME_COUNT)
			<< endl;
	if (left.get(CV_CAP_PROP_FRAME_COUNT)
			!= right.get(CV_CAP_PROP_FRAME_COUNT)) {
		cerr << "Unequal number of left and right images! Exiting..." << endl;
		return 1;
	}
	cv::Size image_size(left.get(CV_CAP_PROP_FRAME_WIDTH),
			left.get(CV_CAP_PROP_FRAME_HEIGHT));
	cerr << "Frame size: " << image_size.width << " × " << image_size.height
			<< endl;

	// Get calibration points
	cerr << "Extracting calibration points... ";
	cv::Size const pattern_size(9, 6);
	float const square_size = 35.15e-3;
	vector<vector<cv::Point2d> > left_image_points;
	vector<vector<cv::Point2d> > right_image_points;
	vector<vector<cv::Point3d> > object_points;
	get_calibration_points(left, right, pattern_size, square_size,
			left_image_points, right_image_points, object_points);
	cerr << "ok" << endl;
	cerr << "Found " << object_points.size() << " usable images." << endl;

	// Calibrate
	cerr << "Calibrating... ";
	cv::Matx33d K1, K2, R;
	cv::Vec3d T;
	cv::Vec4d D1, D2;
	cv::fisheye::stereoCalibrate(object_points, left_image_points,
			right_image_points, K1, D1, K2, D2, image_size, R, T,
			cv::fisheye::CALIB_FIX_SKEW | cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC
					| cv::fisheye::CALIB_CHECK_COND,
			cv::TermCriteria(3, 12, 0));
	cerr << "ok" << endl;

	cerr << "K1:" << endl << K1 << endl;
	cerr << "D1:" << endl << D1 << endl;
	cerr << "K2" << endl << K2 << endl;
	cerr << "D2:" << endl << D2 << endl;
	cerr << "R:" << endl << R << endl;
	cerr << "T:" << endl << T << endl;

	cerr << "Saving results... ";
	cv::FileStorage fs(string(argv[1]) + "calib.yml", cv::FileStorage::WRITE);
	fs << "K1" << cv::Mat(K1);
	fs << "D1" << D1;
	fs << "K2" << cv::Mat(K2);
	fs << "D2" << D2;
	fs << "R" << cv::Mat(R);
	fs << "T" << T;
	cerr << "ok" << endl;

	// Rectify
	cerr << "Rectification... ";
	cv::Mat R1, P1, R2, P2, Q;
	cv::fisheye::stereoRectify(K1, D1, K2, D2, image_size, R, T, R1, R2, P1, P2,
			Q, CV_CALIB_ZERO_DISPARITY, image_size, 0.0, 1.0);
	cerr << "ok" << endl;

	cerr << "R1" << endl << R1 << endl;
	cerr << "P1" << endl << P1 << endl;
	cerr << "R2" << endl << R2 << endl;
	cerr << "P2" << endl << P2 << endl;
	cerr << "Q" << endl << Q << endl;

	cerr << "Saving results... ";
	cv::FileStorage fs2(string(argv[1]) + "rect.yml", cv::FileStorage::WRITE);
	fs2 << "R1" << R1;
	fs2 << "P1" << P1;
	fs2 << "R2" << R2;
	fs2 << "P2" << P2;
	fs2 << "Q" << Q;
	cerr << "ok" << endl;

	// Show results
	cerr << "Calculate remap... ";
	cv::Mat P_l, P_r, map1_l, map1_r, map2_l, map2_r;
	cv::fisheye::initUndistortRectifyMap(K1, D1, R1, P1, image_size, CV_32F,
			map1_l, map2_l);
	cv::fisheye::initUndistortRectifyMap(K2, D2, R2, P2, image_size, CV_32F,
			map1_r, map2_r);
	cerr << "ok" << endl;

	cv::Mat disp_raw_l, disp_raw_r, disp_raw, disp_l, disp_r, disp_rect,
			display;
	cv::VideoCapture left2(string(argv[1]) + "left_%d.png");
	cv::VideoCapture right2(string(argv[1]) + "right_%d.png");
	int key = -1;
	while (key != 27 && left2.read(disp_raw_l) && right2.read(disp_raw_r)) {
		cv::remap(disp_raw_l, disp_l, map1_l, map2_l, cv::INTER_LINEAR);
		cv::remap(disp_raw_r, disp_r, map1_r, map2_r, cv::INTER_LINEAR);
		cv::resize(disp_raw_l, disp_raw_l, cv::Size(0, 0), 0.5, 0.5);
		cv::resize(disp_raw_r, disp_raw_r, cv::Size(0, 0), 0.5, 0.5);
		cv::resize(disp_l, disp_l, cv::Size(0, 0), 0.5, 0.5);
		cv::resize(disp_r, disp_r, cv::Size(0, 0), 0.5, 0.5);
		cv::hconcat(disp_l, disp_r, disp_rect);
		cv::hconcat(disp_raw_l, disp_raw_r, disp_raw);
		cv::vconcat(disp_rect, disp_raw, display);
		imshow("Rectified images", display);
		key = cv::waitKey() & 0xFF;
	}

	return 0;
}
