
#ifndef _UTIL_H__INCLUDED__2005_01_13_
#define _UTIL_H__INCLUDED__2005_01_13_

#include <windows.h>

///////////////////////////////////////////////////////////////////////////////
//	Assert
///////////////////////////////////////////////////////////////////////////////

void	Assert(BOOL b, const char *file, const int line);

#undef ASSERT

#ifdef START_ASSERT
#	define ASSERT(b)	Assert(b, __FILE__, __LINE__)
#   define VERIFY(b)    Assert(b, __FILE__, __LINE__)
#else
#	define ASSERT(b)	((void)0)
#   define VERIFY(b)    ((void)(b))
#endif

///////////////////////////////////////////////////////////////////////////////
//	log
///////////////////////////////////////////////////////////////////////////////

void	log(const char * szFormat, ...);

#undef LOG

#ifdef START_LOG
#	define LOG	if(0);else log
#else
#	define LOG if(1);else log
#endif

///////////////////////////////////////////////////////////////////////////////
//	Random functions
///////////////////////////////////////////////////////////////////////////////

// 返回一个区间[min, max)中的随机数
int           random(int max, int min = 0);
unsigned int  random(unsigned int max, unsigned int min = 0);
double        random(double max, double min = 0.0);

// 返回一个随机布尔值
BOOL	RandomBoolean();

// 随机加权选择，选中者权值复零，未被选中者权值加一；成功返回选中
// 者下标，否则返回负数
unsigned int	RandomSelect(unsigned int weight[], unsigned int n);
unsigned int	RandomSelect(std::vector<unsigned int> &weight);


int gcd(int m, int n);

inline int Round(float f)
{
	return (int)round(f);
}

inline int Round(double d)
{
	return (int)round(d);
}

///////////////////////////////////////////////////////////////////////////////
//	Bitmap functions
///////////////////////////////////////////////////////////////////////////////

struct ZLoadBmpContext
{
	UINT uIDBmp;
	int nLeftSrc;
	int nTopSrc;
	int nWidthSrc;
	int nHeightSrc;
	int nWidthDst;
	int nHeightDst;
};

HBITMAP LoadBmpFromSrc(HDC, const ZLoadBmpContext *);
HBITMAP LoadBmpFromSrc(HDC hDC, UINT uIDBmp,
					   int nLeftSrc, int nTopSrc,
					   int nWidthSrc, int nHeightSrc,
					   int nWidthDst, int nHeightDst);

///////////////////////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////////////////////

#include "FPS.h"


///////////////////////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////////////////////

double equation_bipartition(double (*func)(double, void*),
							void* param,
							double min,
							double max,
							double precision=1E-6);


////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

#endif // _UTIL_H__INCLUDED__2005_01_13_
