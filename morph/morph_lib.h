#ifndef _MORPH_H_
#define _MORPH_H_
#include <opencv2/opencv.hpp>
#include "parser.h"
#define Qfloat float
using namespace cv ;
using namespace std ;

Mat CrossDissoving( Mat &img_src, Mat &img_dst, Qfloat t ) ;
Mat Warping( Mat &img_src, PARA &para, Mat line_src, Mat line_dst, int rows, int cols ) ;
inline Vec3f GaussianInterpolation( Mat &img_src, Qfloat normal_x, Qfloat normal_y, Qfloat sigma ) ;
inline Vec3f GaussianKernelInterpolation( Mat &img_src, Qfloat normal_x, Qfloat normal_y, Qfloat sigma ) ;
inline Qfloat countDisToSegment( Vec2f P, Vec2f Q, Vec2f X, Qfloat min_dis_to_line ) ;
#endif
