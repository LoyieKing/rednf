#ifndef GAME_CTRADESPACE_H_
#define GAME_CTRADESPACE_H_

// df_game_r 还原 —— CTradeSpace（交易空间，G2-4 批次）唯一声明点。
// 原来 CTradeSpace.cpp 内定义；因 CGameManager.h（StaticPool<CTradeSpace,300>）
// 需完整类型，拆到本头供两方 include，避免重复定义。
// 布局（ORIG CTradeSpace::CTradeSpace 0x082a416c / SetIDX 0x082a41f4 推导）：
//   +0x00 Mutex（0x18）          +0x18 int m_index
//   +0x1c char m_bTrading        +0x1d char m_bLocked
//   +0x20 CUser* m_pTraders[2]   +0x28 int m_nState[2]
//   +0x30 TradeSlot m_slots[2][26]（每槽 0x48，每行 0x798）
//   +0xf60 int m_nMoney[2]       （总尺寸 0xf70）

#include "CUser.h"       // CUser（CUser.h 已含 GameTypes.h/Inven_Item.h）
#include "Inven_Item.h"  // Inven_Item（0x3d 字节）

// ---- TradeSlot（0x48；Inven_Item 位于 +0x09）----
#pragma pack(push, 1)
struct TradeSlot
{
    int m_field00;   // +0x00
    int m_field04;   // +0x04
    char m_field08;  // +0x08
    Inven_Item m_item;  // +0x09（0x3d）
    char m_pad[0x48 - 0x09 - 0x3d];

    TradeSlot();
    void Reset();
};
#pragma pack(pop)

class CTradeSpace
{
public:
    CTradeSpace();
    ~CTradeSpace();

    CUser* GetOppositeUser(CUser* user);
    int _CheckTrade4SameAvatar(const TradeSlot* slots, const Inven_Item& item);
    int _IsTradable(const Inven_Item& item);
    void _SaveAvatarItemOwnerChange(unsigned int a, unsigned int b,
                                    unsigned int c, unsigned char d);
    int add_item(int traderIdx, TradeSlot slot, int count);
    int cancel_trade();
    void cancel_trade_by_dis(CUser* user);
    int cancel_trade_for_china(CUser* user);
    int change_trade_state(CUser* user, int state);
    int checkCancelTrade();
    int checkTrade();
    int check_item_exist(int traderIdx, int itemId);
    int check_trade_possibility();
    int get_empty_itemslot(int traderIdx);
    int proceed_trade();
    int regist_item(CUser* user, int type, int a, int b);
    int remove_item(CUser* user, int a, int b, int c, int d);
    void reset();
    int send_lose_item_for_china(CUser* user, Inven_Item& item);
    void set_traders(CUser* a, CUser* b);

    void SetIDX(int idx);
    void lock();
    void unlock();
    char IsLocked();
    void _SetLock(bool flag);
    char is_empty();
    int get_index();

private:
    char m_mutex[0x18];       // +0x00
    int m_index;              // +0x18
    char m_bTrading;          // +0x1c
    char m_bLocked;           // +0x1d
    CUser* m_pTraders[2];     // +0x20
    int m_nState[2];          // +0x28
    char m_slotsData[2 * 0x798];  // +0x30（行距 0x798 = 27 * 0x48；手写构造）
    int m_nMoney[2];          // +0xf60
    char m_pad[0xf70 - 0xf68];
};

#endif  // GAME_CTRADESPACE_H_