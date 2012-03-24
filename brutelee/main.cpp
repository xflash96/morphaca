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
const float EPSILON = 1e-6;

void MakeFeature(Mat &features, int side_n);
void drawFeature(const Mat &line_src, const Mat &line_dst, Mat &out_img, int special);
void MakeDirection(Mat &direction, int n_dir, double unit);

void WrapToLine( Mat &morph, Mat &W, Mat &img_src, Vec2f P, Vec2f Q, Vec2f _P, Vec2f _Q, int rows, int cols, int sign);
void Normalize(Mat &result, Mat &morph, Mat &W );

Qfloat SumDiffMat(Mat &A, Mat &B);

Mat SearchWraps( Mat &direction, Mat &img_src, Mat &img_dst, PARA &para, Mat &line_src, Mat &line_dst, int rows, int cols) ;
inline Qfloat countDisToSegment( Vec2f P, Vec2f Q, Vec2f X, Qfloat min_dis_to_line ) ;

Qfloat const PI=(float)(4*atan(1));

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
	int side_n = 4;
	Mat direction;
	MakeFeature(para.src_lines, side_n);
	MakeFeature(para.dst_lines, side_n);
	MakeDirection(direction, 4, 1./side_n/6);
	

	time_t st = time(0);

 	for( int t = 0 ; t <= para.T ; t++ )
	{
		Mat diff = SearchWraps( direction, img_src, img_dst, para, para.src_lines, para.dst_lines,
					 img_dst.rows, img_dst.cols);
					 
		char s[50] ;
		sprintf( s, "test%02d.png", t ) ;
		imwrite( s, 255*diff ) ;

		time_t now = time(0);
		fprintf(stderr, "t=%d\n", (int)(now-st));
	}


	return 0;
}

void drawLine(Mat &out_img, const Vec2f &src, const Vec2f &dst, const Scalar &color)
{
	int rows = out_img.rows;
	int cols = out_img.cols;
	Point st, ed;
	Vec2f t;
	st.y = src.val[0]*rows;
	st.x = src.val[1]*cols;
	ed.y = dst.val[0]*rows;
	ed.x = dst.val[1]*cols;
	line(out_img, st, ed, color);
}

void drawFeature(const Mat &line_src, const Mat &line_dst, Mat &out_img, int special=-1)
{
	const Scalar src_special = Scalar(255,0,0);
	const Scalar src_color = Scalar(255,0,255);
	const Scalar dst_special = Scalar(0,0,255);
	const Scalar dst_color = Scalar(0,255,255);
	const Vec2f *src_st, *src_ed;
	const Vec2f *dst_st, *dst_ed;
	src_st = line_src.ptr<Vec2f>(0);
	src_ed = line_src.ptr<Vec2f>(1);
	dst_st = line_dst.ptr<Vec2f>(0);
	dst_ed = line_dst.ptr<Vec2f>(1);
	for(int i=0; i<line_src.cols; i++){
		drawLine(out_img, src_st[i], src_ed[i], src_color);
			//drawLine(out_img, dst_st[i], dst_ed[i], dst_color);
	}
	drawLine(out_img, src_st[special], src_ed[special], src_special);
}

void MakeFeature(Mat &features, int n)
{
	features = Mat::zeros(2, 2*n*n, CV_32FC2);
	Vec2f* features_0 = features.ptr<Vec2f>(0);
	Vec2f* features_1 = features.ptr<Vec2f>(1);
	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			int idx = 2*(n*i+j);
			// down 
			features_0[idx] = Vec2f(i*1.f/n, j*1.f/n);
			features_1[idx] = Vec2f((i+1)*1.f/n, j*1.f/n);
			// right
			features_0[idx+1] = Vec2f(i*1.f/n, j*1.f/n);
			features_1[idx+1] = Vec2f(i*1.f/n, (j+1)*1.f/n);
		}
	}
}

void MakeDirection(Mat &direction, int n_dir, double unit)
{
	double delta = 2*PI/n_dir;
	// rotation matrix
	float _unit_rot[] = {cos(delta), sin(delta), -sin(delta), cos(delta)};
	Mat unit_rot = Mat(2, 2, CV_32FC1, _unit_rot);
	// initial vector
	float _v[] = {1.f, 0};
	Mat v = Mat(2, 1, CV_32FC1, _v);;
	v *= unit;

	direction = Mat::zeros(1, n_dir, CV_32FC2);
	for(int i=0; i<n_dir; i++){
		direction.at<Vec2f>(0, i) = Vec2f(v.at<float>(0,0), v.at<float>(1,0));
		v = unit_rot*v;
	}
}

void WrapToAllLine(Mat &morph, Mat &W, Mat &img_src, PARA &para, Mat &line_src, Mat &line_dst, int rows, int cols)
{
	morph = Mat::zeros(rows, cols, CV_32FC3);
	W = Mat::zeros(rows, cols, CV_32FC1);
	int n = line_src.cols;
	Vec2f P, Q, _P, _Q, d;

	for( int l=0; l<n; l++ ){
		P = line_dst.at<Vec2f>(0,l);
		Q = line_dst.at<Vec2f>(1,l);
		_P = line_src.at<Vec2f>(0,l);
		_Q = line_src.at<Vec2f>(1,l);
		WrapToLine(morph, W, img_src, P, Q, _P, _Q, rows, cols, +1);
	}
//	Normalize(morph, W);
}

float SumDiffMat(Mat &A, Mat &B)
{
	Mat d = abs(A-B);
	int rows = d.rows;
	int cols = d.cols;
	float s= 0.;
	for( int i=0; i<rows; i++ ){
		Vec3f *p = d.ptr<Vec3f>(i);
		for( int j=0; j<cols; j++ ){
			s += p[j].val[0] + p[j].val[1] + p[j].val[2];
		}
	}
	return s;
}

Mat SearchWraps( Mat &direction, Mat &img_src, Mat &img_dst, PARA &para, Mat &line_src, Mat &line_dst, int rows, int cols)
{

	int n, nd;
	Mat morph, _morph, W, _W, norm_morph;
	Vec2f P, Q, _P, _Q, d, t;
	float min_diff;

	_morph = Mat::zeros(img_dst.rows, img_dst.cols, CV_32FC3);
	norm_morph = Mat::zeros(img_dst.rows, img_dst.cols, CV_32FC3);
	_W = Mat::zeros(img_dst.rows, img_dst.cols, CV_32FC1);

	n = line_src.cols;
	nd = direction.cols;

	WrapToAllLine(morph, W, img_src, para, line_src, line_dst, rows, cols);
	min_diff = 1e10;

	for( int l=0; l<n; l++){
		P = line_dst.at<Vec2f>(0,l);
		Q = line_dst.at<Vec2f>(1,l);
		_P = line_src.at<Vec2f>(0,l);
		_Q = line_src.at<Vec2f>(1,l);
		for( int k=0; k<nd; k++){
			d = direction.at<Vec2f>(0,k);
			t = _P+d;
			if(t.val[0] < 0 || t.val[1] < 0 || t.val[0] >= rows || t.val[1] >= cols)
				continue;
			morph.copyTo(_morph);
			W.copyTo(_W);
			// reverse wrap
			WrapToLine( _morph, _W, img_src, P, Q, _P, _Q, rows, cols, -1);
			WrapToLine( _morph, _W, img_src, P, Q, _P+d, _Q, rows, cols, +1);
			Normalize(norm_morph, _morph, _W);
			//imshow("norm_morph", norm_morph);
			//imshow("W", W);
			//waitKey();
			float diff_now = SumDiffMat(norm_morph, img_dst);
			fprintf(stderr, "now=%f min=%f\n", diff_now, min_diff);
			if( diff_now < min_diff ){
				_morph.copyTo(morph);
				_W.copyTo(W);
				line_src.at<Vec2f>(0,l) = _P+d;
				_P = _P+d;
				min_diff = diff_now;
				break;
			}
		}
		for( int k=0; k<nd; k++){
			d = direction.at<Vec2f>(0,k);
			t = _Q+d;
			if(t.val[0] < 0 || t.val[1] < 0 || t.val[0] >= rows || t.val[1] >= cols)
				continue;
			morph.copyTo(_morph);
			W.copyTo(_W);
			// reverse wrap
			WrapToLine( _morph, _W, img_src, P, Q, _P, _Q, rows, cols, -1);
			WrapToLine( _morph, _W, img_src, P, Q, _P, _Q+d, rows, cols, +1);
			Normalize(norm_morph, _morph, _W);
			//imshow("norm_morph", norm_morph);
			//imshow("W", W);
			//waitKey();
			float diff_now = SumDiffMat(norm_morph, img_dst);
			fprintf(stderr, "now=%f min=%f\n", diff_now, min_diff);
			if( diff_now < min_diff ){
				_morph.copyTo(morph);
				_W.copyTo(W);
				line_src.at<Vec2f>(1,l) = _Q+d;
				_Q = _Q+d;
				min_diff = diff_now;
				fprintf(stderr, "l = %d d = %f %f\n", l, d.val[0], d.val[1]);
				imshow("morph", norm_morph);
				norm_morph = abs(norm_morph-img_dst);
				drawFeature(line_src, line_dst, norm_morph, 0);
				imshow("diff", norm_morph);
				waitKey(100);
				break;
			}
		}
	}
	return morph;
}

void Normalize( Mat &result, Mat &morph, Mat &W )
{
	int rows = morph.rows;
	int cols = morph.cols;
	for( int i=0; i<rows; i++)
		for( int j=0; j<cols; j++)
			result.at<Vec3f>(i, j) = 1/W.at<float>(i,j)*morph.at<Vec3f>(i, j) ;
}

void WrapToLine( Mat& morph, Mat &W, Mat &img_src, Vec2f P, Vec2f Q, Vec2f _P, Vec2f _Q, int rows, int cols, int sign)
{
	Vec2f X, QmP, _QmP, _X ;
	Qfloat u, v, w;
	Vec3f pixel ;
	Vec2f u_param, v_param, x_param;

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
	u_param = QmP*(1/( QmP.val[0]*QmP.val[0]+QmP.val[1]*QmP.val[1] )) ;
	v_param = Vec2f( QmP.val[1], -QmP.val[0] )*(1/norm(QmP)) ;
	x_param = 1/norm(_QmP)*Vec2f(_QmP.val[1], -_QmP.val[0]);

	for( int i=0 ; i<rows ; i++ ){
		Vec3f* morph_i = morph.ptr<Vec3f>(i);
		float* W_i = W.ptr<float>(i);
		for( int j=0 ; j<cols ; j++ )
		{
			//X = Vec2f( i/(Qfloat)rows, j/(Qfloat)cols ) ;
			X = Vec2f( i, j ) ;

			u = (X-P).dot( u_param ) ;
			v = (X-P).dot( v_param ) ;

			_X = _P + u*(_QmP) + v*x_param ;

			int _x = (int) _X.val[0], _y = (int) _X.val[1];

			if(_x>=0 && _y>=0 && _x<img_src.rows && _y<img_src.cols)
			{
				float dist = countDisToSegment( P, Q, X, abs(v) );
				if( dist < EPSILON )
					w = 1;
				else
					w = 1/dist;
				pixel = img_src.at<Vec3f>(_x, _y);
				morph_i[j] += w*pixel*sign ; 
				W_i[j] += w*sign ;
			}
		}
	}
}

Qfloat countDisToSegment( Vec2f P, Vec2f Q, Vec2f X, Qfloat min_dis_to_line )
{
#if 0
	Vec2f QmP = Q-P ;
	if( (X-P).dot( QmP ) * ( X-Q ).dot(QmP) < 0 )
		return min_dis_to_line*min_dis_to_line ;
	else
#endif
	return min((X-P).dot(X-P), (X-Q).dot(X-Q));
//	return min( norm( X-P ), norm(X-Q) ) ;
}
