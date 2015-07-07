#include "stdafx.h"
#include "FC.h"

#include "FCDoc.h"
#include "FCView.h"

#include "basic.h"
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char options[25][16]=
{
	"addprecision",
	"addspace",
	"addtime",
	"array",
	"auto",
	"binary",
	"comment",
	"empty",
	"emptyline",
	"file",
	"fileoutput",
	"indent",
	"math",
	"memory",
	"never",
	"optimize",
	"precision",
	"report",
	"show",
	"spaceindent",
	"string",
	"text",
	"time",
	"wait",
	"warning",
};

enum ZOption
{
	OPT_ADDPRECISION,
	OPT_ADDSPACE,
	OPT_ADDTIME,
	OPT_ARRAY,
	OPT_AUTO,
	OPT_BINARY,
	OPT_COMMENT,
	OPT_EMPTY,
	OPT_EMPTYLINE,
	OPT_FILE,
	OPT_FILEOUTPUT,
	OPT_INDENT,
	OPT_MATH,
	OPT_MEMORY,
	OPT_NEVER,
	OPT_OPTIMIZE,
	OPT_PRECISION,
	OPT_REPORT,
	OPT_SHOW,
	OPT_SPACEINDENT,
	OPT_STRING,
	OPT_TEXT,
	OPT_TIME,
	OPT_WAIT,
	OPT_WARNING,

	OPT_MAX,
};

#define ASSIGN_IF_BOOL(d,s) if((s)==0) d=FALSE; else if((s)==1) d=TRUE; else;
#define ASSIGN_IF_NOT_BELOW(d,s,min) if((s)>=(min)) d=(s); else;
#define ASSIGN_IF_IN_RANGE(d,s,min,max) if((s)>=(min) && (s)<=(max)) d=(s); else;

ZOption GetOption(const char szOpt[]);

void CFCView::ReadOptions()
{
	std::ifstream fin(GetIniFilePath(), std::ios::in | std::ios::_Nocreate);
	if(!fin)
		return;

	char szOpt[1024];
	char string[1024];
	char cEqual;
	int i;
	ZOption opt;

	while(fin>>szOpt)
	{
		opt = GetOption(szOpt);
		if(opt >= OPT_MAX)
			continue;

		fin>>cEqual; // 输入等号
		if(opt == OPT_FILE)
		{
			fin>>string;
			m_strOutputFile = string;
			continue;
		}
		fin>>i;
		switch(opt)
		{
		case OPT_ADDPRECISION:	ASSIGN_IF_BOOL(m_bOutPrecision,i);			break;
		case OPT_ADDSPACE:		ASSIGN_IF_BOOL(m_bInsertSpace,i);			break;
		case OPT_ADDTIME:		ASSIGN_IF_BOOL(m_bRunTime,i);				break;
		case OPT_ARRAY:			ASSIGN_IF_BOOL(m_bIncludeArray,i);			break;
		case OPT_AUTO:			/* 不处理“自动进行强制类型转换” */		break;
		case OPT_BINARY:		ASSIGN_IF_BOOL(m_bGenerateBinary,i);		break;
		case OPT_COMMENT:		ASSIGN_IF_BOOL(m_bKeepComment,i);			break;
		case OPT_EMPTY:			ASSIGN_IF_BOOL(m_bAllowEmptyStatement,i);	break;
		case OPT_EMPTYLINE:		ASSIGN_IF_BOOL(m_bEmptyLine,i);				break;
		case OPT_FILEOUTPUT:	ASSIGN_IF_BOOL(m_bFileOutput,i);			break;
		case OPT_INDENT:		ASSIGN_IF_IN_RANGE(m_iBackNumber,i,0,8);	break;
		case OPT_MATH:			ASSIGN_IF_BOOL(m_bIncludeMath,i);			break;
		case OPT_MEMORY:		ASSIGN_IF_NOT_BELOW(m_nMemoryApply,i,1);	break;
		case OPT_NEVER:			/* 不处理“禁止隐式类型转换” */			break;
		case OPT_OPTIMIZE:		ASSIGN_IF_BOOL(m_bJumpOptimize,i);			break;
		case OPT_PRECISION:		ASSIGN_IF_IN_RANGE(m_nPrecision,i,0,15);	break;
		case OPT_REPORT:		ASSIGN_IF_BOOL(m_bIgnoreError,1-i);			break;
		case OPT_SHOW:			ASSIGN_IF_BOOL(m_bOutputInput,i);			break;
		case OPT_SPACEINDENT:	ASSIGN_IF_BOOL(m_iBackChoice,i);			break;
		case OPT_STRING:		ASSIGN_IF_BOOL(m_bIncludeString,i);			break;
		case OPT_TEXT:			ASSIGN_IF_BOOL(m_bGenerateText,i);			break;
		case OPT_TIME:			ASSIGN_IF_BOOL(m_bCalcRuntime,i);			break;
		case OPT_WAIT:			ASSIGN_IF_BOOL(m_bWaitEnd,i);				break;
		case OPT_WARNING:		ASSIGN_IF_BOOL(m_bIgnoreCircleNoend,1-i);	break;
		}
	}

	fin.close();
}

void CFCView::SaveOptions()
{
	std::ofstream fout(GetIniFilePath());

	//compile
	fout<<"[compile]\n";
	fout<<"empty\t\t= "<< (m_bAllowEmptyStatement?1:0) <<std::endl;
	fout<<"auto\t\t= "<< 0 <<std::endl;
	fout<<"never\t\t= "<< 0 <<std::endl;
	fout<<std::endl;

	//library
	fout<<"[library]\n";
	fout<<"math\t\t= "<< (m_bIncludeMath?1:0) <<std::endl;
	fout<<"string\t\t= "<< (m_bIncludeString?1:0) <<std::endl;
	fout<<"array\t\t= "<< (m_bIncludeArray?1:0) <<std::endl;
	fout<<std::endl;

	//intermidiate
	fout<<"[intermidiate]\n";
	fout<<"binary\t\t= "<< (m_bGenerateBinary?1:0) <<std::endl;
	fout<<"text\t\t= "<< (m_bGenerateText?1:0) <<std::endl;
	fout<<std::endl;

	//others
	fout<<"[others]\n";
	fout<<"report\t\t= "<< (m_bIgnoreError?0:1) <<std::endl;
	fout<<"optimize\t= "<< (m_bJumpOptimize?1:0) <<std::endl;
	fout<<std::endl;

	//interpret
	fout<<"[interpret]\n";
	fout<<"precision\t= "<< m_nPrecision <<std::endl;
	fout<<"time\t\t= "<< (m_bCalcRuntime?1:0) <<std::endl;
	fout<<"show\t\t= "<< (m_bOutputInput?1:0) <<std::endl;
	fout<<"fileoutput\t= "<< (m_bFileOutput?1:0) <<std::endl;
	if(m_bFileOutput)
		fout<<"file\t\t= "<< m_strOutputFile <<std::endl;
	fout<<"warning\t\t= "<< (m_bIgnoreCircleNoend?0:1) <<std::endl;
	fout<<"memory\t\t= "<< m_nMemoryApply <<std::endl;
	fout<<std::endl;

	//translate
	fout<<"[translate]\n";
	fout<<"addspace\t= "<< (m_bInsertSpace?1:0) <<std::endl;
	fout<<"emptyline\t= "<< (m_bEmptyLine?1:0) <<std::endl;
	fout<<"spaceindent\t= "<< m_iBackChoice <<std::endl;
	fout<<"indent\t\t= "<< m_iBackNumber <<std::endl;
	fout<<"comment\t\t= "<< (m_bKeepComment?1:0) <<std::endl;
	fout<<"addprecision\t= "<< (m_bOutPrecision?1:0) <<std::endl;
	fout<<"addtime\t\t= "<< (m_bRunTime?1:0) <<std::endl;
	fout<<"wait\t\t= "<< (m_bWaitEnd?1:0) <<std::endl;

	fout.close();
}

ZOption GetOption(const char szOpt[])
{
	int low=0,high=OPT_MAX-1,mid;
	int compare;
	while(low<=high)
	{
		mid=(low+high)/2;
		compare=strcmp(szOpt,options[mid]);
		if(compare==0)
		{
			return ZOption(mid);
		}
		if(compare<0)
			high=mid-1;
		else
			low=mid+1;
	}
	return OPT_MAX;

}
