/* from http://www.opencv.org.cn/index.php */
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <ctime>
#include "parser.h"

using namespace cv ;
using namespace std ;
//using namespace Mat ;
#define Qfloat float


Mat CrossDissoving( Mat &img_src, Mat &img_dst, Qfloat t ) ;
Mat Warping( Mat &img_src, PARA &para, Mat line_src, Mat line_dst, int rows, int cols ) ;
inline Vec3f GaussianInterpolation( Mat &img_src, Qfloat normal_x, Qfloat normal_y, Qfloat sigma ) ;
inline Vec3f GaussianKernelInterpolation( Mat &img_src, Qfloat normal_x, Qfloat normal_y, Qfloat sigma ) ;
Qfloat countDisToSegment( Vec2f P, Vec2f Q, Vec2f X, Qfloat min_dis_to_line ) ;
Qfloat const PI=4*atan(1);

int main( int argc, char *argv[] )
{
	PARA para ;
	Mat img_src, img_dst ;
	

	if( argc < 7 || ParseParameters( img_src, img_dst, para, argc, argv ) == 0 )
	{
		cerr << "\n" ;
		cerr << "Usage: ./morph <src image> <dst image> <parameters>\n" ;
		cerr << "-T	The steps during the morphing.\n" ;
		cerr << "-SL	The lines of src image(Required)\n" ;
		cerr << "-DL	The lines of dst image(Required)\n" ;
		cerr << "-a	Parameter a of warpping\n" ;
		cerr << "-b	Parameter b of warpping\n" ;
		cerr << "-p	Parameter p of warpping\n" ;
		return -1 ;
	}

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
		sprintf( s, "test%02d.png", t ) ;
		imwrite( s, 255*morph ) ;
	}


	return 0;
}



Mat CrossDissoving( Mat &img_src, Mat &img_dst, Qfloat t )
{
	Mat mix = (1-t)*img_src+t*img_dst ;
	return mix ;
}

Mat Warping( Mat &img_src, PARA &para, Mat line_src, Mat line_dst, int rows, int cols )
{

	int n ;
	Qfloat u, v, w, W ;
	Qfloat unit ;
	Mat morph ;
	Vec2f X, _P, _Q, P, Q, QmP, _QmP, _X ;
	Vec3f pixel ;

	time_t st = time(0);

	morph = Mat::zeros( rows, cols, CV_32FC3 ) ;
	n = line_src.rows ;
	unit = sqrt( rows*rows+cols*cols ) ;
	for( int i=0 ; i<rows ; i++ ){
		time_t now = time(0);
		fprintf(stderr, "%d t=%d\n", i, (int)(now-st));
		for( int j=0 ; j<cols ; j++ )
		{
			//X = Vec2f( i/(Qfloat)rows, j/(Qfloat)cols ) ;
			X = Vec2f( i, j ) ;
			W = 0 ;
			for( int l=0 ; l<n ; l++ )
			{
				P = line_dst.at<Vec2f>(l, 0) ;
				Q = line_dst.at<Vec2f>(l, 1);
				_P = line_src.at<Vec2f>(l, 0) ;
				_Q = line_src.at<Vec2f>(l, 1) ;

				P.val[0] *= rows ;
				P.val[1] *= cols ;
				Q.val[0] *= rows ;
				Q.val[1] *= cols ;
				_P.val[0] *= img_src.rows ;
				_P.val[1] *= img_src.cols ;
				_Q.val[0] *= img_src.rows ;
				_Q.val[1] *= img_src.cols ;

				QmP = Q-P ;
				_QmP = _Q-_P ;
				u = (X-P).dot( QmP )/( QmP.val[0]*QmP.val[0]+QmP.val[1]*QmP.val[1] ) ;
				v = (X-P).dot( Vec2f( QmP.val[1], -QmP.val[0] ) )/norm(QmP) ;

				_X = _P + u*(_QmP) + v/norm( _QmP )*( Vec2f( _QmP.val[1], -_QmP.val[0] ) ) ;
				pixel = GaussianKernelInterpolation( img_src,
								_X.val[0],
								_X.val[1], 1 ) ;

				if( pixel.val[0] > 0 )
				{
					w = pow( pow( norm(QmP), para.warp_p )/( para.warp_a+countDisToSegment( P, Q, X, abs(v) ) ), para.warp_b ) ;
					morph.at<Vec3f>(i, j) = morph.at<Vec3f>(i, j) + w*pixel ; 
					W += w ;
				}
			}
			morph.at<Vec3f>(i, j) = 1/W*morph.at<Vec3f>(i, j) ;
		}
	}
	return morph ;
}

Vec3f GaussianInterpolation( Mat &img_src, Qfloat x, Qfloat y, Qfloat sigma )
{
	Qfloat w, W, dis ;
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
			dis = ( (quan_x-x)*(quan_x-x)+(quan_y-y)*(quan_y-y) )/sigma ;
			if( sqrt(dis)>2+1e-7 )
				continue ;
			if( quan_x<0 || quan_x>=rows || quan_y<0 || quan_y >= cols  )
				continue ;
			else
			{
				w = 1/( 2*PI*sigma)*exp( -0.5*dis ) ;
				W += w ;
				pixel = pixel+w*img_src.at<Vec3f>(quan_x, quan_y) ;
			}
		}
	if( abs( W ) < 1e-7 )
		Vec3f pixel = Vec3f(-1, -1, -1) ;
	else
		pixel = 1/W*pixel ;
	return pixel ;

}

Qfloat _GaussianKernel[] = {
	0.007306882745280776, 0.032747176537766653, 0.05399096651318806, 0.032747176537766653, 0.007306882745280776,
	0.032747176537766653, 0.146762663173739930, 0.24197072451914337, 0.146762663173739930, 0.032747176537766653,
	0.053990966513188060, 0.241970724519143370, 0.39894228040143270, 0.241970724519143370, 0.053990966513188060,
	0.032747176537766653, 0.146762663173739930, 0.24197072451914337, 0.146762663173739930, 0.032747176537766653,
	0.007306882745280776, 0.032747176537766653, 0.05399096651318806, 0.032747176537766653, 0.007306882745280776,
};
Vec3f GaussianKernelInterpolation( Mat &img_src, Qfloat x, Qfloat y, Qfloat sigma )
{
	Qfloat w, W, dis ;
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
			//dis = ( (quan_x-x)*(quan_x-x)+(quan_y-y)*(quan_y-y) )/sigma ;
			//if( sqrt(dis)>2+1e-7 )
			//	continue ;
			if( quan_x<0 || quan_x>=rows || quan_y<0 || quan_y >= cols  )
				continue ;
			else
			{
				int idx = (i+2)*5+j+2;
				w = _GaussianKernel[idx];
				W += w ;
				pixel = pixel+w*img_src.at<Vec3f>(quan_x, quan_y) ;
			}
		}
	if( abs( W ) < 1e-7 )
		Vec3f pixel = Vec3f(-1, -1, -1) ;
	else
		pixel = 1/W*pixel ;
	return pixel ;

}

Qfloat countDisToSegment( Vec2f P, Vec2f Q, Vec2f X, Qfloat min_dis_to_line )
{
	Vec2f QmP = Q-P ;
	if( (X-P).dot( QmP ) * ( X-Q ).dot(QmP) < 0 )
		return min_dis_to_line ;
	else
		return min( norm( X-P ), norm(X-Q) ) ;
}
