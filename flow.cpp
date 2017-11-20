#include "kalamoscapture.hpp"
#include "kalamosrectify.hpp"

#include <opencv2/opencv.hpp>

#include <memory>
#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc, char** argv) {
	// Check that user is root
	if (getuid()) {
		cerr << "This program must be run as sudo!" << std::endl;
		return 1;
	}
	// Initialize kalamos capture
	KalamosCapture cap;
	cv::Size image_size(cap.get(CV_CAP_PROP_FRAME_WIDTH),
			cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	KalamosRectify rect(image_size);
	// Initialize optical flow
	cv::Ptr<cv::DenseOpticalFlow> of = cv::createOptFlow_DualTVL1();
	of->set("warps", 1);
	of->set("nscales", 5);
	of->set("iterations", 10);

	cerr << "Optical flow settings:" << endl;
	cerr << "tau = " << of->get<double>("tau") << endl;
	cerr << "lambda = " << of->get<double>("lambda") << endl;
	cerr << "theta = " << of->get<double>("theta") << endl;
	cerr << "nscales = " << of->get<int>("nscales") << endl;
	cerr << "warps = " << of->get<int>("warps") << endl;
	cerr << "epsilon = " << of->get<double>("epsilon") << endl;
	cerr << "iterations = " << of->get<int>("iterations") << endl;

	// Capture and calculate optical flow
	cv::Mat left, left_prev;
	cv::Mat flow, flow_disp;
	int key = -1;
	while (key != 27) {
		if (cap.grab() && cap.retrieve(left, KalamosChannel::LEFT)) {
			cv::resize(left, left, cv::Size(0, 0), 0.25, 0.25,
					cv::INTER_NEAREST);
			if (!left_prev.empty()) {
				of->calc(left_prev, left, flow);
				// OF rendering code from https://stackoverflow.com/questions/7693561/opencv-displaying-a-2-channel-image-optical-flow
				//extraxt x and y channels
				cv::Mat xy[2]; //X,Y
				cv::split(flow, xy);
				//calculate angle and magnitude
				cv::Mat magnitude, angle;
				cv::cartToPolar(xy[0], xy[1], magnitude, angle, true);
				//translate magnitude to range [0;1]
				double mag_max;
				cv::minMaxLoc(magnitude, 0, &mag_max);
				mag_max = 40; // XXX
				cerr << "mag_max = " << mag_max << endl;
				magnitude.convertTo(magnitude, -1, 1.0 / mag_max);
				//build hsv image
				cv::Mat _hsv[3], hsv;
				_hsv[0] = angle;
				_hsv[1] = cv::Mat::ones(angle.size(), CV_32F);
				_hsv[2] = magnitude;
				cv::merge(_hsv, 3, hsv);
				//convert to BGR and show
				cv::Mat bgr; //CV_32FC3 matrix
				cv::cvtColor(hsv, bgr, cv::COLOR_HSV2BGR);
				imshow("Optical flow", bgr);
				key = (cv::waitKey(1) & 0xFF);
			}
			left_prev = left;
		}
	}
}
