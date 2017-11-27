#include "kalamoscapture.hpp"
#include "kalamosrectify.hpp"

#include <unistd.h>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {
	// Check that user is root
	if (getuid()) {
		cerr << "This program must be run as sudo!" << std::endl;
		return 1;
	}
	// Initialize video capture
	KalamosCapture cap;
	cv::Size image_size(cap.get(CV_CAP_PROP_FRAME_WIDTH),
			cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	cerr << "Image size: " << image_size.width << " x " << image_size.height
			<< endl;
	KalamosRectify rect(image_size);

	// Initialize SGBM
	cv::StereoSGBM sgbm;
	sgbm.SADWindowSize = 3;
	sgbm.numberOfDisparities = 32;
	sgbm.P1 = 8 * 1 * sgbm.SADWindowSize * sgbm.SADWindowSize; // https://docs.opencv.org/3.3.0/d1/d9f/classcv_1_1stereo_1_1StereoBinarySGBM.html
	sgbm.P2 = 32 * 1 * sgbm.SADWindowSize * sgbm.SADWindowSize;
	sgbm.uniquenessRatio = 10;
	sgbm.speckleWindowSize = 100;
	sgbm.speckleRange = 2;
	sgbm.fullDP = false;

	// Capture and compute stereo
	cv::Mat left, right;
	cv::Mat disp;
	int key = -1;
	while (key != 27) {
		if (cap.grab() && cap.retrieve(left, KalamosChannel::LEFT)
				&& cap.retrieve(right, KalamosChannel::RIGHT)) {
			rect.rectify(left, right, left, right);
			// Scale down images
			cv::resize(left, left, cv::Size(0, 0), 0.25, 0.25,
					cv::INTER_NEAREST);
			cv::resize(right, right, cv::Size(0, 0), 0.25, 0.25,
					cv::INTER_NEAREST);
			// Calculate disparity
			sgbm(left, right, disp);
			// Show result
			cv::Mat disp_col;

			double min, max;
			cv::minMaxIdx(disp, &min, &max);
//			cerr << "min/16 = " << min / 16.0 << ", max/16 = " << max / 16.0
//					<< endl;
			cv::convertScaleAbs(disp, disp_col,
					255.0 / (16 * sgbm.numberOfDisparities), 0);
//			disp.convertTo(disp_col, CV_8UC1, 255.0 / (64 * 16), 0);
			cv::applyColorMap(disp_col, disp_col, cv::COLORMAP_JET);

			const double gray_weight = 100;
			cv::cvtColor(left, left, CV_GRAY2BGR);
			cv::convertScaleAbs(left, left, gray_weight / 255,
					255 - gray_weight);
			cv::multiply(disp_col, left, disp_col, 1.0 / 255);
			cv::imshow("Disparity", disp_col);
//			cv::imshow("Left", left);
		}
		key = (cv::waitKey(1) & 0xFF);
	}
}
