#ifndef GAME_CCERASHOP_H
#define GAME_CCERASHOP_H

// ---------------------------------------------------------------------------
// df_game_r —— WongWork::CCeraShop 权威声明（唯一）。
// 归并自 CCeraShop_Special.cpp / CMailBoxHelper.cpp / GlobalData.cpp /
// Dispatcher_BuyCeraShopItem_impl.cpp 的局部 CCeraShop 声明（原布局/成员集不一致）。
// 方法签名与 mangled 符号见 docs/class_func_reports/WongWork__CCeraShop/README.md。
// ---------------------------------------------------------------------------

#include <vector>
#include <cstring>

#include "Inven_Item.h"   // gift 族按值传 Inven_Item，需完整类型

class CUser;      // 前向（参数指针；完整定义在 CUser.h）
class Stream;     // 前向（ProcessIPGUserData 形参）
class CCeraShopGoods;  // 前向（_CheckBuyingCondition/_CheckValidCount 形参；完整定义在 CItemList.h）
class CItem;      // 前向（autoUnsealItem 形参；完整定义在 CItem.h）
struct MSG_BUY_CERASHOP_ITEM;          // 前向（ProcessBuyRequest const& 形参；完整定义在 SigTypes.h）
namespace WongWork { namespace IPG { struct SIPGData; } }  // 前向（ProcessIPGUserData 形参）
class STSpecailItem_Param;   // 完整定义见本文件尾部（ORIG W ctor 0x08234ec2，inline）
class STSpecailItem_Result;

// ORIG SpecialItemHandler 为全局类（mangled _ZN18SpecialItemHandler…，非 WongWork 前缀）。
// 仅 CCeraShop 持有指针；权威声明见 SpecialItemHandler.h，实现见 CCeraShop_Special.cpp。
class SpecialItemHandler;

// ORIG 全局枚举 ENUM_CERA_SPECIALITEM（mangled 21ENUM_CERA_SPECIALITEM，非 WongWork 前缀）。
// 供 _onBuySpecialItem 的 2 参使用；仅已知调用方传值 6（booster）。命名保持占位，
// 调用处用整型字面量显式转型，避免编造语义名。
enum ENUM_CERA_SPECIALITEM
{
    ENUM_CERA_SPECIALITEM_UNKNOWN_0 = 0,
    ENUM_CERA_SPECIALITEM_BOOSTER = 6   // 值来自 respond_special_item_booster/package 汇编（movl $0x6）
};

// ORIG 全局类型 tagPackageItemSelection（mangled 23tagPackageItemSelection，非 WongWork 前缀）。
// respond_special_item_package 消费：位于 stCeraShopItemParam_t+0xc 的 vector<tagPackageItemSelection>。
// 布局（respond_special_item_package 汇编逐字段）：+0x00 int（itemIdx，与响应 itemIdx 比对）、
// +0x04 ushort（m_package274 外层索引）、+0x06 ushort（内层 vector 索引）；sizeof = 8。
struct tagPackageItemSelection
{
    int m_itemIdx;         // +0x00
    unsigned short m_outer; // +0x04（vector<vector<STStackablePackageData>> 外层索引）
    unsigned short m_inner; // +0x06（内层 vector 索引）
};

namespace WongWork
{

// ORIG 布局：+0x00 SpecialItemHandler* m_handler（ctor new 0x74）+0x04 int
// m_originalItemIdx（setOriginalItem 0x08327ee6 写入 / getOriginalItem 0x08327ef4 读取）；
// ctor 0x083202b8 / dtor 0x08320314，对象 sizeof = 8（非多态）。
class CCeraShop
{
public:
    // ORIG stCeraShopItemParam_t（ctor 0x0817a244）：两 vector，sizeof 0x18。
    // +0x00 std::vector<std::pair<int,int>>（respond_special_item_package 遍历配对 itemIdx→count）
    // +0x0c std::vector<tagPackageItemSelection>（已选包条目，package 处理时迭代/erase）
    struct stCeraShopItemParam_t
    {
        // ORIG ctor 0x0817a244：构造两 vector 后各调一次 clear。
        stCeraShopItemParam_t()
        {
            m_pairs.clear();
            m_sel.clear();
        }
        std::vector<std::pair<int, int> > m_pairs;     // +0x00
        std::vector<tagPackageItemSelection> m_sel;    // +0x0c
    };

    CCeraShop();                      // 0x083202b8
    ~CCeraShop();                     // 0x08320314

    bool IsSpecialItem(int itemIdx, int& outType, int& outValue, bool& flag);  // 0x08324a1e
    // _CheckBuyingCondition（CCeraShopGoods 重载）0x08324810 与 _CheckValidCount 0x08323af4：
    // 实现在 CCeraShop_Special.cpp（本任务），声明由本权威头提供。
    bool _CheckBuyingCondition(CUser* user, const CCeraShopGoods* goods,
                               unsigned int param3, unsigned char param4,
                               unsigned short param5, char param6, int& outCode);  // 0x08324810
    int checkBuyingCondition(CUser* user, unsigned int a, unsigned long itemIdx,
                             unsigned int b, unsigned short c, unsigned char d,
                             unsigned short e);                                 // 0x08323f2a
    // 六参重载（ORIG mangled jmjtht；实现在 CCeraShop_Special.cpp）。
    int _CheckBuyingCondition(CUser* user, unsigned int goodsNo, unsigned long itemIdx,
                              unsigned int stackCount, unsigned short expiry,
                              unsigned char selByte, unsigned short selSlot);   // 0x08323f86
    bool _CheckValidCount(CCeraShopGoods const** goodsList);                       // 0x08323af4
    int _CheckConditionPutonAvatar(CUser* user);                               // 0x08324966
    bool ProcessSpecialItem(CUser* user, int itemIdx, int count, int& restrictCode,
                            long curSec, bool flag,
                            std::vector<Inven_Item>& items,
                            stCeraShopItemParam_t& param,
                            char c1, char c2, bool flag2, int extra);            // 0x0832587e
    // ORIG 0x08325a30：体仅用 user(arg1)+DB 请求，无 this/成员；调用点 4 参槽无 this。
    static void SaveFeaturedIdx(CUser* user, int restrictCode, int curSec, int d);
    void _ClosePremium(CUser* user, int type);                                   // 0x08327bb6
    void _sendPremiumPacket(CUser* user, int a, int type, int remainSec);        // 0x083277dc
    void ProcessError(CUser* user, unsigned short a, int b, unsigned int c,
                      int d, int e, char f);                                      // 0x08323978

    // ---- SpecialItemHandler::respond_special_item_* 消费的 CCeraShop 方法 ----
    // （声明自 ORIG mangled 符号；实现由 CCeraShop 还原批次提供，见 WongWork__CCeraShop 报告。）
    void setOriginalItem(int itemIdx);                                            // 0x08327ee6
    int getOriginalItem();                                                        // 0x08327ef4
    void _onBuySpecialItem(CUser* user, ENUM_CERA_SPECIALITEM kind,
                           unsigned long itemIdx);                                // 0x08327454
    void _sendCargoUpgradePacket(CUser* user, int type);                          // 0x08327726
    void _sendCharacSlotExtendPacket(CUser* user, int type);                      // 0x08327674
    void _sendInventoryUpgradePacket(CUser* user, int type);                      // 0x083275be
    int _processBoosterSelect(CUser* user, int itemIdx, int count, int& result,
                              long curSec, bool flag,
                              std::vector<Inven_Item>& items,
                              stCeraShopItemParam_t& param, char c1, char c2);   // 0x08327894
    void _processAdditionalItem(CUser* user, unsigned long itemIdx,
                                unsigned short slot, unsigned char a,
                                unsigned char b,
                                std::vector<Inven_Item>& items,
                                stCeraShopItemParam_t& param,
                                unsigned char flag);                              // 0x083267a2
    // ---- 特殊商品子流程（本 TU 实现；签名与 ORIG mangled 一致）----
    bool Is2010ChuseokPacket(unsigned int itemIdx);                            // 0x083280dc（ORIG mangled …Ej）
    void _processCoin(CUser* user, unsigned short value, bool flag);            // 0x08325a88
    void autoUnsealItem(const CItem* item, Inven_Item& invItem);                // 0x08327f10
    int _processItem(CUser* user, Inven_Item item, int count, int& restrictCode,
                     long curSec, bool flag, bool save,
                     std::vector<Inven_Item>& items,
                     stCeraShopItemParam_t& param);                              // 0x08325b22
    int _processCreature(CUser* user, Inven_Item item, int& outCode,
                         int& outFlag, bool flag, int usablePeriod,
                         int expirationDate);                                    // 0x08325d66

    // ---- W6_CeraShopDeps 批次补齐（声明 append，ORIG mangled 核实于 df_game_r nm）----
    // 错误日志输入视图（LogError 0x0832033a 消费；字段语义未全还原，标注【推断】）。
    struct stErrorLogInput_t
    {
        int m_field0;            // +0x00（→ SIG_CERASHOP_ERROR_LOG +0x00）
        int m_field4;            // +0x04（→ +0x04）
        int m_field8;            // +0x08（→ +0x14）
        int m_fieldc;            // +0x0c（→ +0x18）
        unsigned short m_field10;// +0x10（→ +0x10）
    };
    // P159：ORIG 0x08320445 尾部 `mov $0x1,%ebx`+D1 后回存 ⇒ 实际返回 int 1
    // （mangled 不含返回类型；全仓无非语句式调用点）。
    int LogError(const stErrorLogInput_t& input) const;     // 0x0832033a
    bool CallSpecialItemHandler(int type, const STSpecailItem_Param& param,
                                STSpecailItem_Result& result);                     // 0x08325854
    void _ProcessIPGError(CUser* user, IPG::SIPGData* reqData,
                          IPG::SIPGData* resData, unsigned char errCodeA,
                          int errCodeB, unsigned int productNo, int subCode,
                          int code2, char flag);                                   // 0x083237f6
    // [b28c-L7][SIGFIX:WongWork::CCeraShop::ProcessBuyRequest] ORIG
    // __PRETTY_FUNCTION__ 串 = "bool WongWork::CCeraShop::ProcessBuyRequest(
    // CUser*, MSG_BUY_CERASHOP_ITEM const&)"（P174，
    // docs/function_audit/sig_return_mismatch_b28c.tsv 第 18 行）⇒ int→bool；
    // mangled 不含返回类型⇒符号不变，全仓无调用点（grep 实证）⇒ 零外部影响。
    bool ProcessBuyRequest(CUser* user, const MSG_BUY_CERASHOP_ITEM& msg);          // 0x832047a
    void ProcessIPGUserData(CUser* user, IPG::SIPGData* reqData,
                            IPG::SIPGData* resData, Stream* stream);               // 0x8321d2e
    void _processCoinGift(CUser* user, unsigned short currencyType,
                          unsigned int buyIdx, int cost);                          // 0x0832619c
    void _processCoinGift(CUser* user, unsigned short currencyType,
                          unsigned int buyIdx, int cost, int count);               // 0x08326332
    void _processItemGift(CUser* user, Inven_Item item,
                          unsigned int buyIdx, int giftKind);                      // 0x083264d6
    void _processAvatarGift(CUser* user, int slot, int periodDays, char c1, char c2,
                            char const* name, unsigned int buyIdx, int cost);      // 0x08326036
    void _processCreatureGift(CUser* user, Inven_Item item, unsigned int buyIdx,
                              int a4, int a5, int a6);                             // 0x083265e8
private:
    SpecialItemHandler* m_handler;  // +0x00

    int m_originalItemIdx;          // +0x04（setOriginalItem 写入 / getOriginalItem 读取）

};

}  // namespace WongWork

// ---------------------------------------------------------------------------
// STSpecailItem_Param / STSpecailItem_Result —— ORIG 全局类型（mangled 19STSpecailItem……
// 非 WongWork 前缀）。完整定义自 CCeraShop_Special.cpp 提升（2026-08-30，Fix7_I）。
// 布局（ORIG W ctor 0x08234ec2/0x08234f36 转写）：见下注释。
// 供 Dispatcher_CeraPackageOpen::process 真实调用 CallSpecialItemHandler。
// ---------------------------------------------------------------------------
struct STSpecailItem_Param
{
    STSpecailItem_Param(std::vector<Inven_Item>& items,
                        WongWork::CCeraShop::stCeraShopItemParam_t& cera)
    {
        m_items = &items;
        m_ceraParam = &cera;
        m_user = 0;
        m_itemIdx = 0;
        m_value = 0;
        m_count = 0;
        m_time = 0;
        m_flag = 0;
        m_char1e = 0;
        m_char1d = m_char1e;
        m_char1f = 0;
        m_extra = 1;
    }

    void* m_user;            // +0x00（ProcessSpecialItem 写 CUser*）
    int m_itemIdx;           // +0x04
    int m_value;             // +0x08
    void* m_items;           // +0x0c
    void* m_ceraParam;       // +0x10
    int m_count;             // +0x14
    long m_time;             // +0x18
    unsigned char m_flag;    // +0x1c
    unsigned char m_char1d;  // +0x1d
    unsigned char m_char1e;  // +0x1e
    unsigned char m_char1f;  // +0x1f
    int m_extra;             // +0x20
};

// ORIG _ZN20STSpecailItem_ResultC1Ev (W 0x08234f36)：仅清零首 4 字节。
struct STSpecailItem_Result
{
    STSpecailItem_Result() { m_value = 0; }
    int m_value;  // +0x00
};

#endif  // GAME_CCERASHOP_H
