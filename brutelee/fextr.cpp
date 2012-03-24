/* from http://www.opencv.org.cn/index.php */
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/calib3d/calib3d.hpp>

using namespace std;
using namespace cv;

unsigned int hamdist2(unsigned char* x, unsigned char*y, size_t size)
{
	HammingLUT lut;
	unsigned int result;

	result = lut(x, y, (int)size);
	return result;

}

void nn_search(
		vector<KeyPoint>& keys1,
		Mat& descr1,
		vector<KeyPoint>& keys2,
		Mat& descr2,
		vector<DMatch>& matches)
{
	for (int i=0; i<(int)keys2.size(); i++){
		unsigned int min_dist = INT_MAX;
		int min_idx = -1;
		unsigned char* query_feat = descr2.ptr(i);
		for (int j=0; j<(int)keys1.size(); j++){
			unsigned char* train_feat = descr1.ptr(j);
			unsigned int dist = hamdist2(query_feat, train_feat, 32);
			if (dist < min_dist){
				min_dist = dist;
				min_idx = j;
			}
		}
		if (min_dist < 400){
			matches.push_back(DMatch(i, min_idx, 0, (float)min_dist));
		}
	}
}

int main(int argc, char** argv)
{
#if 0
	if (argc<3) {
		return 0;
	}
	string src_image_name = argv[1];
	string dst_image_name = argv[2];
#else
	string src_image_name = "../morph/imm/01-1m.jpg";
	string dst_image_name = "../morph/imm/03-1m.jpg";
#endif

	Mat src_image = imread(src_image_name);
	Mat dst_image = imread(dst_image_name);

	if (src_image.empty() || dst_image.empty()) {
		return -1;
	}

#if 0
	SiftFeatureDetector detector(0.08,0.08);
	vector<KeyPoint> src_keys, dst_keys;
	detector.detect(src_image, src_keys);
	detector.detect(dst_image, dst_keys);
	
	// computing descriptors
	Mat src_descr, dst_descr;
	SiftDescriptorExtractor extractor;
	Mat descriptors1, descriptors2;
	extractor.compute(src_image, src_keys, src_descr);
	extractor.compute(dst_image, dst_keys, dst_descr);
	
	// matching descriptors
	BruteForceMatcher<L2<float> > matcher;
	vector<DMatch> matches;
	matcher.match(src_descr, dst_descr, matches);
#else
	ORB dst_orb(100, ORB::CommonParams(1.2f, 1));
	ORB src_orb(100, ORB::CommonParams(1.2f, 1));

	vector<KeyPoint> src_keys, dst_keys;
	Mat src_descr, dst_descr;

	src_orb(src_image, Mat(), src_keys, src_descr, false);
	dst_orb(dst_image, Mat(), dst_keys, dst_descr, false);

	vector<DMatch> matches;

	nn_search(src_keys, src_descr, dst_keys, dst_descr, matches);
#endif

	Mat match_img;
	drawMatches(src_image, src_keys, dst_image, dst_keys, matches,
			match_img);
//			,CV_RGB(0,255,0), CV_RGB(0,0,255));
	string win_name = "Match";
	namedWindow(win_name, WINDOW_AUTOSIZE);
	imshow(win_name, match_img);
	waitKey();

	return 0;
}
