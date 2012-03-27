/* from http://www.opencv.org.cn/index.php */
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <ctime>
#include "parser.h"
#include "morph_lib.h"

using namespace cv ;
using namespace std ;
//using namespace Mat ;

int main( int argc, char *argv[] )
{
	PARA para ;
	Mat img_src, img_dst ;
	

	if( argc < 7 || ParseParameters( img_src, img_dst, para, argc, argv ) == 0 )
	{
		cerr << "\n" ;
		cerr << "Usage: ./morph <src image> <dst image> <parameters>\n" ;
		cerr << "-T	The steps during the morphing.\n" ;
		cerr << "-t	The type of interpolation.(Default is 0)\n" ;
		cerr << "        0: Direct interpolation.\n" ;
		cerr << "        1: Bilinear interpolation.\n" ;
		cerr << "        2: Gaussian interpolation.\n" ;
		cerr << "-SL	The lines of src image(Required)\n" ;
		cerr << "-DL	The lines of dst image(Required)\n" ;
		cerr << "-a	Parameter a of warpping\n" ;
		cerr << "-b	Parameter b of warpping\n" ;
		cerr << "-p	Parameter p of warpping\n" ;
		return -1 ;
	}

	time_t st = time(0);
	fprintf(stderr, "a%lf b%lf p%lf\n", para.warp_a, para.warp_b, para.warp_p);
 	for( int t = 0 ; t <= para.T ; t++ )
	{
		Mat src_warp = Warping( img_src, para, para.src_lines,
		                         (1-t/para.T)*para.src_lines+t/para.T*para.dst_lines, 
					 (int)( (1-t/para.T)*(img_src.rows)+t/para.T*(img_dst.rows)+1e-7 ), 
					 (int)( (1-t/para.T)*(img_src.cols)+t/para.T*(img_dst.cols)+1e-7 ) ) ;
					 
		Mat dst_warp = Warping( img_dst, para, para.dst_lines,
		                         (1-t/para.T)*para.src_lines+t/para.T*para.dst_lines, 
					 (int)( (1-t/para.T)*(img_src.rows)+t/para.T*(img_dst.rows)+1e-7 ), 
					 (int)( (1-t/para.T)*(img_src.cols)+t/para.T*(img_dst.cols)+1e-7 ) ) ;
					 
		Mat morph = CrossDissoving( src_warp, dst_warp, t/para.T ) ;
		char s[50] ;
		if( para.type == 0 )
			sprintf( s, "./direct%02d.png", t ) ;
		else if( para.type == 1 )
			sprintf( s, "./bilinear%02d.png", t ) ;
		else if( para.type == 2 )
			sprintf( s, "./gaussian%02d.png", t ) ;
		imwrite( s, 255*morph ) ;

		time_t now = time(0);
		fprintf(stderr, "t=%d\n", (int)(now-st));
	}

	return 0;
}

