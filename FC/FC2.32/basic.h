#pragma once

#include "FC-define.h"

CString GetFileName(CString strFileTitle);
CString DesysString(CString string);
int SizeOf(ZIT it);
CString TransStringNewline(CString string);
CString GetIniFilePath();
CString GetTempFCRFilePath();
char *GetLabel(int nLine, char label[8]);

// fc library functions
int DownCompareChar(const void *ch1,const void *ch2);
int DownCompareInt(const void *i1,const void *i2);
int DownCompareDouble(const void *d1,const void *d2);
int UpCompareChar(const void *ch1,const void *ch2);
int UpCompareInt(const void *i1,const void *i2);
int UpCompareDouble(const void *d1,const void *d2);
