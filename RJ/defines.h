
#ifndef _INCLUDE_DEFINES_H_ZKF_2004_10_02_
#define _INCLUDE_DEFINES_H_ZKF_2004_10_02_

///////////////////////////////////////////////////////////////////////////////
//// 窗口
///////////////////////////////////////////////////////////////////////////////

#define WND_WIDTH					800
#define WND_HEIGHT					600

///////////////////////////////////////////////////////////////////////////////
//// 纸牌
///////////////////////////////////////////////////////////////////////////////

#define CARD_EACH_INIT				13				// 每个玩家牌的初始数目
#define CARD_EACH_MAX				13				// 每个玩家牌可能最大的数目
#define PLAYER_NUM					4				// 玩家个数

#define INTERVAL_H					15				// 牌水平间隔
#define INTERVAL_V					17				// 牌垂直间隔
#define INTERVAL_SUIT_H				25				// 各色牌之间的水平间隔
#define INTERVAL_SUIT_V				28				// 各色牌之间的垂直间隔

#define CARD_WIDTH					71				// 牌的宽度
#define CARD_HEIGHT					96				// 牌的高度

#define CARD_POPUP					INTERVAL_V		// 牌弹出时的弹出高度

#define CENTER_CARD					6		// 已打出牌的种子（即各花色的七）
#define VALUE_A						0		// A的牌值
#define VALUE_K						12		// K的牌值
#define FIRST_OUT_CARD				45		// 必须最先打出的牌（黑桃七）

#define SUIT(card)					(card/SUIT_EACH)
#define VALUE(card)					(card%SUIT_EACH)


// 四个玩家牌的摆放位置
#define LEFT_EAST(client_width)		((client_width) - LEFT_WEST - CARD_WIDTH)
#define LEFT_WEST					20
#define TOP_NORTH					4
#define TOP_SOUTH(client_height)	((client_height) - TOP_NORTH - CARD_HEIGHT)

#define SUIT_NUM					4						// 花色的种数
#define SUIT_EACH					13						// 每种花色牌的数目
#define CARD_NUM					(LIST_EACH*LIST_NUM)	// 牌的总数

#define LIST_NUM					SUIT_NUM				// 已打出牌的组数
#define LIST_EACH					SUIT_EACH				// 已打出牌每组的数目
#define LIST_INTERVAL_H				(CARD_WIDTH+INTERVAL_H)	// 已打出牌的牌之间的水平间隔
#define LIST_INTERVAL_V				INTERVAL_V				// 已打出牌的牌之间的竖直间隔
// 已打出牌的摆放位置
#define LEFT_LIST(client_width)		((client_width-(LIST_NUM-1)*LIST_INTERVAL_H-CARD_WIDTH)/2)
#define TOP_LIST(client_height)		((client_height-(LIST_EACH-1)*LIST_INTERVAL_V-CARD_HEIGHT)/2)

///////////////////////////////////////////////////////////////////////////////
//// 计时器
///////////////////////////////////////////////////////////////////////////////

#define IDT_DISPATCH				17333	// 发牌计时器id（动画关闭时无效）
#define TIMESLICE_DISPATCH			60		// 发牌的延迟时间

#define IDT_CLEAN_UP				17334	// 整理牌计时器id（动画关闭时无效）
#define TIMESLICE_CLEAN_UP			200		// 整理牌的延迟时间

#define IDT_THINKING				17335	// 思考计时器id
#define TIMESLICE_LOW				1000	// 低速
#define TIMESLICE_MID				600		// 中速
#define TIMESLICE_HIGH				400		// 高速

#define IDT_CARD_SENDING			17336	// 出牌计时器id（动画关闭时无效）
#define TIMESLICE_CARD_SENDING		10		// 出牌的延迟时间
#define CARD_SENDING_NUM			8		// 出牌的动画帧数

#define IDT_TOKEN_TRANSFER			17337	// 传送令牌计时器id（动画关闭时无效）
#define TIMESLICE_TOKEN_TRANSFER	10		// 传送令牌到下一个位置的延迟时间
#define TOKEN_TRANSFER_NUM			8		// 传送令牌的动画帧数

#define IDT_CARD_SELECT				17338	// 选择牌的计时器id
#define TIMESLICE_CARD_SELECT		100		// 选择牌的延迟时间

#define IDT_CARD_TRANSFER			17339	// 传送牌的计时器id（动画关闭时无效）
#define TIMESLICE_CARD_TRANSFER		10		// 传送牌的延迟时间
#define CARD_TRANSFER_NUM			8		// 传送牌的的动画帧数

///////////////////////////////////////////////////////////////////////////////
//// 令牌
///////////////////////////////////////////////////////////////////////////////

#define TOKEN_WIDTH					50
#define TOKEN_HEIGHT				50

#define INTERVAL_TOKEN				20

///////////////////////////////////////////////////////////////////////////////
//// 其它
///////////////////////////////////////////////////////////////////////////////

#define ID_EAST				0
#define ID_NORTH			1
#define ID_WEST				2
#define ID_SOUTH			3
#define ID_PLAYER			ID_SOUTH

// 如果某玩家不仅没有打出牌，反而比最初的牌还多，则每张牌处以以下的罚分
#define PUNISH_PER_CARD		13

#endif // _INCLUDE_DEFINES_H_ZKF_2004_10_02_
