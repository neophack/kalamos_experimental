#ifndef __KALAMOSCAPTURE_H__
#define __KALAMOSCAPTURE_H__

#include <opencv2/highgui.hpp>

#include <string>

enum KalamosChannel {
	LEFT,
	RIGHT,
	size, // Number of channels
};

/**
 * VideoCapture class for Kalamos / SLAMDunk
 */
class KalamosCapture: public cv::VideoCapture {
public:
	KalamosCapture();
	virtual ~KalamosCapture();

	virtual bool open(const std::string& filename);
	virtual bool open(int device);
	virtual bool isOpened() const;
	virtual void release();

	virtual bool grab();
	virtual bool retrieve(cv::Mat& image, int channel = 0);
	//	virtual cv::VideoCapture& operator >>(cv::Mat& image);
	//	virtual bool read(cv::Mat& image);

	virtual bool set(int propId, double value);
	virtual double get(int propId);
};

#endif
