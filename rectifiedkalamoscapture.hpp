#ifndef __RECTIFIEDKALAMOSCAPTURE_H__
#define __RECTIFIEDKALAMOSCAPTURE_H__

#include "kalamoscapture.hpp"

#define GET(type,var) inline const type& var(void) const { return _##var; }

class RectifiedKalamosCapture: public KalamosCapture {
public:
	RectifiedKalamosCapture(
			const string& intrinsics_file = "/factory/intrinsics.yml",
			const string& extrinsics_file = "/factory/extrinsics.yml");
	virtual ~RectifiedKalamosCapture();

	virtual bool retrieve(cv::Mat& image, int channel = 0);

	GET(cv::Mat, Kl)
	GET(cv::Mat, Kr)
	GET(cv::Mat, Dl)
	GET(cv::Mat, Dr)
	GET(cv::Mat, R)
	GET(cv::Mat, T)
	GET(cv::Mat, Rl)
	GET(cv::Mat, Rr)
	GET(cv::Mat, Pl)
	GET(cv::Mat, Pr)
	GET(cv::Mat, Q)
	GET(cv::Mat, map1l)
	GET(cv::Mat, map1r)
	GET(cv::Mat, map2l)
	GET(cv::Mat, map2r)

private:
	cv::Mat _Kl, _Kr, _Dl, _Dr, _R, _T, _Rl, _Rr, _Pl, _Pr, _Q, _map1l, _map1r, _map2l, _map2r;
};

#endif
