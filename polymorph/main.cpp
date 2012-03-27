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
	Mat warp, morph, dst_lines ;
	int dst_rows, dst_cols ;
	double tmp_rows, tmp_cols ;

	if( argc<4 || ParseParameters( imgs, para, argc, argv ) == 0 )
	{
		cerr << "\n" ;
		cerr << "Usage: ./polymorph <#imgs> <img1>...<imgN> <line1>...<lineN> <parameters>\n" ;
		cerr << "-t	The type of interpolation.(Default is 0)\n" ;
		cerr << "        0: Direct interpolation.\n" ;
		cerr << "        1: Bilinear interpolation.\n" ;
		cerr << "        2: Gaussian interpolation.\n" ;
		cerr << "-a	Parameter a of warpping\n" ;
		cerr << "-b	Parameter b of warpping\n" ;
		cerr << "-p	Parameter p of warpping\n" ;
		cerr << "-wi    The weight of image i, default is 1\n" ;
		return -1 ;
	}

	dst_lines = Mat::zeros( (para.lines[0]).rows, (para.lines[0]).cols, CV_32FC2 ) ;
	tmp_rows = 0 ;
	tmp_cols = 0 ;
	for( int i = 0 ; i<para.N ; i++ )
	{
		dst_lines = dst_lines + para.weight[i]*para.lines[i] ;
		tmp_rows= tmp_rows+ para.weight[i]*imgs[i].rows ;
		tmp_cols = tmp_cols + para.weight[i]*imgs[i].cols ;
	}
	dst_rows= int( tmp_rows+1e-7 ) ;
	dst_cols= int( tmp_cols+1e-7 ) ;
	morph = Mat::zeros( dst_rows, dst_cols, CV_32FC3 ) ;
 	for( int i = 0 ; i < para.N ; i++ )
	{
		warp = Warping( imgs[i], para, para.lines[i], dst_lines, dst_rows, dst_cols) ;
		morph = morph+para.weight[i]*warp ;
	}
	char s[50] ;
	if( para.type == 0 )
		sprintf( s, "./poly_direct.png" ) ;
	else if( para.type == 1 )
		sprintf( s, "./poly_bilinear.png" ) ;
	else if( para.type == 2 )
		sprintf( s, "./poly_gaussian.png" ) ;
	imwrite( s, 255*morph ) ;

	return 0;
}

