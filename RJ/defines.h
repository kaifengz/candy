
#ifndef _INCLUDE_DEFINES_H_ZKF_2004_10_02_
#define _INCLUDE_DEFINES_H_ZKF_2004_10_02_

///////////////////////////////////////////////////////////////////////////////
//// ����
///////////////////////////////////////////////////////////////////////////////

#define WND_WIDTH					800
#define WND_HEIGHT					600

///////////////////////////////////////////////////////////////////////////////
//// ֽ��
///////////////////////////////////////////////////////////////////////////////

#define CARD_EACH_INIT				13				// ÿ������Ƶĳ�ʼ��Ŀ
#define CARD_EACH_MAX				13				// ÿ������ƿ���������Ŀ
#define PLAYER_NUM					4				// ��Ҹ���

#define INTERVAL_H					15				// ��ˮƽ���
#define INTERVAL_V					17				// �ƴ�ֱ���
#define INTERVAL_SUIT_H				25				// ��ɫ��֮���ˮƽ���
#define INTERVAL_SUIT_V				28				// ��ɫ��֮��Ĵ�ֱ���

#define CARD_WIDTH					71				// �ƵĿ��
#define CARD_HEIGHT					96				// �Ƶĸ߶�

#define CARD_POPUP					INTERVAL_V		// �Ƶ���ʱ�ĵ����߶�

#define CENTER_CARD					6		// �Ѵ���Ƶ����ӣ�������ɫ���ߣ�
#define VALUE_A						0		// A����ֵ
#define VALUE_K						12		// K����ֵ
#define FIRST_OUT_CARD				45		// �������ȴ�����ƣ������ߣ�

#define SUIT(card)					(card/SUIT_EACH)
#define VALUE(card)					(card%SUIT_EACH)


// �ĸ�����Ƶİڷ�λ��
#define LEFT_EAST(client_width)		((client_width) - LEFT_WEST - CARD_WIDTH)
#define LEFT_WEST					20
#define TOP_NORTH					4
#define TOP_SOUTH(client_height)	((client_height) - TOP_NORTH - CARD_HEIGHT)

#define SUIT_NUM					4						// ��ɫ������
#define SUIT_EACH					13						// ÿ�ֻ�ɫ�Ƶ���Ŀ
#define CARD_NUM					(LIST_EACH*LIST_NUM)	// �Ƶ�����

#define LIST_NUM					SUIT_NUM				// �Ѵ���Ƶ�����
#define LIST_EACH					SUIT_EACH				// �Ѵ����ÿ�����Ŀ
#define LIST_INTERVAL_H				(CARD_WIDTH+INTERVAL_H)	// �Ѵ���Ƶ���֮���ˮƽ���
#define LIST_INTERVAL_V				INTERVAL_V				// �Ѵ���Ƶ���֮�����ֱ���
// �Ѵ���Ƶİڷ�λ��
#define LEFT_LIST(client_width)		((client_width-(LIST_NUM-1)*LIST_INTERVAL_H-CARD_WIDTH)/2)
#define TOP_LIST(client_height)		((client_height-(LIST_EACH-1)*LIST_INTERVAL_V-CARD_HEIGHT)/2)

///////////////////////////////////////////////////////////////////////////////
//// ��ʱ��
///////////////////////////////////////////////////////////////////////////////

#define IDT_DISPATCH				17333	// ���Ƽ�ʱ��id�������ر�ʱ��Ч��
#define TIMESLICE_DISPATCH			60		// ���Ƶ��ӳ�ʱ��

#define IDT_CLEAN_UP				17334	// �����Ƽ�ʱ��id�������ر�ʱ��Ч��
#define TIMESLICE_CLEAN_UP			200		// �����Ƶ��ӳ�ʱ��

#define IDT_THINKING				17335	// ˼����ʱ��id
#define TIMESLICE_LOW				1000	// ����
#define TIMESLICE_MID				600		// ����
#define TIMESLICE_HIGH				400		// ����

#define IDT_CARD_SENDING			17336	// ���Ƽ�ʱ��id�������ر�ʱ��Ч��
#define TIMESLICE_CARD_SENDING		10		// ���Ƶ��ӳ�ʱ��
#define CARD_SENDING_NUM			8		// ���ƵĶ���֡��

#define IDT_TOKEN_TRANSFER			17337	// �������Ƽ�ʱ��id�������ر�ʱ��Ч��
#define TIMESLICE_TOKEN_TRANSFER	10		// �������Ƶ���һ��λ�õ��ӳ�ʱ��
#define TOKEN_TRANSFER_NUM			8		// �������ƵĶ���֡��

#define IDT_CARD_SELECT				17338	// ѡ���Ƶļ�ʱ��id
#define TIMESLICE_CARD_SELECT		100		// ѡ���Ƶ��ӳ�ʱ��

#define IDT_CARD_TRANSFER			17339	// �����Ƶļ�ʱ��id�������ر�ʱ��Ч��
#define TIMESLICE_CARD_TRANSFER		10		// �����Ƶ��ӳ�ʱ��
#define CARD_TRANSFER_NUM			8		// �����ƵĵĶ���֡��

///////////////////////////////////////////////////////////////////////////////
//// ����
///////////////////////////////////////////////////////////////////////////////

#define TOKEN_WIDTH					50
#define TOKEN_HEIGHT				50

#define INTERVAL_TOKEN				20

///////////////////////////////////////////////////////////////////////////////
//// ����
///////////////////////////////////////////////////////////////////////////////

#define ID_EAST				0
#define ID_NORTH			1
#define ID_WEST				2
#define ID_SOUTH			3
#define ID_PLAYER			ID_SOUTH

// ���ĳ��Ҳ���û�д���ƣ�������������ƻ��࣬��ÿ���ƴ������µķ���
#define PUNISH_PER_CARD		13

#define for if(0);else for

#endif // _INCLUDE_DEFINES_H_ZKF_2004_10_02_
