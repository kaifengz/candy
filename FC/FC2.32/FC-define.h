#pragma once

enum ZElement;
enum ZErrorType;
enum ZIdentType;
enum ZOperator;
struct ZIdent;
struct ZFunction;
union ZSecond;
struct ZCode;

typedef ZOperator	ZOP;
typedef ZErrorType	ZET;
typedef ZIdentType	ZIT;

#define FUNCTION_NUM_INC	8
#define IDENT_NUM_INC		16
#define CODE_NUM_INC		1024

enum ZElement
{
	E_FILEEND,		//源代码结束符
	E_NULL,			//空，不认识的符号
	E_IDENT,		//标识符
	E_CHARACTER,	//字符
	E_INUMBER,		//整数
	E_DNUMBER,		//实数
	E_STRING,		//字符串
////运算符
	E_PLUS,			//加
	E_MINUS,		//减
	E_TIMES,		//乘
	E_SLASH,		//除
	E_MOD,			//余
	E_PLUSBECOMES,	//加等
	E_MINUSBECOMES,	//减等
	E_TIMESBECOMES,	//乘等
	E_SLASHBECOMES,	//除等
	E_MODBECOMES,	//余等
	E_EQUAL,		//等于
	E_NOTEQUAL,		//不等于
	E_LESS,			//小于
	E_LESSEQUAL,	//小于等于
	E_GREAT,		//大于
	E_GREATEQUAL,	//大于等于
	E_BECOMES,		//赋值号
	E_NOT,			//非
	E_AND,			//与
	E_OR,			//或
////界符
	E_LPAREN,		//左圆括号
	E_RPAREN,		//右圆括号
	E_LSUB,			//左方括号
	E_RSUB,			//右方括号
	E_BEGIN,		//左花括号
	E_END,			//右花括号
	E_COMMA,		//逗号
	E_COLON,		//冒号
	E_SEMICOLON,	//分号
////关键字
	E_BREAK,		//break
	E_CASE,			//case
	E_CHAR,			//char
	E_CONTINUE,		//continue
	E_DEFAULT,		//default
	E_DO,			//do
	E_DOUBLE,		//double
	E_ELSE,			//else
	E_EXIT,			//exit
	E_FOR,			//for
	E_IF,			//if
	E_INPUT,		//input
	E_INT,			//int
	E_OUTPUT,		//output
	E_RETURN,		//return
	E_SWITCH,		//switch
	E_VOID,			//void
	E_WHILE,		//while
////库函数
	////数学库
	E_ABS,			//abs
	E_ARCCOS,		//arccos
	E_ARCCTG,		//arcctg
	E_ARCSIN,		//arcsin
	E_ARCTG,		//arctg
	E_CH,			//ch
	E_COS,			//cos
	E_CTG,			//ctg
	E_CTH,			//cth
	E_EXP,			//exp
	E_LG,			//lg
	E_LN,			//ln
	E_POW,			//pow
	E_SH,			//sh
	E_SIN,			//sin
	E_SQRT,			//sqrt
	E_TG,			//tg
	E_TH,			//th
	////字符串
	E_STRCMP,		//strcmp
	E_STRCPY,		//strcpy
	E_STRLEN,		//strlen
	E_STRLWR,		//strlwr
	E_STRUPR,		//strupr
	////数组
	E_AVERAGE,		//average
	E_DOWNSORT,		//downsort
	E_MAX,			//min
	E_MIN,			//max
	E_SUM,			//sum
	E_UPSORT,		//upsort
};

enum ZErrorType
{
	ET_DEFINEDFUNCTION,	//函数已经定义
	ET_DEFINEDIDENT,	//标识符已经定义
	ET_DIFPARAMNUM,		//函数参数数目与预定义时不相同
	ET_DIFPARAMTYPE,	//函数参数类型与预定义时不相同
	ET_DIFRETURNTYPE,	//函数返回类型与预定义时不相同
	ET_DONOWHILE,		//do-while循环语句缺少while
	ET_DOUBLEMOD,		//浮点数参加求模运算
	ET_DOUBLESUB,		//实数（或实数表达式）作为数组下标
	ET_DOUBLESWITCH,	//浮点数用于选择语句
	ET_EMPTYSTATE,		//不能有空语句
	ET_FUNCTIONUS,		//存在没有实现的函数
	ET_INPUTNACARRAY,	//无法输入非字符数组
	ET_LONGSTR2ARRAY,	//用于初始化字符数组的字符串太长
	ET_MISSBEGIN,		//缺少左括号“{”
	ET_MISSCOLON,		//缺少冒号“:”
	ET_MISSCOMMA,		//缺少逗号“,”
	ET_MISSEND,			//缺少右括号“}”
	ET_MISSIDENT,		//缺少标识符
	ET_MISSLPAREN,		//缺少左括号“(”
	ET_MISSLSUB,		//缺少左括号“[”
	ET_MISSRPAREN,		//缺少右括号“)”
	ET_MISSRSUB,		//缺少右括号“]”
	ET_MISSSEMICOLON,	//缺少分号“;”
	ET_MUSTCHANGETYPE,	//必须进行强制类型转换
	ET_MUSTRETURNVALUE,	//返回类型非void函数必须有返回值
	ET_NOARRIDENTARR,	//把不是数组的标识符作为数组操作
	ET_NOMAIN,			//找不到main函数
	ET_NOVOIDMAIN,		//main函数返回类型不是void
	ET_PARAMMAIN,		//main函数带有参数
	ET_OUTPUTNCARRAY,	//输出的数组不是字符数组
	ET_SOURCENOEND,		//源代码异常结束
	ET_STRINGNOEND,		//字符串异常结束
	ET_UNKNOWNARRAYSIZE,//未知的数组长度
	ET_UNKNOWNERROR,	//未知的语法语义错误
	ET_UNKNOWNFUNCDEF,	//未知的函数定义形式
	ET_UNKNOWNIDENT,	//未定义的标识符
	ET_UNKNOWNOP,		//未知的操作
	ET_UNKNOWNSTATE,	//未知的语句形式
	ET_UNKNOWNSYSCHAR,	//未知的转义字符
	ET_UNKNOWNTOKEN,	//非法符号
	ET_VOIDIDENT,		//标识符类型为void
	ET_VOIDINEXPRESSION,//表达式中调用了返回类型为void的函数
	ET_VOIDRETURNVALUE,	//返回类型为void的函数不能有返回值
	ET_WRONGBREAK,		//错误的break语句
	ET_WRONGCHARACTER,	//错误的字符表示
	ET_WRONGCONTINUE,	//错误的continue语句
	ET_WRONGIDENTTYPE,	//错误的变量类型
	ET_WRONGINIT,		//错误的变量或数组初始化
	ET_WRONGPARAMNUM,	//函数调用时参数数目不正确
	ET_WRONGPARAMTYPE,	//函数调用时参数类型不正确
	ET_WRONGRETURNTYPE,	//错误的函数返回类型
};

//49个零操作数操作符
//77个单整数操作数操作符
// 1个单字符操作数操作符
// 1个单实数操作数操作符
// 2个字符串单操作数操作符
// 9个双整数操作数操作符
// 1个整数字符串双操作数操作符
//总共140个操作符
enum ZOperator
{
	OP_C2D,				//类型转换 char => double	（sd.iNumber为地址偏移量）
	OP_C2I,				//类型转换 char => int		（sd.iNumber为地址偏移量）
	OP_CALL,			//函数调用（sd为函数地址）
	OP_CHANGESIGNALC,	//改变字符符号
	OP_CHANGESIGNALD,	//改变实数符号
	OP_CHANGESIGNALI,	//改变整数符号
	OP_COPYI,			//复制栈顶数值（整数）
	OP_D2C,				//类型转换 double => char	（sd.iNumber为地址偏移量）
	OP_D2I,				//类型转换 double => int	（sd.iNumber为地址偏移量）
	OP_DELARRAYC,		//删除一个字符数组（sd为数组地址）
	OP_DELARRAYD,		//删除一个实数数组（sd为数组地址）
	OP_DELARRAYI,		//删除一个整数数组（sd为数组地址）
	OP_DELETEC,			//删除栈顶字符
	OP_DELETED,			//删除栈顶实数
	OP_DELETEI,			//删除栈顶整数
	OP_EQUALC,			//等于（sd.iNumber为条件满足时的跳转地址）
	OP_EQUALD,			//等于（sd.iNumber为条件满足时的跳转地址）
	OP_EQUALI,			//等于（sd.iNumber为条件满足时的跳转地址）
	OP_EXIT,			//程序结束
	OP_FUNCTIONSTART,	//函数开始标志（sd.iNumber为变量参数所需的最大内存）
	OP_GREATC,			//大于（sd.iNumber为条件满足时的跳转地址）
	OP_GREATD,			//大于（sd.iNumber为条件满足时的跳转地址）
	OP_GREATEQUALC,		//大于等于（sd.iNumber为条件满足时的跳转地址）
	OP_GREATEQUALD,		//大于等于（sd.iNumber为条件满足时的跳转地址）
	OP_GREATEQUALI,		//大于等于（sd.iNumber为条件满足时的跳转地址）
	OP_GREATI,			//大于（sd.iNumber为条件满足时的跳转地址）
	OP_I2C,				//类型转换 int => char	（sd.iNumber为地址偏移量）
	OP_I2D,				//类型转换 int => double（sd.iNumber为地址偏移量）
	OP_INPUTARRAYC,		//输入字符数组值（sd.iNumber为数组地址，下标在栈顶）
	OP_INPUTARRAYD,		//输入实型数组值（sd.iNumber为数组地址，下标在栈顶）
	OP_INPUTARRAYI,		//输入整型数组值（sd.iNumber为数组地址，下标在栈顶）
	OP_INPUTC,			//输入字符（sd.iNumber为地址）
	OP_INPUTCA,			//输入字符数组（sd.iNumber为数组地址）
	OP_INPUTD,			//输入实数（sd.iNumber为地址）
	OP_INPUTI,			//输入整数（sd.iNumber为地址）
	OP_JUMP,			//无条件跳转（sd为跳转地址）
	OP_LESSC,			//小于（sd.iNumber为条件满足时的跳转地址）
	OP_LESSD,			//小于（sd.iNumber为条件满足时的跳转地址）
	OP_LESSEQUALC,		//小于等于（sd.iNumber为条件满足时的跳转地址）
	OP_LESSEQUALD,		//小于等于（sd.iNumber为条件满足时的跳转地址）
	OP_LESSEQUALI,		//小于等于（sd.iNumber为条件满足时的跳转地址）
	OP_LESSI,			//小于（sd.iNumber为条件满足时的跳转地址）
	OP_LOADARRAY,		//导入数组地址（sd.iNumber为数组头地址）
	OP_LOADARRAYC,		//导入字符数组值（sd.iNumber为数组头地址，下标在栈顶
	OP_LOADARRAYD,		//导入实数数组值（sd.iNumber为数组头地址，下标在栈顶）
	OP_LOADARRAYI,		//导入整数数组值（sd.iNumber为数组头地址，下标在栈顶）
	OP_LOADCONSTC,		//导入字符常数	（sd为参数）
	OP_LOADCONSTD,		//导入实常数	（sd为参数）
	OP_LOADCONSTI,		//导入整常数	（sd为参数）
	OP_LOADVARC,		//导入字符（sd.iNumber为地址）
	OP_LOADVARD,		//导入实数（sd.iNumber为地址）
	OP_LOADVARI,		//导入整数（sd.iNumber为地址）
	OP_MINUSC,			//减（字符）
	OP_MINUSD,			//减（实数）
	OP_MINUSI,			//减（整数）
	OP_MODC,			//余（字符）
	OP_MODI,			//余（整数）
	OP_NEWARRAYC,		//申请一个字符数组（sd.iNumber为数组头地址，sd.iNumber2为长度）
	OP_NEWARRAYD,		//申请一个实数数组（sd.iNumber为数组头地址，sd.iNumber2为长度）
	OP_NEWARRAYI,		//申请一个整数数组（sd.iNumber为数组头地址，sd.iNumber2为长度）
	OP_NOTEQUALC,		//不等于（sd.iNumber为条件满足时的跳转地址）
	OP_NOTEQUALD,		//不等于（sd.iNumber为条件满足时的跳转地址）
	OP_NOTEQUALI,		//不等于（sd.iNumber为条件满足时的跳转地址）
	OP_NULL,			//空操作
	OP_OUTPUTC,			//输出字符
	OP_OUTPUTCA,		//输出字符数组（sd为字符数组地址）
	OP_OUTPUTD,			//输出实数
	OP_OUTPUTI,			//输出整数
	OP_OUTPUTS,			//输出字符串（sd为字符串实地址）
	OP_PARAMTRANSA,		//数组参数传递（sd.iNumber为地址，sd.iNumber2为偏移量）
	OP_PARAMTRANSC,		//字符参数传递（sd.iNumber为地址，sd.iNumber2为偏移量）
	OP_PARAMTRANSD,		//实数参数传递（sd.iNumber为地址，sd.iNumber2为偏移量）
	OP_PARAMTRANSI,		//整数参数传递（sd.iNumber为地址，sd.iNumber2为偏移量）
	OP_PLUSC,			//加（字符）
	OP_PLUSD,			//加（实数）
	OP_PLUSI,			//加（整数）
	OP_RETURN,			//void	函数返回（sd.iNumber为偏移量）
	OP_RETURNC,			//char	函数返回（sd.iNumber为偏移量）
	OP_RETURND,			//int	函数返回（sd.iNumber为偏移量）
	OP_RETURNI,			//double函数返回（sd.iNumber为偏移量）
	OP_SAVEARRAYC,		//存储字符数组值（sd.iNumber为数组地址，数值在栈顶，下标在栈次顶）
	OP_SAVEARRAYD,		//存储实数数组值（sd.iNumber为数组地址，数值在栈顶，下标在栈次顶）
	OP_SAVEARRAYI,		//存储整数数组值（sd.iNumber为数组地址，数值在栈顶，下标在栈次顶）
	OP_SAVEC,			//保存字符变量（sd为变量地址）
	OP_SAVED,			//保存实数变量（sd为变量地址）
	OP_SAVEI,			//保存整数变量（sd为变量地址）
	OP_SLASHC,			//除（字符）
	OP_SLASHD,			//除（实数）
	OP_SLASHI,			//除（整数）
	OP_TIMESC,			//乘（字符）
	OP_TIMESD,			//乘（实数）
	OP_TIMESI,			//乘（整数）
//////库函数调用操作符
	//math
	OP_ABSC,			//char abs(char)
	OP_ABSD,			//int abs(int)
	OP_ABSI,			//double abs(double)
	OP_ARCCOS,			//double arccos(double)
	OP_ARCCTG,			//double arcctg(double)
	OP_ARCSIN,			//double arcsin(double)
	OP_ARCTG,			//double arctg(double)
	OP_CH,				//double ch(double)
	OP_COS,				//double cos(double)
	OP_CTG,				//double ctg(double)
	OP_CTH,				//double cth(double)
	OP_EXP,				//double exp(double)
	OP_LG,				//double lg(double)
	OP_LN,				//double ln(double)
	OP_POW,				//double pow(double)
	OP_SH,				//double sh(double)
	OP_SIN,				//double sin(double)
	OP_SQRT,			//double sqrt(double)
	OP_TG,				//double tg(double)
	OP_TH,				//double th(double)
	//string
	OP_STRCMP,			//int strcmp([]char,[]char)
	OP_STRCPYCA,		//void strcpy([]char,[]char)
	OP_STRCPYS,			//void strcpy([]char,"")
	OP_STRLENCA,		//int strlen([]char)
	OP_STRLENS,			//int strlen("")
	OP_STRLWR,			//void strlwr([]char)
	OP_STRUPR,			//void strupr([]char)
	//array
	OP_AVERAGEC,		//double average(char,int)
	OP_AVERAGED,		//double average(double,int)
	OP_AVERAGEI,		//double arerage(int,int)
	OP_DOWNSORTC,		//void downsort([]char,int)
	OP_DOWNSORTD,		//void downsort([]double,int)
	OP_DOWNSORTI,		//void downsort([]int,int)
	OP_MAXC,			//char max([]char,int)
	OP_MAXD,			//double max([]double,int)
	OP_MAXI,			//int max([]int,int)
	OP_MINC,			//char min([]char,int)
	OP_MIND,			//double min([]double,int)
	OP_MINI,			//int min([]int,int)
	OP_SUMC,			//int sum([]char,int)
	OP_SUMD,			//double sum([]double,int)
	OP_SUMI,			//int sum([]int,int)
	OP_UPSORTC,			//void upsort([]char,int)
	OP_UPSORTD,			//void upsort([]double,int)
	OP_UPSORTI,			//void upsort([]int,int)
};

enum ZIdentType
{
	IT_NULL,		//
	IT_VOID,		//void
	IT_CHAR,		//char
	IT_INT,			//int
	IT_DOUBLE,		//double
	IT_CHARARRAY,	//char[]
	IT_INTARRAY,	//int[]
	IT_DOUBLEARRAY,	//double[]
	IT_ECHAR,		//
	IT_EINT,		//
	IT_EDOUBLE,		//
	IT_ECHARARRAY,	//
	IT_EINTARRAY,	//
	IT_EDOUBLEARRAY,//
	IT_FUNCTION,	//函数
	IT_FUNCTIONUS,	//尚未实现的函数
};

struct ZIdent
{
	CString name;		//变量名
	ZIdentType type;	//变量类型
	int level;			//变量所处的嵌套层次
	int nAddress;		//变量的存储地址
};

struct ZFunction
{
	int CurLevel;			//当前嵌套层次
	ZIdent **pIdent;		//指向变量数组的指针
	int nIdent;				//变量当前数目
	int nMaxIdent;			//变量数组所能容纳的最大变量数目
	ZIdent **pParam;		//指向参数数组的指针
	int nParam;				//参数当前数目
	int nMaxParam;			//参数数组所能容纳的最大参数数目
	int nMaxTotalMemory;	//参数、变量所占用的最大内存
	int nCurAddress;		//当前参数、变量使用的内存量
	int nStartAddress;		//函数起始地址
	BOOL bSolved;			//是否已经具体实现
	CString name;			//函数名
	ZIdentType ReturnType;	//函数返回类型
};

union ZSecond
{
	struct
	{
		union
		{
			char cNumber;	//单字符操作数
			char *string;	//单字符串指针操作数
			int iNumber;	//单整数操作数 或 双操作数的第一操作数
		};
		union
		{
			int iNumber2;	//双整数操作数的第二操作数
			char*string2;	//整数、字符串指针双操作数的第二操作数
		};
	};
	double dNumber;			//单实数操作数
};

struct ZCode
{
	ZOperator op;	//操作符
	ZSecond sd;		//操作数
};
