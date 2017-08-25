// Experiment to render left frame directly from kalamos_context

#include <kalamos_context.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <unistd.h>

using namespace kalamos;
using namespace cv;

StereoSGBM sgbm;

static void onStereoYuv(StereoYuvData const &data) {
//	std::cout << "stereo" << std::endl;
//	std::cout << data.leftYuv[0]->size();

//	imshow("Disparity", *data.leftYuv[0]);
//	waitKey(1);
//	return;

// Shrink input images
	std::cout << "Shrink images... ";
	Mat left_small, right_small;
	resize(*data.leftYuv[0], left_small, Size(), 0.25, 0.25, INTER_NEAREST);
	resize(*data.rightYuv[0], right_small, Size(), 0.25, 0.25, INTER_NEAREST);
	std::cout << "ok" << std::endl;
	// Calculate disparity
	Mat disp;
	std::cout << "SGBM..." << std::endl;
	sgbm(left_small, right_small, disp);
	std::cout << "ok" << std::endl;
	// Show result
	double min, max;
	minMaxIdx(disp, &min, &max);
	Mat coloredDisparityMap, scaledDisparityMap;
	convertScaleAbs(disp, scaledDisparityMap, 255 / (max - min));
	applyColorMap(scaledDisparityMap, coloredDisparityMap, COLORMAP_BONE);
	Mat output;
	resize(coloredDisparityMap, output, Size(), 4, 4, INTER_CUBIC);

	imshow("Disparity", output);
	waitKey(1);
}

static void kalamos_init(void) {
	// Set up callbacks
	Callbacks cbs;
	cbs.stereoYuvCallback = onStereoYuv;
	cbs.period = 1.0 / 25.0;
	// Set up options
	Options opt;
	opt.videoMode = VideoMode::MODE_900_700_120;
	// Initialize kalamos context
	std::cout << "Initialize kalamos context..." << std::endl;
	std::unique_ptr<Context> k_context = init(cbs, opt);
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

	// Set SGBM tuning
	sgbm.numberOfDisparities = 64;
	sgbm.P1 = 200;
	sgbm.P2 = 1600;

	// Open window
	std::cout << "Open window... " << std::endl;
	namedWindow("Disparity");
	std::cout << "ok" << std::endl << std::endl;
	// Start kalamos
	kalamos_init();
	return 0;
}
