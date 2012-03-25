#include "parser.h"
#include <map>
#include <cctype>
#include <cstdio>

using namespace std ;

double const PI=4*atan(1);
PARA::PARA()
{
	T = 10 ;
	warp_p = 0 ;
	warp_a = 0.1 ;
	warp_b = 1 ;
	type = 0 ;
}

bool ParseParameters( Mat &img_src, Mat &img_dst, PARA &para,  int argc, char *argv[] ) 
{
	Mat raw_img_src = imread( argv[1], 1) ;
	Mat raw_img_dst = imread( argv[2], 1) ;
	int check_count[2] ;

	check_count[0] = 0 ;
	check_count[1] = 0 ;
	para = PARA() ;
	if( raw_img_src.empty() || raw_img_dst.empty() )
		return 0 ;
	raw_img_src.convertTo( img_src, CV_32FC3, 1/255.0 ) ;
	raw_img_dst.convertTo( img_dst, CV_32FC3, 1/255.0 ) ;
	for( int i=3 ; i<argc ; i+=2 )
	{
		if( !strcmp( "-T", argv[i] ) )
			para.T = (double)( atoi( argv[i+1] ) ) ;
		else if( !strcmp( "-a", argv[i] ) )
			para.warp_a = atof( argv[i+1] ) ;
		else if( !strcmp( "-b", argv[i] ) )
			para.warp_b = atof( argv[i+1] ) ;
		else if( !strcmp( "-p", argv[i] ) )
			para.warp_p = atof( argv[i+1] ) ;
		else if( !strcmp( "-t", argv[i] ) )
			para.type = atof( argv[i+1] ) ;
		else if( !strcmp( "-SL", argv[i] ) )
			check_count[0] = ParseLine( para.src_lines, argv[i+1] ) ;
		else if( !strcmp( "-DL", argv[i] ) )
			check_count[1] = ParseLine( para.dst_lines, argv[i+1] ) ;
	}
	if( check_count[0] == 0 || check_count[1] == 0 || check_count[0] != check_count[1] )
		return 0 ;
	return 1 ;
}

int ParseLine( Mat &lines, char *filename )
{
	char buf[128] ;
	int *idx, *to ;
	float *x, *y ;
	int n_point, n_line ;
	int tmp ;
	map<int,int> idx_map ;
	FILE *fp ;

	fp = fopen( filename, "r" ) ;
	if( fp == NULL )
		return 0 ;

	//get number of points
	while( fgets( buf, 128, fp ) != NULL )
	{
		if( isdigit( buf[0] ) == 0 )
			continue ;
		else
		{
			sscanf( buf, "%d", &n_point ) ;
			break ;
		}
	}
	x = new float[n_point] ;
	y = new float[n_point] ;
	to = new int[n_point] ;
	idx = new int[n_point] ;
	n_line = 0 ;
	for( int i = 0 ; i<n_point ; i++ )
	{
		while( fgets( buf, 128, fp ) != NULL && isdigit( buf[0] ) == 0 ) ;
		sscanf( buf, "%d %d %f %f %d %d %d", &tmp, &tmp, &y[i], &x[i], &idx[i], &tmp, &to[i] ) ;
		if( idx[i] != to[i] )
			n_line++ ;
		idx_map[ idx[i] ] = i ;

	}
	fclose(fp) ;

	lines = Mat( 2, n_line+1, CV_32FC2 ) ;
	for( int i=0, j=0 ; i<n_point ; i++ )
	{
		if( idx[i] != to[i] )
		{
			( lines.at<Vec2f>( 0, j ) ).val[0] = x[i] ;
			( lines.at<Vec2f>( 0, j ) ).val[1] = y[i] ;
			( lines.at<Vec2f>( 1, j ) ).val[0] = x[ idx_map[ to[i] ] ] ;
			( lines.at<Vec2f>( 1, j ) ).val[1] = y[ idx_map[ to[i] ] ] ;
			j++ ;
		}
	}
	( lines.at<Vec2f>( 0, n_line ) ).val[0] = PI*1e-3 ;
	( lines.at<Vec2f>( 0, n_line ) ).val[1] = 0.5-1e-4 ;
	( lines.at<Vec2f>( 1, n_line ) ).val[0] = PI*1e-3 ;
	( lines.at<Vec2f>( 1, n_line ) ).val[1] = 0.5+1e-4 ;
	delete x ;
	delete y ;
	delete idx ;
	delete to ;
	return n_line+1 ;
}
