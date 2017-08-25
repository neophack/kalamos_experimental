#include <kalamos_context.hpp>

#include <opencv2/opencv.hpp>

using namespace std;

string load_path, save_path;

static void show_help(int argc, char **argv) {
	cerr << "Usage: " << argv[0] << " [--load <load_path>] [--save <save_path>]"
			<< endl;
	cerr << "--load <load_path>: Load calibration images from <load_path>."
			<< endl;
	cerr << "--save <save_path>: Save calibration images to <save_path>."
			<< endl;
}

static int parse_args(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		if (string(argv[i]) == "--help" || string(argv[i]) == "-h") {
			return 1;
		} else if (string(argv[i]) == "--save" && i + 1 < argc) {
			save_path = string(argv[i + 1]);
		} else if (string(argv[i]) == "--load" && i + 1 < argc) {
			load_path = string(argv[i + 1]);
		} else {
			cerr << "Unknown argument '" << argv[i] << "'!" << endl;
			return 1;
		}
	}
	return 0;
}

static int kalamos_init(void) {
	// Set up callbacks
	kalamos::Callbacks cbs;
	// Initialize context
	std::unique_ptr<kalamos::Context> ctx;
}

static int capture_images(vector<cv::Mat[2]> &imgs) {
	return 1;
}

int main(int argc, char **argv) {
	// Parse command line arguments
	if (parse_args(argc, argv)) {
		show_help(argc, argv);
		return 1;
	}

	// Get calibration images
	vector<cv::Mat[2]> imgs;
	if (load_path.empty()) {
		// Obtain calibration images from kalamos
		capture_images(imgs);
	} else {
		// TODO
	}
}
