#ifndef _MLG_GLMATH
#define _MLG_GLMATH

#ifdef  __cplusplus
extern "C" {
#endif

//系统缺少的数学函数，在这里集中声明一下
//////////////////////////////////////////////
//定义暂时没有实现的函数
//////////////////////////////////////////////
extern	double	sqrt( double x );
extern	double	fabs( double x );
extern	double	ceil( double x );
extern	double	floor( double x );
extern	double	strtod( const char *nptr, char **endptr  );
extern	double	sin( double x );
extern	double	cos( double x );
extern	void	qsort( void *base, size_t num, size_t width, int (*compare )(const void *elem1, const void *elem2 ) );
extern	void	*bsearch( const void *key, const void *base, size_t num, size_t width, int (*compare ) ( const void *elem1, const void *elem2 ) );
extern	double	pow( double x, double y );
extern	double	sqrt( double x );
extern	int		_finite( double x );
extern	double	log( double x );
extern	double	exp( double x );
extern	double	frexp( double x, int *expptr );

#ifdef  __cplusplus
}
#endif

#endif