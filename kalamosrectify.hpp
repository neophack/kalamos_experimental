#ifndef __RECTIFIEDKALAMOSCAPTURE_H__
#define __RECTIFIEDKALAMOSCAPTURE_H__

#include <opencv2/opencv.hpp>

#include <string>

using namespace std;

#define GETTER(type,var) inline const type& var(void) const { return _##var; }

class KalamosRectify {
public:
	KalamosRectify(
			const cv::Size& image_size,
			const string& intrinsics_file = "/factory/intrinsics.yml",
			const string& extrinsics_file = "/factory/extrinsics.yml");

	void rectify(
			const cv::Mat& left,
			const cv::Mat& right,
			cv::Mat& left_rect,
			cv::Mat& right_rect);

	GETTER(cv::Mat, Kl)
	GETTER(cv::Mat, Kr)
	GETTER(cv::Mat, Dl)
	GETTER(cv::Mat, Dr)
	GETTER(cv::Mat, R)
	GETTER(cv::Mat, T)
	GETTER(cv::Mat, Rl)
	GETTER(cv::Mat, Rr)
	GETTER(cv::Mat, Pl)
	GETTER(cv::Mat, Pr)
	GETTER(cv::Mat, Q)
	GETTER(cv::Mat, map1l)
	GETTER(cv::Mat, map1r)
	GETTER(cv::Mat, map2l)
	GETTER(cv::Mat, map2r)

private:
	cv::Mat _Kl, _Kr, _Dl, _Dr, _R, _T, _Rl, _Rr, _Pl, _Pr, _Q, _map1l, _map1r, _map2l, _map2r;
};

#endif
