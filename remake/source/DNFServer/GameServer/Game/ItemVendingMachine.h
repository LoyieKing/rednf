#ifndef DNF_GAME_ITEM_VENDING_MACHINE_H_
#define DNF_GAME_ITEM_VENDING_MACHINE_H_

// ---------------------------------------------------------------------------
// df_game_r —— ItemVendingMachine 权威声明（唯一）。
// 归并自 GlobalData.cpp / Dispatcher_BuyCeraShopItem_impl.cpp 的局部声明。
// ORIG 布局 / mangled 符号见 docs/class_func_reports/ItemVendingMachine/README.md。
//
// Buyer 入口 BuyItem(CUser*, const MSG_BUY_CERASHOP_ITEM&) 为 __thiscall 委托：
// 仅调 ProcessBuyRequestEx(this, user, msg) 后返回 1（ORIG 0x0854c4d0）。
// ProcessBuyRequestEx（0x081769f6，115KB 报告）是真实业务依赖，定义在 GlobalData.cpp。
// ---------------------------------------------------------------------------

#include "GameEnums.h"  // INVEN_TYPE（_CheckEnableInventoryCondition/_GetInvenSlotInfo 形参）
#include <map>
#include <vector>

#include "TaiwanCash.h"  // Taiwan::BillTransaction（AuctionNeedInfo 成员）

#include "CCeraShop.h"  // WongWork::CCeraShop 完整定义（ItemVendingMachine 持有其成员）

namespace ARAD
{
// ORIG PurcahseBonus ctor 0x81ac710：+0 byte=0、+4 int=0、+8 short=0（0xc 字节）。
struct PurcahseBonus
{
    PurcahseBonus() : m_flag(0), m_field4(0), m_field8(0) {}
    // ORIG _ZNK4ARAD13PurcahseBonuseqERKS0_ W @0x088b66ce：仅比较 +0x00 字节。
    bool operator==(const PurcahseBonus& other) const { return m_flag == other.m_flag; }
    unsigned char m_flag;    // +0x00（购买次数门槛）
    // +0x04（1=代币 5=里程券 其它=itemIdx）。[b28d-solo-Arad_ItemBonus] 改 unsigned：
    // ORIG _processBonusItem 0x081ac395 段 `mov 0x4(%eax),%eax; mov %eax,%ebx`
    // （unsigned 字段→int 形参的转换中转，P265/P211 档③；int 字段 4.4 恒折单段）。
    // sizeof/偏移/mangled 零影响；m_field4 消费点仅 Arad_ItemBonus.cpp（已全仓 grep）。
    unsigned int m_field4;
    short m_field8;          // +0x08（数量）
};

// ORIG CeraShopPurcahseCountBonus ctor/dtor 0x854c5b6/0x854c5ca：
// 仅 map<int, vector<PurcahseBonus>> 成员自动构造/析构。
class CeraShopPurcahseCountBonus
{
public:
    CeraShopPurcahseCountBonus() {}
    ~CeraShopPurcahseCountBonus() {}

    // ---- W6_ARAD_ItemBonus 批次实现（Game/Arad_ItemBonus.cpp）----
    // ORIG _ZN4ARAD26CeraShopPurcahseCountBonus10LoadScriptEv T @0x081abf14
    bool LoadScript();
    // ORIG _ZN4ARAD26CeraShopPurcahseCountBonus13GiveBonusItemEP5CUsermit T @0x081abf44
    void GiveBonusItem(CUser* user, unsigned long itemIdx, int count,
                       unsigned short threshold);
    // ORIG _ZN4ARAD26CeraShopPurcahseCountBonus14CheckBonusItemEi T @0x081abff0
    bool CheckBonusItem(int key);
private:
    // ORIG _ZN4ARAD26CeraShopPurcahseCountBonus14_findBonusItemERKSt6vectorINS_13PurcahseBonusESaIS2_EERS2_t
    // T @0x081ac04c —— 取 vector 中 m_flag <= threshold 的最后一个元素拷入 out。
    bool _findBonusItem(const std::vector<PurcahseBonus>& list,
                        PurcahseBonus& out, unsigned short threshold);
    // ORIG _ZN4ARAD26CeraShopPurcahseCountBonus17_processBonusItemEP5CUseriRKNS_13PurcahseBonusE
    // T @0x081ac13e —— bonus 发放主逻辑（代币/里程券/物品三通道）。
    void _processBonusItem(CUser* user, int goodsIdx,
                           const PurcahseBonus& bonus);

    std::map<int, std::vector<PurcahseBonus> > m_bonusMap;  // +0x00
};
}  // namespace ARAD

class CUser;           // 前向（BuyItem/ProcessBuyRequestEx 形参）；完整定义见 CUser.h
struct MSG_BUY_CERASHOP_ITEM;  // 完整定义见 SigTypes.h（形参为 const 引用）
class CItem;           // 前向（_CheckEnableItemCondition 输出 CItem const*&）
class CCeraShopGoods;  // 前向（_Check* helper 形参）；完整定义见 CItemList.h
struct stAvatarTypeSelect_t;  // 前向（_CheckEnableAvatarCondition 输出引用）；完整定义见 CEquipItem.h

class ItemVendingMachine
{
public:
    ItemVendingMachine();   // 0x0854c3e4
    ~ItemVendingMachine();  // 0x0854c42a
    // ORIG BuyItem 0x0854c4d0：仅委托 ProcessBuyRequestEx(this, user, msg) 后返回 1。
    // bool 化（DL18 Lane18）：ORIG 调用点 0x0818eac0 call;xor$1;test;je =
    // bool callee 直判形；int 声明生 sete 物化。mangled 无返回类型，ABI 零变。
    bool BuyItem(CUser* user, const MSG_BUY_CERASHOP_ITEM& msg);
    // ORIG ProcessBuyRequestEx 0x081769f6：BuyItem 的唯一真实依赖（115KB 报告）。
    // b28c-l23 SIGFIX：int→bool（ORIG __PRETTY_FUNCTION__ 串 @0x08b87080 =
    // "bool ItemVendingMachine::ProcessBuyRequestEx(CUser*, const
    // MSG_BUY_CERASHOP_ITEM&)"，P174；体内 return 值仅 0/1）。mangled 不变。
    bool ProcessBuyRequestEx(CUser* user, const MSG_BUY_CERASHOP_ITEM& msg);

    // ORIG BuyAuctionItem 0x08175c5c（_ZN18ItemVendingMachine14BuyAuctionItemEP5CUserRNS_
    // 15AuctionNeedInfoEibi）：拍卖币购买拍卖行物品，Taiwan 计费两段 purchase
    //（0x3c 主扣费 / 0x3e 手续费），成功返回 true。实现在 ItemVendingMachine_Auction.cpp。
    struct AuctionNeedInfo
    {
        int m_price;                        // +0x00 拍卖价格
        long long m_itemUid;                // +0x04 拍卖物品唯一号
        int m_fee;                          // +0x0c 手续费（BuyAuctionItem 计算写回）
        Taiwan::BillTransaction m_trans1;   // +0x10 主扣费事务（0xc）
        Taiwan::BillTransaction m_trans2;   // +0x1c 手续费事务（0xc）
    };                                      // sizeof = 0x28（ORIG put_binary 0x28）
    bool BuyAuctionItem(CUser* user, AuctionNeedInfo& info, int a3,
                        bool bPremiumFee, int refValue);

    // ---- 私有校验/串化 helper（ORIG 成员；ProcessBuyRequestEx 依赖；实现在 GlobalData.cpp）----
    bool _CheckEnableGiftCondition(const CCeraShopGoods* goods, CUser* user, char c);   // 0x08176722
    bool _CheckEnableItemCondition(const CCeraShopGoods* goods, const CItem*& item,
                                   std::vector<std::pair<int, int> >& pairs);            // 0x0817697c
    bool _CheckEnableStateCondition(const CCeraShopGoods* goods, CUser* user);           // 0x081768b0
    bool _CheckEnableAvatarCondition(const CCeraShopGoods* goods, char c,
                                     unsigned char uc, stAvatarTypeSelect_t& out);       // 0x08176794
    // ---- W8_LongTail Batch07_VendingHero 批次（Game/W8LT_VendingHero.cpp）----
    // ORIG 0x08179eae：委托 m_bonus.LoadScript()。
    bool LoadPurcahseBonusScript();
    // ORIG 0x08179ed8：委托 m_bonus.CheckBonusItem((int)itemIdx)。
    bool CheckPurcahseBonusItem(unsigned long itemIdx);
    // ORIG 0x08179f08：委托 m_bonus.GiveBonusItem(user, itemIdx, count, threshold)。
    void ProcessPurcahseBonusItem(CUser* user, unsigned long itemIdx, int count,
                                  unsigned short threshold);
    // ORIG 0x0854c470：(int)(CItemList::GetVendingBonusRate() *
    // CCeraShopGoods::GetBonusWeight())。
    int GetFinalProbability(int goodsIdx);
    // ORIG 0x081768ba：库存余量 + 白名单 itemIdx(200/0xcb/0xcc/0xcd/5/6/7) 判定。
    bool _CheckEnableInventoryCondition(CUser* user, const CItem* item,
                                        INVEN_TYPE invenType,
                                        Inven_Item::ITEM_TYPE itemType);
    // ORIG 0x08176348：static Inven_Item temp; reset 后经 GetRelatedItem 的
    // vtable+0x08 拷贝；返回 temp.m_field1（ITEM_TYPE）。ORIG 返回 int
    //（调用点 4 字节直存；尾部 movzbl %al 扩展）。
    // b28c-L19：返回类型改 Inven_Item::ITEM_TYPE（ORIG _GetInvenSlotInfo 0x081763e5
    // store 为 LHS-first `mov 0x14(%ebp),%edx; mov %eax,(%edx)` ⇒ 与赋值目标同型，
    // P211；int 形使 4.4 异型化 RHS-first）。mangled 不含返回类型，符号不变。
    Inven_Item::ITEM_TYPE _GetItemType(const CCeraShopGoods* goods);
    // ORIG 0x081763be：按 groupIdx 映射 (INVEN_TYPE, ITEM_TYPE) 输出对。
    bool _GetInvenSlotInfo(const CCeraShopGoods* goods, INVEN_TYPE& invenType,
                           Inven_Item::ITEM_TYPE& itemType);

    // ---- IPG 计费应答族（形参 = user / SIPGData* req / SIPGData* res / Stream*）----
    void ProcessIPGUserData(CUser* user, WongWork::IPG::SIPGData* reqData,
                            WongWork::IPG::SIPGData* resData, Stream* stream);       // 0x0854c4f6
    void ProcessIPGUserDataJpnEx(CUser* user, WongWork::IPG::SIPGData* reqData,
                                 WongWork::IPG::SIPGData* resData, Stream* stream);  // 0x081783ea
    void ProcessIPG_ResultInput(CUser* user, WongWork::IPG::SIPGData* reqData,
                                WongWork::IPG::SIPGData* resData, Stream* stream);   // 0x0817866c
    void ProcessIPG_ResultOutput(CUser* user, WongWork::IPG::SIPGData* reqData,
                                 WongWork::IPG::SIPGData* resData, Stream* stream);  // 0x08178676
    void ProcessIPG_ResultQuery(CUser* user, WongWork::IPG::SIPGData* reqData,
                                WongWork::IPG::SIPGData* resData, Stream* stream);   // 0x081784d4
    // ---- 台湾计费记录 ORIG 0x0817991e ----
    void BillingRecord(CUser* user, int a3, int a4, int a5,
                       const char* itemName, const char* accName, int priceType,
                       unsigned long long uid, char payFlag);
    // [AnonLane round8 2026-09-11 · P139] 第三参拼写按 ORIG prett 串定：ORIG
    // `_ZZN18ItemVendingMachine14_GetItemStringEPK14CCeraShopGoodsPcjE19__PRETTY_FUNCTION__`
    // r 0x08b870e0 size 0x4e 串尾为 `size_t)`（我方 `unsigned int` 版 size 0x54，差 6）。
    // 32 位 size_t == unsigned int ⇒ mangled `…Pcj` / 布局 / 调用点零变化。
    void _GetItemString(const CCeraShopGoods* goods, char* buf, size_t len);       // 0x0817645a
private:
    // ORIG ctor 0x854c3e4 / dtor 0x854c42a：CCeraShop@+0 +
    // CeraShopPurcahseCountBonus@+8（成员自动构造/析构，顺序一致）。
    WongWork::CCeraShop m_ceraShop;            // +0x00
    ARAD::CeraShopPurcahseCountBonus m_bonus;  // +0x08
};

#endif  // DNF_GAME_ITEM_VENDING_MACHINE_H_
