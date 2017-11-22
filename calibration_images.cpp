#include <opencv2/opencv.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <unistd.h>

#include "kalamoscapture.hpp"

using namespace std;

const string EXT = ".png";

int main(int argc, char **argv) {
	// Check that user is root
	if (getuid()) {
		cerr << "This program must be run as sudo!" << endl;
		return 1;
	}
	// Check command line arguments
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " <image_path>" << endl;
		return 1;
	}
	string image_path(argv[1]);

	// Instantiate Kalamos capture device
	KalamosCapture cap;

	// Capture images
	int idx = 0;
	cv::Mat left, right;
	cv::Mat display_left, display_right, display;
	int key = 0;
	while ((key & 0xEFFFFF) != 27) { // Loop until escape key
		// Get images
		if (cap.grab()) {
			cap.retrieve(left, KalamosChannel::LEFT);
			cap.retrieve(right, KalamosChannel::RIGHT);
			// Show images to user
			cv::resize(left, display_left, cv::Size(640, 480));
			cv::resize(right, display_right, cv::Size(640, 480));
			cv::hconcat(display_left, display_right, display);
			imshow("Capture calibration images...", display);
			key = cv::waitKey(1);
			// Save if key pressed
			if (key != -1 && (key & 0xEFFFFF) != 27) {
				// Save images
				stringstream filename_left, filename_right;
				filename_left << image_path << "left_" << idx << EXT;
				filename_right << image_path << "right_" << idx << EXT;
				cv::imwrite(filename_left.str(), left);
				cv::imwrite(filename_right.str(), right);
				cout << filename_left.str() << endl;
				idx++;
				// Show that image is recorded
				cv::subtract(cv::Scalar::all(255), display, display);
				imshow("Capture calibration images...", display);
				cv::waitKey(100);
			}
		}
	}
}
