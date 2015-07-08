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
	E_FILEEND,		//Դ���������
	E_NULL,			//�գ�����ʶ�ķ���
	E_IDENT,		//��ʶ��
	E_CHARACTER,	//�ַ�
	E_INUMBER,		//����
	E_DNUMBER,		//ʵ��
	E_STRING,		//�ַ���
////�����
	E_PLUS,			//��
	E_MINUS,		//��
	E_TIMES,		//��
	E_SLASH,		//��
	E_MOD,			//��
	E_PLUSBECOMES,	//�ӵ�
	E_MINUSBECOMES,	//����
	E_TIMESBECOMES,	//�˵�
	E_SLASHBECOMES,	//����
	E_MODBECOMES,	//���
	E_EQUAL,		//����
	E_NOTEQUAL,		//������
	E_LESS,			//С��
	E_LESSEQUAL,	//С�ڵ���
	E_GREAT,		//����
	E_GREATEQUAL,	//���ڵ���
	E_BECOMES,		//��ֵ��
	E_NOT,			//��
	E_AND,			//��
	E_OR,			//��
////���
	E_LPAREN,		//��Բ����
	E_RPAREN,		//��Բ����
	E_LSUB,			//������
	E_RSUB,			//�ҷ�����
	E_BEGIN,		//������
	E_END,			//�һ�����
	E_COMMA,		//����
	E_COLON,		//ð��
	E_SEMICOLON,	//�ֺ�
////�ؼ���
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
////�⺯��
	////��ѧ��
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
	////�ַ���
	E_STRCMP,		//strcmp
	E_STRCPY,		//strcpy
	E_STRLEN,		//strlen
	E_STRLWR,		//strlwr
	E_STRUPR,		//strupr
	////����
	E_AVERAGE,		//average
	E_DOWNSORT,		//downsort
	E_MAX,			//min
	E_MIN,			//max
	E_SUM,			//sum
	E_UPSORT,		//upsort
};

enum ZErrorType
{
	ET_DEFINEDFUNCTION,	//�����Ѿ�����
	ET_DEFINEDIDENT,	//��ʶ���Ѿ�����
	ET_DIFPARAMNUM,		//����������Ŀ��Ԥ����ʱ����ͬ
	ET_DIFPARAMTYPE,	//��������������Ԥ����ʱ����ͬ
	ET_DIFRETURNTYPE,	//��������������Ԥ����ʱ����ͬ
	ET_DONOWHILE,		//do-whileѭ�����ȱ��while
	ET_DOUBLEMOD,		//�������μ���ģ����
	ET_DOUBLESUB,		//ʵ������ʵ�����ʽ����Ϊ�����±�
	ET_DOUBLESWITCH,	//����������ѡ�����
	ET_EMPTYSTATE,		//�����п����
	ET_FUNCTIONUS,		//����û��ʵ�ֵĺ���
	ET_INPUTNACARRAY,	//�޷�������ַ�����
	ET_LONGSTR2ARRAY,	//���ڳ�ʼ���ַ�������ַ���̫��
	ET_MISSBEGIN,		//ȱ�������š�{��
	ET_MISSCOLON,		//ȱ��ð�š�:��
	ET_MISSCOMMA,		//ȱ�ٶ��š�,��
	ET_MISSEND,			//ȱ�������š�}��
	ET_MISSIDENT,		//ȱ�ٱ�ʶ��
	ET_MISSLPAREN,		//ȱ�������š�(��
	ET_MISSLSUB,		//ȱ�������š�[��
	ET_MISSRPAREN,		//ȱ�������š�)��
	ET_MISSRSUB,		//ȱ�������š�]��
	ET_MISSSEMICOLON,	//ȱ�ٷֺš�;��
	ET_MUSTCHANGETYPE,	//�������ǿ������ת��
	ET_MUSTRETURNVALUE,	//�������ͷ�void���������з���ֵ
	ET_NOARRIDENTARR,	//�Ѳ�������ı�ʶ����Ϊ�������
	ET_NOMAIN,			//�Ҳ���main����
	ET_NOVOIDMAIN,		//main�����������Ͳ���void
	ET_PARAMMAIN,		//main�������в���
	ET_OUTPUTNCARRAY,	//��������鲻���ַ�����
	ET_SOURCENOEND,		//Դ�����쳣����
	ET_STRINGNOEND,		//�ַ����쳣����
	ET_UNKNOWNARRAYSIZE,//δ֪�����鳤��
	ET_UNKNOWNERROR,	//δ֪���﷨�������
	ET_UNKNOWNFUNCDEF,	//δ֪�ĺ���������ʽ
	ET_UNKNOWNIDENT,	//δ����ı�ʶ��
	ET_UNKNOWNOP,		//δ֪�Ĳ���
	ET_UNKNOWNSTATE,	//δ֪�������ʽ
	ET_UNKNOWNSYSCHAR,	//δ֪��ת���ַ�
	ET_UNKNOWNTOKEN,	//�Ƿ�����
	ET_VOIDIDENT,		//��ʶ������Ϊvoid
	ET_VOIDINEXPRESSION,//���ʽ�е����˷�������Ϊvoid�ĺ���
	ET_VOIDRETURNVALUE,	//��������Ϊvoid�ĺ��������з���ֵ
	ET_WRONGBREAK,		//�����break���
	ET_WRONGCHARACTER,	//������ַ���ʾ
	ET_WRONGCONTINUE,	//�����continue���
	ET_WRONGIDENTTYPE,	//����ı�������
	ET_WRONGINIT,		//����ı����������ʼ��
	ET_WRONGPARAMNUM,	//��������ʱ������Ŀ����ȷ
	ET_WRONGPARAMTYPE,	//��������ʱ�������Ͳ���ȷ
	ET_WRONGRETURNTYPE,	//����ĺ�����������
};

//49���������������
//77��������������������
// 1�����ַ�������������
// 1����ʵ��������������
// 2���ַ�����������������
// 9��˫����������������
// 1�������ַ���˫������������
//�ܹ�140��������
enum ZOperator
{
	OP_C2D,				//����ת�� char => double	��sd.iNumberΪ��ַƫ������
	OP_C2I,				//����ת�� char => int		��sd.iNumberΪ��ַƫ������
	OP_CALL,			//�������ã�sdΪ������ַ��
	OP_CHANGESIGNALC,	//�ı��ַ�����
	OP_CHANGESIGNALD,	//�ı�ʵ������
	OP_CHANGESIGNALI,	//�ı���������
	OP_COPYI,			//����ջ����ֵ��������
	OP_D2C,				//����ת�� double => char	��sd.iNumberΪ��ַƫ������
	OP_D2I,				//����ת�� double => int	��sd.iNumberΪ��ַƫ������
	OP_DELARRAYC,		//ɾ��һ���ַ����飨sdΪ�����ַ��
	OP_DELARRAYD,		//ɾ��һ��ʵ�����飨sdΪ�����ַ��
	OP_DELARRAYI,		//ɾ��һ���������飨sdΪ�����ַ��
	OP_DELETEC,			//ɾ��ջ���ַ�
	OP_DELETED,			//ɾ��ջ��ʵ��
	OP_DELETEI,			//ɾ��ջ������
	OP_EQUALC,			//���ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_EQUALD,			//���ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_EQUALI,			//���ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_EXIT,			//�������
	OP_FUNCTIONSTART,	//������ʼ��־��sd.iNumberΪ�����������������ڴ棩
	OP_GREATC,			//���ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_GREATD,			//���ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_GREATEQUALC,		//���ڵ��ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_GREATEQUALD,		//���ڵ��ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_GREATEQUALI,		//���ڵ��ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_GREATI,			//���ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_I2C,				//����ת�� int => char	��sd.iNumberΪ��ַƫ������
	OP_I2D,				//����ת�� int => double��sd.iNumberΪ��ַƫ������
	OP_INPUTARRAYC,		//�����ַ�����ֵ��sd.iNumberΪ�����ַ���±���ջ����
	OP_INPUTARRAYD,		//����ʵ������ֵ��sd.iNumberΪ�����ַ���±���ջ����
	OP_INPUTARRAYI,		//������������ֵ��sd.iNumberΪ�����ַ���±���ջ����
	OP_INPUTC,			//�����ַ���sd.iNumberΪ��ַ��
	OP_INPUTCA,			//�����ַ����飨sd.iNumberΪ�����ַ��
	OP_INPUTD,			//����ʵ����sd.iNumberΪ��ַ��
	OP_INPUTI,			//����������sd.iNumberΪ��ַ��
	OP_JUMP,			//��������ת��sdΪ��ת��ַ��
	OP_LESSC,			//С�ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_LESSD,			//С�ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_LESSEQUALC,		//С�ڵ��ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_LESSEQUALD,		//С�ڵ��ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_LESSEQUALI,		//С�ڵ��ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_LESSI,			//С�ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_LOADARRAY,		//���������ַ��sd.iNumberΪ����ͷ��ַ��
	OP_LOADARRAYC,		//�����ַ�����ֵ��sd.iNumberΪ����ͷ��ַ���±���ջ��
	OP_LOADARRAYD,		//����ʵ������ֵ��sd.iNumberΪ����ͷ��ַ���±���ջ����
	OP_LOADARRAYI,		//������������ֵ��sd.iNumberΪ����ͷ��ַ���±���ջ����
	OP_LOADCONSTC,		//�����ַ�����	��sdΪ������
	OP_LOADCONSTD,		//����ʵ����	��sdΪ������
	OP_LOADCONSTI,		//����������	��sdΪ������
	OP_LOADVARC,		//�����ַ���sd.iNumberΪ��ַ��
	OP_LOADVARD,		//����ʵ����sd.iNumberΪ��ַ��
	OP_LOADVARI,		//����������sd.iNumberΪ��ַ��
	OP_MINUSC,			//�����ַ���
	OP_MINUSD,			//����ʵ����
	OP_MINUSI,			//����������
	OP_MODC,			//�ࣨ�ַ���
	OP_MODI,			//�ࣨ������
	OP_NEWARRAYC,		//����һ���ַ����飨sd.iNumberΪ����ͷ��ַ��sd.iNumber2Ϊ���ȣ�
	OP_NEWARRAYD,		//����һ��ʵ�����飨sd.iNumberΪ����ͷ��ַ��sd.iNumber2Ϊ���ȣ�
	OP_NEWARRAYI,		//����һ���������飨sd.iNumberΪ����ͷ��ַ��sd.iNumber2Ϊ���ȣ�
	OP_NOTEQUALC,		//�����ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_NOTEQUALD,		//�����ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_NOTEQUALI,		//�����ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
	OP_NULL,			//�ղ���
	OP_OUTPUTC,			//����ַ�
	OP_OUTPUTCA,		//����ַ����飨sdΪ�ַ������ַ��
	OP_OUTPUTD,			//���ʵ��
	OP_OUTPUTI,			//�������
	OP_OUTPUTS,			//����ַ�����sdΪ�ַ���ʵ��ַ��
	OP_PARAMTRANSA,		//����������ݣ�sd.iNumberΪ��ַ��sd.iNumber2Ϊƫ������
	OP_PARAMTRANSC,		//�ַ��������ݣ�sd.iNumberΪ��ַ��sd.iNumber2Ϊƫ������
	OP_PARAMTRANSD,		//ʵ���������ݣ�sd.iNumberΪ��ַ��sd.iNumber2Ϊƫ������
	OP_PARAMTRANSI,		//�����������ݣ�sd.iNumberΪ��ַ��sd.iNumber2Ϊƫ������
	OP_PLUSC,			//�ӣ��ַ���
	OP_PLUSD,			//�ӣ�ʵ����
	OP_PLUSI,			//�ӣ�������
	OP_RETURN,			//void	�������أ�sd.iNumberΪƫ������
	OP_RETURNC,			//char	�������أ�sd.iNumberΪƫ������
	OP_RETURND,			//int	�������أ�sd.iNumberΪƫ������
	OP_RETURNI,			//double�������أ�sd.iNumberΪƫ������
	OP_SAVEARRAYC,		//�洢�ַ�����ֵ��sd.iNumberΪ�����ַ����ֵ��ջ�����±���ջ�ζ���
	OP_SAVEARRAYD,		//�洢ʵ������ֵ��sd.iNumberΪ�����ַ����ֵ��ջ�����±���ջ�ζ���
	OP_SAVEARRAYI,		//�洢��������ֵ��sd.iNumberΪ�����ַ����ֵ��ջ�����±���ջ�ζ���
	OP_SAVEC,			//�����ַ�������sdΪ������ַ��
	OP_SAVED,			//����ʵ��������sdΪ������ַ��
	OP_SAVEI,			//��������������sdΪ������ַ��
	OP_SLASHC,			//�����ַ���
	OP_SLASHD,			//����ʵ����
	OP_SLASHI,			//����������
	OP_TIMESC,			//�ˣ��ַ���
	OP_TIMESD,			//�ˣ�ʵ����
	OP_TIMESI,			//�ˣ�������
//////�⺯�����ò�����
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
	IT_FUNCTION,	//����
	IT_FUNCTIONUS,	//��δʵ�ֵĺ���
};

struct ZIdent
{
	CString name;		//������
	ZIdentType type;	//��������
	int level;			//����������Ƕ�ײ��
	int nAddress;		//�����Ĵ洢��ַ
};

struct ZFunction
{
	int CurLevel;			//��ǰǶ�ײ��
	ZIdent **pIdent;		//ָ����������ָ��
	int nIdent;				//������ǰ��Ŀ
	int nMaxIdent;			//���������������ɵ���������Ŀ
	ZIdent **pParam;		//ָ����������ָ��
	int nParam;				//������ǰ��Ŀ
	int nMaxParam;			//���������������ɵ���������Ŀ
	int nMaxTotalMemory;	//������������ռ�õ�����ڴ�
	int nCurAddress;		//��ǰ����������ʹ�õ��ڴ���
	int nStartAddress;		//������ʼ��ַ
	BOOL bSolved;			//�Ƿ��Ѿ�����ʵ��
	CString name;			//������
	ZIdentType ReturnType;	//������������
};

union ZSecond
{
	struct
	{
		union
		{
			char cNumber;	//���ַ�������
			char *string;	//���ַ���ָ�������
			int iNumber;	//������������ �� ˫�������ĵ�һ������
		};
		union
		{
			int iNumber2;	//˫�����������ĵڶ�������
			char*string2;	//�������ַ���ָ��˫�������ĵڶ�������
		};
	};
	double dNumber;			//��ʵ��������
};

struct ZCode
{
	ZOperator op;	//������
	ZSecond sd;		//������
};
