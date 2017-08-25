// Experiment to render left frame directly from kalamos_context

#include <kalamos_context.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <unistd.h>

using namespace kalamos;
using namespace cv;

static void onStereoYuv(StereoYuvData const &data) {
//	std::cout << "stereo" << std::endl;
//	std::cout << data.leftYuv[0]->size();
	imshow("Left frame", *data.leftYuv[0]);
	waitKey(1);
}

static void kalamos_init(void) {
	// Set up callbacks
	Callbacks cbs;
	cbs.stereoYuvCallback = onStereoYuv;
	cbs.period = 1.0 / 25.0;
	// Initialize kalamos context
	std::cout << "Initialize kalamos context..." << std::endl;
	std::unique_ptr<Context> k_context = init(cbs);
	if (!k_context) {
		std::cerr << "Error initializing k_context" << std::endl;
		return;
	}
	std::cout << "ok" << std::endl << std::endl;
	// Start capture service
	std::cout << "Start capture service... " << std::endl;
	std::unique_ptr<kalamos::ServiceHandle> captureHandle =
			k_context->startService(ServiceType::CAPTURE);
	std::cout << "ok" << std::endl << std::endl;
	// Run
	std::cout << "Run... " << std::endl;
	k_context->run();
	std::cout << "ok" << std::endl << std::endl;
}

int main(int argc, char **argv) {
	if (getuid()) {
		std::cerr << "This program must be run as sudo!" << std::endl;
		return 1;
	}

	// Open window
	std::cout << "Open window... " << std::endl;
	namedWindow("Left frame");
	std::cout << "ok" << std::endl << std::endl;
	// Start kalamos
	kalamos_init();
	return 0;
}
