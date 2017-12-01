#include "kalamoscapture.hpp"

#include <kalamos_context.hpp>

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstdint>

using namespace std;

static bool initialized = false;
static unique_ptr<kalamos::Context> context;
static unique_ptr<kalamos::ServiceHandle> capturehandle;
static thread kalamos_thread;

// Frame grabbing
static mutex frame_mutex;
static condition_variable frame_cv;
static bool should_grab = false;
static cv::Mat current_frame[KalamosChannel::size];
static uint64_t current_ts;

static void onStereoYuv(kalamos::StereoYuvData const &data) {
	unique_lock<mutex> lk(frame_mutex);
	if (should_grab) {
		data.leftYuv[0]->copyTo(current_frame[KalamosChannel::LEFT]);
		data.rightYuv[0]->copyTo(current_frame[KalamosChannel::RIGHT]);
		current_ts = data.ts;
		should_grab = false;
		lk.unlock();
		frame_cv.notify_all();
	}
}

static void kalamosThread(void) {
	context->run();
}

static bool kalamosInit(void) {
	if (initialized) return true;
	// Set callbacks
	kalamos::Callbacks cbs;
	cbs.stereoYuvCallback = onStereoYuv;
	// Set options
	kalamos::Options opts;
	// Initialize kalamos context
	context = kalamos::init(cbs, opts);
	if (!context) {
		cerr << "Error opening kalamos context!" << endl;
		return false;
	}
	// Start capture service
	capturehandle = context->startService(kalamos::ServiceType::CAPTURE);
	// Start kalamos thread
	kalamos_thread = thread(kalamosThread);
	initialized = true;
	return true;
}

KalamosCapture::KalamosCapture() {
	kalamosInit();
}

KalamosCapture::~KalamosCapture() {
	release(); // Does not actually do anything though...
}

bool KalamosCapture::open(const string& filename) {
	return kalamosInit();
}

bool KalamosCapture::open(int device) {
	return kalamosInit();
}

bool KalamosCapture::isOpened() const {
	return initialized;
}

void KalamosCapture::release() {
	// Do nothing, can't stop Kalamos from running.
}

/**
 * Grab the current image.
 *
 * Grabs a copy of all images at the same time.
 * @return
 */
bool KalamosCapture::grab() {
	if (!isOpened()) {
		return false;
	}
	unique_lock<mutex> lk(frame_mutex);
	should_grab = true;
	while (should_grab) {
		frame_cv.wait(lk);
	}
	return true;
}

/**
 * Retrieve the grabbed video image.
 * @param image
 * @param channel Channel to grab (KalamosChannel)
 * @return
 */
bool KalamosCapture::retrieve(cv::Mat& image, int channel) {
	unique_lock<mutex> lk(frame_mutex);
	if (channel >= 0 && channel < KalamosChannel::size &&
			!current_frame[0].empty()) {
		current_frame[channel].copyTo(image);
		return true;
	}
	return false;
}

bool KalamosCapture::set(int propId, double value) {
	return false; // Not supported
}

double KalamosCapture::get(int propId) {
	double value = 0.0;
	switch (propId) {
	case CV_CAP_PROP_POS_MSEC:
		frame_mutex.lock();
		value = current_ts / 1.0e6;
		frame_mutex.unlock();
		break;

	case CV_CAP_PROP_FRAME_WIDTH:
		switch (context->options().videoMode) {
		case kalamos::VideoMode::MODE_1280_960_30:
			value = 1280;
			break;
		case kalamos::VideoMode::MODE_1500_1500_30:
			value = 1500;
			break;
		case kalamos::VideoMode::MODE_1500_1500_60:
			value = 1500;
			break;
		case kalamos::VideoMode::MODE_900_700_120:
			value = 900;
			break;
		default:
			cerr << "Unknown video mode '" << (int)context->options().videoMode
					<< "'!" << endl;
			break;
		}

	case CV_CAP_PROP_FRAME_HEIGHT:
		switch (context->options().videoMode) {
		case kalamos::VideoMode::MODE_1280_960_30:
			value = 960;
			break;
		case kalamos::VideoMode::MODE_1500_1500_30:
			value = 1500;
			break;
		case kalamos::VideoMode::MODE_1500_1500_60:
			value = 1500;
			break;
		case kalamos::VideoMode::MODE_900_700_120:
			value = 700;
			break;
		default:
			cerr << "Unknown video mode '" << (int)context->options().videoMode
					<< "'!" << endl;
			break;
		}

	default:
		break;
	}
	return value;
}
