#ifndef _PARA_H
#define _PARA_H
#include <opencv2/opencv.hpp>

using namespace cv ;

class PARA
{
public:
	PARA() ;

	int type ;
	double T ;
	int N ;
	double warp_p ;
	double warp_a ;
	double warp_b ;
	Mat src_lines ;
	Mat dst_lines ;
} ;

bool ParseParameters( Mat &img_src, Mat &img_dst, PARA &para,  int argc, char *argv[] ) ;
int ParseLine( Mat &lines, char *filename ) ;
#endif
