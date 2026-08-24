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

class CUser;      // 前向（参数指针；完整定义在 CUser.h）
class Inven_Item; // 前向（容器元素；完整定义在 Inven_Item.h）
class CCeraShopGoods;  // 前向（_CheckBuyingCondition/_CheckValidCount 形参；完整定义在 CItemList.h）

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

// ORIG 布局：+0x00 SpecialItemHandler* m_handler（ctor new 0x74）+0x04 void* m_field4；
// ctor 0x083202b8 / dtor 0x08320314，对象 sizeof = 8（非多态）。
class CCeraShop
{
public:
    // ORIG stCeraShopItemParam_t（ctor 0x0817a244）：两 vector，sizeof 0x18。
    // +0x00 std::vector<std::pair<int,int>>（respond_special_item_package 遍历配对 itemIdx→count）
    // +0x0c std::vector<tagPackageItemSelection>（已选包条目，package 处理时迭代/erase）
    struct stCeraShopItemParam_t
    {
        stCeraShopItemParam_t() {}
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
    bool _CheckValidCount(const CCeraShopGoods* const* goodsList);                 // 0x08323af4
    int checkBuyingCondition(CUser* user, unsigned int a, unsigned long itemIdx,
                             unsigned int b, unsigned short c, unsigned char d,
                             unsigned short e);                                 // 0x08323f2a
    int _CheckBuyingCondition(CUser* user, unsigned int a, unsigned long itemIdx,
                              unsigned int b, unsigned short c, unsigned char d,
                              unsigned short e);                                // 0x08323f86
    int _CheckConditionPutonAvatar(CUser* user);                               // 0x08324966
    bool ProcessSpecialItem(CUser* user, int itemIdx, int count, int& restrictCode,
                            long curSec, bool flag,
                            std::vector<Inven_Item>& items,
                            stCeraShopItemParam_t& param,
                            char c1, char c2, bool flag2, int extra);            // 0x0832587e
    void SaveFeaturedIdx(CUser* user, int restrictCode, int curSec, int d);      // 0x08325a30
    void _ClosePremium(CUser* user, int type);                                   // 0x08327bb6
    void _sendPremiumPacket(CUser* user, int a, int type, int remainSec);        // 0x083277dc
    void ProcessError(CUser* user, unsigned short a, int b, unsigned int c,
                      int d, int e, char f);                                      // 0x08323978

    // ---- SpecialItemHandler::respond_special_item_* 消费的 CCeraShop 方法 ----
    // （声明自 ORIG mangled 符号；实现由 CCeraShop 还原批次提供，见 WongWork__CCeraShop 报告。）
    void setOriginalItem(int itemIdx);                                            // 0x08327ee6
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

private:
    SpecialItemHandler* m_handler;  // +0x00
    void* m_field4;                 // +0x04
};

}  // namespace WongWork

#endif  // GAME_CCERASHOP_H
