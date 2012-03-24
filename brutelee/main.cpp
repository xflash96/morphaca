/* from http://www.opencv.org.cn/index.php */
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <ctime>

using namespace cv ;
using namespace std ;
//using namespace Mat ;
#define Qfloat float
const float EPSILON = 1e-6;

void MakeFeature(Mat &ctrl_points, int side_n);
void drawFeature(const Mat &ctrl_points, int side_n, Mat &out_img, int special);
void MakeDirection(Mat &direction, int n_dir, double unit);

inline void WrapToLine( Mat &morph, Mat &W, Mat &img_src, Vec2f P, Vec2f Q, Vec2f _P, Vec2f _Q, int rows, int cols, int sign);
void Normalize(Mat &result, Mat &morph, Mat &W );

Qfloat SumDiffMat(Mat &A, Mat &B);

Mat SearchWraps( Mat &direction, Mat &img_src, Mat &img_dst, Mat &ctrl_points, int rows, int cols, int side_n) ;
inline Qfloat countDisToSegment( Vec2f P, Vec2f Q, Vec2f X, Qfloat min_dis_to_line ) ;

Qfloat const PI=(float)(4*atan(1));

int main( int argc, char *argv[] )
{
	Mat img_src, img_dst ;
	Mat ctrl_points;
	
	Mat raw_img_src = imread( argv[1], 1) ;
	Mat raw_img_dst = imread( argv[2], 1) ;
	raw_img_src.convertTo( img_src, CV_32FC3, 1/255.0 ) ;
	raw_img_dst.convertTo( img_dst, CV_32FC3, 1/255.0 ) ;

	int side_n = 16;
	Mat direction;
	MakeFeature(ctrl_points, side_n);
	MakeDirection(direction, 4, 1./side_n/6);
	
	time_t st = time(0);

	int T = 10;
 	for( int t = 0 ; t <= T ; t++ )
	{
		Mat diff = SearchWraps( direction, img_src, img_dst, ctrl_points,
					 img_dst.rows, img_dst.cols, side_n);
					 
		time_t now = time(0);
		fprintf(stderr, "t = %d\n", (int)(now-st));
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

void drawFeature(Mat &ctrl_points, int side_n, Mat &out_img, int special=-1)
{
	const Scalar special_color = Scalar(255,0,0);
	const Scalar normal_color = Scalar(255,0,255);
	Scalar color;

	for( int i=0; i<side_n; i++ ){
		for( int j=0; j<side_n; j++ ){
			int idx = i*side_n+j;
			if( j+1<side_n){
				if( idx==special || idx+1==special )
					color = special_color;
				else
					color = normal_color;
				drawLine(out_img, 
						ctrl_points.at<Vec2f>(0, idx),
						ctrl_points.at<Vec2f>(0, idx+1),
						color);
			}
			if( i+1<side_n ){
				if( idx==special || idx+side_n==special )
					color = special_color;
				else
					color = normal_color;
				drawLine(out_img, 
						ctrl_points.at<Vec2f>(0, idx),
						ctrl_points.at<Vec2f>(0, idx+side_n),
						color);
			}
		}
	}
}

void MakeFeature(Mat &ctrl_points, int side_n)
{

	ctrl_points = Mat::zeros(2, side_n*side_n, CV_32FC2);
	Vec2f* ctrl_points_0 = ctrl_points.ptr<Vec2f>(0);
	Vec2f* ctrl_points_1 = ctrl_points.ptr<Vec2f>(1);
	float unit = 1.f/(side_n-1);
	for(int i=0; i<side_n; i++){
		for(int j=0; j<side_n; j++){
			int idx = (side_n*i+j);
			ctrl_points_0[idx] = Vec2f(i*unit, j*unit);
			ctrl_points_1[idx] = Vec2f(i*unit, j*unit);
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

/* Let idx = (n*i+j)
 *
 *           idx-n
 *           |
 *           |
 * idx-1 --------- idx+1
 *           |       
 *           |
 *           idx+n
 */
void WrapToNeighborLine(Mat &morph, Mat &W, Mat &img_src, Mat &ctrl_points, Vec2f _P, int rows, int cols, int side_n, int i, int j, int sign)
{
	int idx = i*side_n+j;

	Vec2f P = ctrl_points.at<Vec2f>(1,idx);
	if( i-1>=0 ){
		Vec2f _U = ctrl_points.at<Vec2f>(0,idx-side_n);
		Vec2f U = ctrl_points.at<Vec2f>(1,idx-side_n);
		WrapToLine(morph, W, img_src, U, P, _U, _P, rows, cols, sign);
	}
	if( i+1<side_n ){
		Vec2f _D = ctrl_points.at<Vec2f>(0,idx+side_n);
		Vec2f D = ctrl_points.at<Vec2f>(1,idx+side_n);
		WrapToLine(morph, W, img_src, P, D, _P, _D, rows, cols, sign);
	}
	if( j-1>=0 ){
		Vec2f _L = ctrl_points.at<Vec2f>(0,idx-1);
		Vec2f L = ctrl_points.at<Vec2f>(1,idx-1);
		WrapToLine(morph, W, img_src, L, P, _L, _P, rows, cols, sign);
	}
	if( j+1<side_n ){
		Vec2f _R = ctrl_points.at<Vec2f>(0,idx+1);
		Vec2f R = ctrl_points.at<Vec2f>(1,idx+1);
		WrapToLine(morph, W, img_src, P, R, _P, _R, rows, cols, sign);
	}
}

void WrapToAllLine(Mat &morph, Mat &W, Mat &img_src, Mat &ctrl_points, int rows, int cols, int side_n)
{
	morph = Mat::zeros(rows, cols, CV_32FC3);
	W = Mat::zeros(rows, cols, CV_32FC1);

	for( int i=0; i<side_n; i++ ){
		for( int j=0; j<side_n; j++){
			int idx = i*side_n+j;
			Vec2f P = ctrl_points.at<Vec2f>(1,idx);
			Vec2f _P = ctrl_points.at<Vec2f>(0,idx);
			if( j+1<side_n){
				Vec2f _R = ctrl_points.at<Vec2f>(0,idx+1);
				Vec2f R = ctrl_points.at<Vec2f>(1,idx+1);
				WrapToLine(morph, W, img_src, P, R, _P, _R, rows, cols, 1);
			}
			if( i+1<side_n ){
				Vec2f _D = ctrl_points.at<Vec2f>(0,idx+side_n);
				Vec2f D = ctrl_points.at<Vec2f>(1,idx+side_n);
				WrapToLine(morph, W, img_src, P, D, _P, _D, rows, cols, 1);
			}
		}
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

Mat SearchWraps( Mat &direction, Mat &img_src, Mat &img_dst, Mat &ctrl_points, int rows, int cols, int side_n)
{

	Mat morph, _morph, W, _W, norm_morph;
	float min_diff;

	_morph = Mat::zeros(img_dst.rows, img_dst.cols, CV_32FC3);
	norm_morph = Mat::zeros(img_dst.rows, img_dst.cols, CV_32FC3);
	_W = Mat::zeros(img_dst.rows, img_dst.cols, CV_32FC1);

	int n_directions = direction.cols;

	WrapToAllLine(morph, W, img_src, ctrl_points, rows, cols, side_n);
	min_diff = 1e10;

	for( int i=0; i<side_n; i++ ){
	for( int j=0; j<side_n; j++ ){
		int l = i*side_n+j;
		Vec2f _P = ctrl_points.at<Vec2f>(0, l);
		for( int k=0; k<n_directions; k++){
			Vec2f d = direction.at<Vec2f>(0,k);
			Vec2f t = _P+d;
			if(t.val[0] < 0 || t.val[1] < 0 || t.val[0] >= 1. || t.val[1] >= 1.)
				continue;
			morph.copyTo(_morph);
			W.copyTo(_W);

			WrapToNeighborLine( _morph, _W, img_src, ctrl_points, _P, rows, cols, side_n, i, j, -1);
			WrapToNeighborLine( _morph, _W, img_src, ctrl_points, _P+d, rows, cols, side_n, i, j, +1);
			imshow("W", _W);
			waitKey();
			Normalize(norm_morph, _morph, _W);


			float diff_now = SumDiffMat(norm_morph, img_dst);
			fprintf(stderr, "now=%f min=%f\n", diff_now, min_diff);
				drawFeature(ctrl_points, side_n, norm_morph, l);
			if( diff_now < min_diff ){
				_morph.copyTo(morph);
				_W.copyTo(W);
				ctrl_points.at<Vec2f>(0,l) = _P+d;
				_P = _P+d;

				fprintf(stderr, "l = %d d = %f %f\n", l, d.val[0], d.val[1]);
				min_diff = diff_now;
#if 1
				imshow("morph", norm_morph);
				norm_morph = abs(norm_morph-img_dst);
				drawFeature(ctrl_points, side_n, norm_morph, l);
				imshow("diff", norm_morph);
				waitKey(100);
#endif
				break;
			}
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
					w = 1/EPSILON;
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
#if 1
	Vec2f QmP = Q-P ;
	if( (X-P).dot( QmP ) * ( X-Q ).dot(QmP) < 0 )
		return min_dis_to_line*min_dis_to_line ;
	else
#endif
	return min((X-P).dot(X-P), (X-Q).dot(X-Q));
//	return min( norm( X-P ), norm(X-Q) ) ;
}
