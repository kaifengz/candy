#pragma once

typedef enum
{
	IM_CHAR,
	IM_INT,
	IM_DOUBLE,
	IM_STRING,
}ZInputMode,ZIM;

//49个零操作数操作符
//77个单整数操作数操作符
// 1个单字符操作数操作符
// 1个单实数操作数操作符
// 2个字符串单操作数操作符
// 9个双整数操作数操作符
// 1个整数字符串双操作数操作符
//总共140个操作符
typedef enum
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
	//candy
	OP_FC23,			//void fc23()
	OP_ZKF,				//void zkf()
}ZOperator,ZOP;

typedef union
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
}ZSecond;

typedef struct
{
	ZOperator op;	//操作符
	ZSecond sd;		//操作数
}ZCode;
