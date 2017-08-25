#include "kalamoscapture.hpp"

#include <kalamos_context.hpp>

#include <iostream>
#include <thread>
#include <mutex>
#include <cstdint>

using namespace std;

static bool initialized = false;
static unique_ptr<kalamos::Context> context;
static unique_ptr<kalamos::ServiceHandle> capturehandle;
static thread kalamos_thread;

// Current frames
static mutex mutex_frame;
static cv::Mat current_frame[KalamosChannel::size];
static uint64_t current_ts;
// Grabbed frames
static cv::Mat grabbed_frame[KalamosChannel::size];

static void onStereoYuv(kalamos::StereoYuvData const &data) {
	mutex_frame.lock();
	data.leftYuv[0]->copyTo(current_frame[KalamosChannel::LEFT]);
	data.rightYuv[0]->copyTo(current_frame[KalamosChannel::RIGHT]);
	current_ts = data.ts;
	mutex_frame.unlock();
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
	bool success = true;
	mutex_frame.lock();
	for (int i = 0; i < KalamosChannel::size; i++) {
		current_frame[i].copyTo(grabbed_frame[i]);
		if (grabbed_frame[i].empty()) {
			success = false;
		}
	}
	mutex_frame.unlock();
	return success;
}

/**
 * Retrieve the grabbed video image.
 * @param image
 * @param channel Channel to grab (KalamosChannel)
 * @return
 */
bool KalamosCapture::retrieve(cv::Mat& image, int channel) {
	if (channel >= 0 && channel < KalamosChannel::size &&
			!grabbed_frame[0].empty()) {
		grabbed_frame[channel].copyTo(image);
		return true;
	}
	return false;
}

bool KalamosCapture::set(int propId, double value) {
	return false; // Not supported
}

double KalamosCapture::get(int propId) {
	return 0.0; // Not supported
}
