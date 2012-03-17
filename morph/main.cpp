/* from http://www.opencv.org.cn/index.php */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <opencv2/opencv.hpp>
#include "parser.h"

using namespace cv ;
using namespace std ;
//using namespace Mat ;


Mat CrossDissoving( Mat &img_src, Mat &img_dst, double t ) ;
Mat Warpping( Mat &img_src, PARA &para, Mat line_src, Mat line_dst, int rows, int cols ) ;
Vec3f GaussianInterpolation( Mat &img_src, double normal_x, double normal_y, double sigma ) ;
double const PI=4*atan(1);

int main( int argc, char *argv[] )
{
	PARA para ;
	Mat img_src, img_dst ;
	

	if( ParseParameters( img_src, img_dst, para, argc, argv ) == 0 )
		return -1 ;

 	for( int t = 0 ; t <= para.T ; t++ )
	{
		Mat src_warp = Warpping( img_src, para, para.src_lines,
		                         (1-t/para.T)*para.src_lines+t/para.T*para.dst_lines, 
					 (int)( (1-t/para.T)*(img_src.rows)+t/para.T*(img_dst.rows)+1e-7 ), 
					 (int)( (1-t/para.T)*(img_src.cols)+t/para.T*(img_dst.cols)+1e-7 ) ) ;
					 
		Mat dst_warp = Warpping( img_dst, para, para.dst_lines,
		                         (1-t/para.T)*para.src_lines+t/para.T*para.dst_lines, 
					 (int)( (1-t/para.T)*(img_src.rows)+t/para.T*(img_dst.rows)+1e-7 ), 
					 (int)( (1-t/para.T)*(img_src.cols)+t/para.T*(img_dst.cols)+1e-7 ) ) ;
					 
		Mat morph = CrossDissoving( src_warp, dst_warp, t/para.T ) ;
		char s[50] ;
		sprintf( s, "/home/student/97/b97018/htdocs/test%02d.png", t ) ;
		imwrite( s, 255*morph ) ;
	}


	return 0;
}



Mat CrossDissoving( Mat &img_src, Mat &img_dst, double t )
{
	Mat mix = (1-t)*img_src+t*img_dst ;
	return mix ;
}

Mat Warpping( Mat &img_src, PARA &para, Mat line_src, Mat line_dst, int rows, int cols )
{

	int n ;
	double u, v, w, W ;
	Mat morph ;
	Vec2f X, _P, _Q, P, Q, QmP, _QmP, _X ;
	Vec3f pixel ;

	morph = Mat::zeros( rows, cols, CV_32FC3 ) ;
	n = line_src.rows ;
	for( int i=0 ; i<rows ; i++ )
		for( int j=0 ; j<cols ; j++ )
		{
			X = Vec2f( i/(double)rows, j/(double)cols ) ;
			W = 0 ;
			for( int l=0 ; l<n ; l++ )
			{
				P = line_dst.at<Vec2f>(l, 0) ;
				Q = line_dst.at<Vec2f>(l, 1);
				_P = line_src.at<Vec2f>(l, 0) ;
				_Q = line_src.at<Vec2f>(l, 1) ;
				QmP = Q-P ;
				_QmP = _Q-_P ;
				u = (X-P).dot( QmP )/( QmP.val[0]*QmP.val[0]+QmP.val[1]*QmP.val[1] ) ;
				v = (X-P).dot( Vec2f( QmP.val[1], -QmP.val[0] ) )/norm(QmP) ;
				_X = _P + u*(_QmP) + v/norm( _QmP )*( Vec2f( _QmP.val[1], -_QmP.val[0] ) ) ;
				w = pow( pow( abs(u), para.warp_p )/( para.warp_a+abs(v) ), para.warp_b ) ;
				pixel = GaussianInterpolation( img_src,
								_X.val[0]*img_src.rows,
								_X.val[1]*img_src.cols, 1 ) ;
				Vec3f pixel1 ;
				pixel1 = (img_src.at<Vec3f>)(i, j) ;
				if( pixel.val[0] > 0 )
				{
					morph.at<Vec3f>(i, j) = morph.at<Vec3f>(i, j) + w*pixel ; 
					W += w ;
				}
			}
			morph.at<Vec3f>(i, j) = 1/W*morph.at<Vec3f>(i, j) ;
		}
	return morph ;
}

Vec3f GaussianInterpolation( Mat &img_src, double x, double y, double sigma )
{
	double w, W ;
	int quan_x, quan_y, rows, cols ;
	rows = img_src.rows ;
	cols = img_src.cols ;
	Vec3f pixel = Vec3f(0, 0, 0) ;
	W = 0 ;
	for( int i=-2 ; i<=2 ; i++ )
		for( int j=-2 ; j<=2 ; j++ )
		{
			quan_x = (int)( x+i+1e-7 ) ;
			quan_y = (int)( y+j+1e-7 ) ;
			if( quan_x<0 || quan_x>=rows || quan_y<0 || quan_y >= cols )
				continue ;
			w = 1/(2*PI*sigma)*exp( -1/(2*sigma)*( (quan_x-x)*(quan_x-x)+(quan_y-y)*(quan_y-y) ) ) ;
			W += w ;
			pixel = pixel+w*img_src.at<Vec3f>(quan_x, quan_y) ;
		}
	if( abs( W ) < 1e-7 )
		Vec3f pixel = Vec3f(-1, -1, -1) ;
	else
	{
		pixel = 1/W*pixel ;
	}
	return pixel ;
}
