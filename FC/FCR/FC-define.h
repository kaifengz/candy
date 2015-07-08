#pragma once

typedef enum
{
	IM_CHAR,
	IM_INT,
	IM_DOUBLE,
	IM_STRING,
}ZInputMode,ZIM;

//49���������������
//77��������������������
// 1�����ַ�������������
// 1����ʵ��������������
// 2���ַ�����������������
// 9��˫����������������
// 1�������ַ���˫������������
//�ܹ�140��������
typedef enum
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
}ZSecond;

typedef struct
{
	ZOperator op;	//������
	ZSecond sd;		//������
}ZCode;
