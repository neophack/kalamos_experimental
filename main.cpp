#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

int main(int argc, char **argv) {
	namedWindow("Output", 1);
	Mat output = Mat::zeros(120, 350, CV_8UC3);
	putText(output, "Hello world!", cvPoint(15, 70), FONT_HERSHEY_PLAIN, 3,
			cvScalar(0, 255, 0), 4);
	imshow("Output", output);
	waitKey(0);
	return 0;
}
