#include <opencv2/opencv.hpp>
#include <unistd.h>

#include "kalamoscapture.hpp"
#include "kalamosrectify.hpp"

cv::Size singlesize(640, 480);

int main(int argc, char **argv) {
	// Check that user is root
	if (getuid()) {
		cerr << "This program must be run as sudo!" << std::endl;
		return 1;
	}
	// Open Kalamos capture device
	KalamosCapture cap;
	if (!cap.isOpened()) {
		cerr << "Error opening KalamosCapture!" << endl;
		return 1;
	}
	// Instantiate rectifier
	cv::Size image_size(cap.get(CV_CAP_PROP_FRAME_WIDTH),
			cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	KalamosRectify rect(image_size);
	// Show left and right images
	bool loop = true;
	cv::Mat left, right;
	cv::Mat display;
	while (loop) {
		// Grab images
		if (cap.grab()
				&& cap.retrieve(left, KalamosChannel::LEFT)
				&& cap.retrieve(right, KalamosChannel::RIGHT)) {
			// Rectify
			rect.rectify(left, right, left, right);
			// Scale down
			cv::resize(left, left, singlesize);
			cv::resize(right, right, singlesize);
			// Display
			cv::hconcat(left, right, display);
			imshow("Kalamos", display);
			loop = (cv::waitKey(1) == -1);
		}
	}
	return 0;
}
