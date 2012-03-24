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

int main( int argc, char *argv[] )
{
	PARA para ;
	Mat *imgs ;
	Mat warp, mix, dst_line ;
	int dst_row, dst_col ;
	double tmp_row, tmp_col ;

	if( ParseParameters( imgs, para, argc, argv ) == 0 )
	{
		cerr << "\n" ;
		cerr << "Usage: ./polymorph <#imgs> <img1>...<imgN> <line1>...<lineN>\n" ;
		cerr << "-a	Parameter a of warpping\n" ;
		cerr << "-b	Parameter b of warpping\n" ;
		cerr << "-p	Parameter p of warpping\n" ;
		cerr << "-wi    The weight of image i, default is 1\n" ;
		return -1 ;
	}

	dst_lines = Mat::zeros( (para.lines[0]).rows, (para.lines[0]).cols, CV_32FC2 ) ;
	tmp_row = 0 ;
	tmp_col = 0 ;
	for( int i = 0 ; i<para.N ; i++ )
	{
		dst_lines = dst_lines + para.weight[i]*para.lines[i] ;
		tmp_row = tmp_row + para.weight[i]*imgs[i].rows ;
		tmp_cols = tmp_cols + para.weight[i]*imgs[i].cols ;
	}
	dst_row = int( tmp_row+1e-7 ) ;
	dst_col = int( tmp_col+1e-7 ) ;
	mix = Mat::zeros( dst_rows, dst_cols, CV_32FC3 ) ;
 	for( int i = 0 ; i < para.N ; i++ )
	{
		
	}

 	for( int i = 0 ; i < para.N ; t++ )
	{
		Mat src_warp = Warping( img_src, para, para.src_lines,
		                         (1-t/para.T)*para.src_lines+t/para.T*para.dst_lines, 
					 (int)( (1-t/para.T)*(img_src.rows)+t/para.T*(img_dst.rows)+1e-7 ), 
					 (int)( (1-t/para.T)*(img_src.cols)+t/para.T*(img_dst.cols)+1e-7 ) ) ;
					 
		Mat dst_warp = Warping( img_dst, para, para.dst_lines,
		                         (1-t/para.T)*para.src_lines+t/para.T*para.dst_lines, 
					 (int)( (1-t/para.T)*(img_src.rows)+t/para.T*(img_dst.rows)+1e-7 ), 
					 (int)( (1-t/para.T)*(img_src.cols)+t/para.T*(img_dst.cols)+1e-7 ) ) ;
					 
		char s[50] ;
		sprintf( s, "test%02d.png", t ) ;
		imwrite( s, 255*morph ) ;

		fprintf(stderr, "t=%d\n", (int)(now-st));
	}


	return 0;
}

