// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// window headers
#include <windows.h>
#include <scrnsave.h>

// c headers
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

// OpenGL headers
#include <gl\gl.h>
#include <gl\glu.h>
// #include <gl\glaux.h>

// C++ stl
#pragma warning(disable:4786)

#include <algorithm>
#include <list>
//#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>

using std::list;
//using std::map;
using std::multiset;
using std::pair;
using std::queue;
using std::set;
using std::stack;
using std::string;
using std::swap;
using std::vector;

// my headers
#include "resource.h"
#include "util.h"

// for
#if _MSC_VER < 1400
#	define for if(0);else for
#endif

#define countof(x)	(sizeof(x)/sizeof((x)[0]))

// reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
