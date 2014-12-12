
#include "stdafx.h"

void Assert(BOOL b, const char *file, const int line)
{
	if(b)
		;
	else
	{
		LOG("ASSERT FAIL:\n"
			 "FILE: %s\n"
			 "LINE: %d\n",
			 file, line);

		ExitProcess(-1);
	}
}

void log(const char *szFormat, ...)
{
#pragma warning(disable:4996)
// warning C4996: 'fopen': This function or variable may be unsafe. Consider using fopen_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
// D:\Program Files\Microsoft Visual Studio 8\VC\include\stdio.h(234) : see declaration of 'fopen'

	static FILE* g_fpLog = NULL;

	if(g_fpLog == NULL)
	{
		g_fpLog = fopen("mysaver.log", "a");
		if(g_fpLog == NULL)
			return;
		log("========================================");
		log("START LOG");
	}

#define MAX_BUFFER_SIZE			1024
#define MAX_LINE				128
	char header_prefix[MAX_BUFFER_SIZE];
	char space_prefix[MAX_BUFFER_SIZE];
	char *pLine[MAX_LINE];
	char line[MAX_BUFFER_SIZE];
	char new_format[4*MAX_BUFFER_SIZE];

	char format[MAX_BUFFER_SIZE];
	strcpy(format, szFormat);
	if(format[ strlen(format)-1 ] != '\n')
		strcat(format, "\n");

	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		sprintf(header_prefix, "%04d-%02d-%02d %02d:%02d:%02d.%03d     ",
			st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

		memset(space_prefix, ' ', strlen(header_prefix));
		space_prefix[ strlen(header_prefix) ] = '\0';
	}

	{
		memset(pLine, '\0', sizeof(pLine));
		char *p = format;
		for(int i=0; *p; i++)
		{
			pLine[i] = p;
			p = strchr(p, '\n');
			*p = '\0';
			p++;
		}
	}

	{
		new_format[0] = '\0';
		for(int i=0; pLine[i]; i++)
		{
			sprintf(line, "%s%s\n",
				(i==0 ? header_prefix : space_prefix),
				pLine[i]);
			strcat(new_format, line);
		}
	}

	va_list arg;
	va_start(arg, szFormat);
	vfprintf(g_fpLog, new_format, arg);
	va_end(arg);

	fflush(g_fpLog);

#pragma warning(default:4996)
}

///////////////////////////////////////////////////////////////////////////////
//	Random functions
///////////////////////////////////////////////////////////////////////////////

int random(int max, int min) // a random number of [min, max)
{
	if(min > max)
	{
		int t = min;
		min = max;
		max = t;
	}

	return rand() % (max-min) + min;
}

unsigned int random(unsigned int max, unsigned int min)
{
	if(min > max)
	{
		unsigned int t = min;
		min = max;
		max = t;
	}

	return rand() % (max-min) + min;
}

double random(double max, double min)
{
	if (min > max)
	{
		double t = min;
		min = max;
		max = t;
	}

	return rand() % 1000 / 1000.0 * (max - min) + min;
}

BOOL RandomBoolean()
{
	return (rand() % 1024 < 512);
}

unsigned int RandomSelect(unsigned int weight[], unsigned int n)
{
	ASSERT(n > 0);

	if(n == 1)
	{
		weight[0] = 0;
		return 0;
	}

	unsigned int sum_weight = 0;
	unsigned int i;
	unsigned int iSel;

	for(i=0; i<n; i++)
	{
		sum_weight += weight[i];
	}

	if(sum_weight == 0)
	{
		iSel = random(n);
	}
	else
	{
		int t = random(sum_weight);

		iSel = -1;
		for(i=0; i<n; i++)
		{
			t -= weight[i];
			if(t < 0)
			{
				iSel = i;
				break;
			}
		}
		ASSERT(iSel >= 0);
	}

	for(i=0; i<n; i++)
		++weight[i];
	weight[iSel] = 0;

	return iSel;
}

unsigned int RandomSelect(std::vector<unsigned int> &weight)
{
	return RandomSelect((unsigned int*)(&weight[0]), (unsigned int)weight.size());
}


int gcd(int m, int n)
{
	if (m >= n)
		m %= n;

	while (m != 0)
	{
		int t = n % m;
		n = m;
		m = t;
	}

	return n;
}

///////////////////////////////////////////////////////////////////////////////
//	Bitmap functions
///////////////////////////////////////////////////////////////////////////////

HBITMAP LoadBmpFromSrc(HDC hDC, const ZLoadBmpContext *pLBC)
{
	return LoadBmpFromSrc(
		hDC,
		pLBC->uIDBmp,
		pLBC->nLeftSrc, pLBC->nTopSrc,
		pLBC->nWidthSrc, pLBC->nHeightSrc,
		pLBC->nWidthDst, pLBC->nHeightDst);
}

HBITMAP LoadBmpFromSrc(HDC hDC, UINT uIDBmp,
					   int nLeftSrc, int nTopSrc,
					   int nWidthSrc, int nHeightSrc,
					   int nWidthDst, int nHeightDst)
{
	HBITMAP hBmpDest = CreateCompatibleBitmap(hDC,
		nWidthDst, nHeightDst);

	// the second dc for destination bitmap
	HDC hDCDest = CreateCompatibleDC(hDC);
	HBITMAP hOldBmpDest = (HBITMAP)SelectObject(hDCDest, hBmpDest);

	// the third dc for source bitmap
	HBITMAP hBmpSrc = (HBITMAP)LoadImage(
		GetModuleHandle(NULL), MAKEINTRESOURCE(uIDBmp),
		IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE);
	HDC hDCSrc = CreateCompatibleDC(hDC);
	HBITMAP hOldBmpSrc = (HBITMAP)SelectObject(hDCSrc, hBmpSrc);

	// stretch copy
	StretchBlt(hDCDest, 0, 0, nWidthDst, nHeightDst,
		hDCSrc, nLeftSrc, nTopSrc, nWidthSrc, nHeightSrc,
		SRCCOPY);

	{	// free memmory
		SelectObject(hDCSrc, hOldBmpSrc);
		DeleteDC(hDCSrc);
		DeleteObject(hBmpSrc);

		SelectObject(hDCDest, hOldBmpDest);
		DeleteDC(hDCDest);
	}

	return hBmpDest;
}


///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

double equation_bipartition(double (*func)(double, void*),
							void* param,
							double min,
							double max,
							double precision)
{
	ASSERT(func != NULL);
	ASSERT(min < max);
	ASSERT(precision >= 0);

	const double y_min = func(min, param);
	const double y_max = func(max, param);
	ASSERT( (y_min <= 0 && y_max >= 0) ||
		(y_min >= 0 && y_max <= 0) );

	if (y_min == 0)
		return min;
	if (y_max == 0)
		return max;

	if (y_min < y_max)
	{
		while (1)
		{
			const double mid = (min + max) / 2.0;
			const double y_mid = func(mid, param);
			if (y_mid >= -precision && y_mid <= precision)
				return mid;
			if (y_mid > 0)
				max = mid;
			else // y_mid < 0
				min = mid;
		}
	}
	else // y_min > y_max
	{
		while (1)
		{
			const double mid = (min + max) / 2.0;
			const double y_mid = func(mid, param);
			if (y_mid >= -precision && y_mid <= precision)
				return mid;
			if (y_mid > 0)
				min = mid;
			else // y_mid < 0
				max = mid;
		}
	}
}
