
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

// ����һ������[min, max)�е������
int           random(int max, int min = 0);
unsigned int  random(unsigned int max, unsigned int min = 0);
double        random(double max, double min = 0.0);

// ����һ���������ֵ
BOOL	RandomBoolean();

// �����Ȩѡ��ѡ����Ȩֵ���㣬δ��ѡ����Ȩֵ��һ���ɹ�����ѡ��
// ���±꣬���򷵻ظ���
unsigned int	RandomSelect(unsigned int weight[], unsigned int n);
unsigned int	RandomSelect(std::vector<unsigned int> &weight);


int gcd(int m, int n);

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
