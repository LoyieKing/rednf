#ifndef GAME_CTRADESPACE_H_
#define GAME_CTRADESPACE_H_

// df_game_r 还原 —— CTradeSpace（交易空间，G2-4 批次）唯一声明点。
// 原来 CTradeSpace.cpp 内定义；因 CGameManager.h（StaticPool<CTradeSpace,300>）
// 需完整类型，拆到本头供两方 include，避免重复定义。
// 布局（ORIG CTradeSpace::CTradeSpace 0x082a416c / SetIDX 0x082a41f4 推导）：
//   +0x00 Mutex（0x18）          +0x18 int m_index
//   +0x1c char m_bTrading        +0x1d char m_bLocked
//   +0x20 CUser* m_pTraders[2]   +0x28 int m_nState[2]
//   +0x30 TradeSlot m_slots[2][27]（每槽 0x48，每行 0x798）
//   +0xf60 int m_nMoney[2]        +0xf68 int m_fieldF68[2]（proceed_trade 生物计数）
//   （总尺寸 0xf70）

#include "CUser.h"       // CUser（CUser.h 已含 GameTypes.h/Inven_Item.h）
#include "Inven_Item.h"  // Inven_Item（0x3d 字节）
#include "GameTypes.h"   // Mutex（0x18；真实成员，C1 隐式构造同 ORIG）

// ---- TradeSlot（0x48；Inven_Item 位于 +0x09）----
// 自然对齐尝试（Main 批准逐成员断言兜底）：Inven_Item 是 pack(1) 类
// （align 1）故 m_item 仍落 +0x09；结构 align 4 → sizeof 0x46 进位 0x48，
// 尾部 2 字节为隐式 padding。ORIG 证据（remove_item@0x852bfcc）：
//   m_field00@0：+0xa52 `movl $0x0,(%eax)`（lea +0x30 基）
//   m_field04@4：+0xa79 `movl $0x0,(%eax)`（lea +0x34 基）
//   m_field08@8：+0xaa0 `movb $0x0,0x8(%eax)`
//   m_item@9：+0xb0f `add $0x9,%eax` 后 set_add_info/get_add_info 调用基
//   行距 0x48：+0xa44 `imul $0x798`（=42*0x48）+ slot*0x48 shl 链
//   拷贝 0x46（跳尾 padding）：+0x3c9 `rep movsl`17 dword + +0x3d1 尾 word
// pack(1) 曾使 4.4 按 align1 走逐残基拷贝慢路（+16 ins/处×4 处）。
// 若下列任一断言失败（align 假设破裂）⇒ 回退 pack(1) 并上报 Main 裁决。
struct TradeSlot
{
    int m_field00;   // +0x00
    int m_field04;   // +0x04
    char m_field08;  // +0x08
    Inven_Item m_item;  // +0x09（0x3d）

    // P216：ORIG CTradeSpace::C1 0x082a416c 对 m_slots[2][27] 隐式构造循环
    // **无 EH cleanup**（4.1 不生成；4.4 恒发 landing+_Unwind_Resume，N91）。
    // 给元素 ctor 加 throw() 让 4.4 判 call nothrow（mangled/布局不变）。
    TradeSlot() throw();
    void Reset();
};
static_assert(offsetof(TradeSlot, m_field00) == 0x00, "ts0");
static_assert(offsetof(TradeSlot, m_field04) == 0x04, "ts4");
static_assert(offsetof(TradeSlot, m_field08) == 0x08, "ts8");
static_assert(offsetof(TradeSlot, m_item) == 0x09, "ts9");
static_assert(sizeof(TradeSlot) == 0x48, "ts size");

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
    bool cancel_trade();  // 头声明 bool：ORIG __PRETTY_FUNCTION__
        // "bool CTradeSpace::cancel_trade()"（cancel_trade@0x852effa Ghidra C
        // cMyTrace 第 1 参）；尾 `return cancel_trade_for_china(0)` 原样穿透
        // （ORIG call 后无 movzbl），int 声明会诱发 bool→int 归一 +1 指令。
    void cancel_trade_by_dis(CUser* user);
    bool cancel_trade_for_china(CUser* user);  // 头声明 bool：ORIG
        // __PRETTY_FUNCTION__ 串 "bool CTradeSpace::cancel_trade_for_china(CUser*)"
        // （@0x852e544 +0x181）；mangled 不含返回类型，ABI 不变。
    bool change_trade_state(CUser* user, int state);  // SIGFIX: bool（ORIG __PRETTY_FUNCTION__ 串）
    bool checkCancelTrade();  // bool：ORIG PRETTY 串 'bool CTradeSpace::checkCancelTrade()' @0x8c98540
    bool checkTrade();  // bool：ORIG PRETTY 串 'bool CTradeSpace::checkTrade()' @0x8c98444
    int check_item_exist(int traderIdx, int itemId);
    bool check_trade_possibility();  // ORIG 0x852994a 体 xor$1+test al = bool 物化
    int get_empty_itemslot(int traderIdx);
    bool proceed_trade();  // 头声明 bool：ORIG __PRETTY_FUNCTION__ 串
                           // "bool CTradeSpace::proceed_trade()"
                           // （proceed_trade@0x853087a +0x1209 _ZZ…E19__PRETTY_FUNCTION__）；
                           // mangled 不含返回类型，ABI 不变（尾部 mov%ebx,%eax 直返 0/1）
    int regist_item(CUser* user, int type, int a, int b);
    int remove_item(CUser* user, int a, int b, int c, int d);
    void reset();
    int send_lose_item_for_china(CUser* user, Inven_Item& item);
    void set_traders(CUser* a, CUser* b);

    void SetIDX(int idx);
    void lock();
    void unlock();
    bool IsLocked();  // ORIG remove_item 0x0852bfe3 `call; test %al,%al; je`（无 setne）⇒ bool（P292）
    void _SetLock(bool flag);
    bool is_empty();  // ORIG 0x82343a0 movzbl 0x1c + xor$1 = bool 返回
    int get_index();

private:
    Mutex m_mutex;            // +0x00
    int m_index;              // +0x18
    bool m_bTrading;          // +0x1c
    bool m_bLocked;           // +0x1d
    CUser* m_pTraders[2];     // +0x20
    int m_nState[2];          // +0x28
    TradeSlot m_slotsData[2][27];  // +0x30（每行 27 槽 × 0x48 = 0x798；二维数组隐式构造循环同 ORIG 双层形态）
    int m_nMoney[2];          // +0xf60
    int m_fieldF68[2];        // +0xf68（ORIG proceed_trade 0x853087a 生物转移计数：
                              //   ChangeOwner 末参 = CreatureItemSize+m_fieldF68[trader]；
                              //   转移成功 m_fieldF68[1-trader] += 1。原 m_pad 占位拆出）
};

#endif  // GAME_CTRADESPACE_H_
