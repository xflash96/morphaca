#ifndef _PARA_H
#define _PARA_H
#include <opencv2/opencv.hpp>

using namespace cv ;

class PARA
{
public:
	PARA() ;

	int N ;
	int type ;
	double warp_p ;
	double warp_a ;
	double warp_b ;
	Mat *lines ;
	double *weight ;
} ;

bool ParseParameters( Mat* &imgs, PARA &para, int argc, char *argv[] ) ; 
int ParseLine( Mat &lines, char *filename ) ;
#endif
