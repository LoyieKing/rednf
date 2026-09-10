#ifndef GAME_ARAD_ITEM_BONUS_H_
#define GAME_ARAD_ITEM_BONUS_H_

// ============================================================================
// df_game_r 还原 —— ARAD 物品/奖励域（W6_ARAD_ItemBonus 批次）。
//
// 源反汇编：df_game_r（elf32-i386）docs/class_func_reports/{ARAD__ARAD_ITEM,
// ARAD__CeraShopPurcahseCountBonus,ARAD}/{对应函数}.md；未出报告者已按 ORIG
// nm/objdump 核对 mangled 与地址（见 /tmp/df_batches/W6_ARAD_ItemBonus.md）。
//
// 符号口径：
//   - ARAD::ARAD_ITEM 三个方法 ORIG 函数体不引用 this，按静态成员声明
//     （mangled _ZN4ARAD9ARAD_ITEM... 与普通成员一致，ABI 兼容）。
#include <map>
#include <vector>

#include "AradRabbitBox.h"
#include "ItemVendingMachine.h"  // ARAD::PurcahseBonus（本批次实现其 == / Bonus 五方法）
#include "Arad_DataManager.h"    // PurchaseLimitItem::Info 完整类型

class CUser;
class CStackableItem;
class Inven_Item;


namespace ARAD_DATAS
{
// ---------------------------------------------------------------------------
// AradMailItemInfo —— ORIG C1 @0x081a3fc2 (W)：清零 +0/+0xa/+0xc/+0x10/+0x14。
// GetItemAttr @0x08197e12 (W const)：返回 +0x14 字节。sizeof = 0x18。
// 各字段语义为邮件物品载荷的直拷贝通道（send_mail 逐字段转写）。
// ---------------------------------------------------------------------------
struct AradMailItemInfo
{
    AradMailItemInfo()
        : m_flag(0), m_fieldA(0), m_fieldC(0), m_field10(0), m_itemAttr(0) {}

    // ORIG GetItemAttr _ZNK10ARAD_DATAS16AradMailItemInfo11GetItemAttrEv W
    unsigned char GetItemAttr() const { return m_itemAttr; }  // 0x08197e12
    unsigned char m_flag;    // +0x00 → SIG +0x2a
    int m_field4;            // +0x04（send_mail 0x081a329e 写 1）→ SIG +0x2f
    unsigned char m_field8;  // +0x08（send_mail 0x081a329e 写 2）
    unsigned short m_fieldA; // +0x0a → SIG +0x34
    int m_fieldC;            // +0x0c → SIG +0x2b
    int m_field10;           // +0x10 → SIG +0x36
    unsigned char m_itemAttr;// +0x14（GetItemAttr 读取位）
};
// ---------------------------------------------------------------------------
// AradMailInfo —— ORIG C1 @0x081a3ff2 (W)：清零以下标量字段后构造内嵌 item。
// sizeof ≈ 0x38（尾部对齐）。字符串指针字段由调用方保证生存期。
// ---------------------------------------------------------------------------
struct AradMailInfo
{
    AradMailInfo()
        : m_isSystemMail(0), m_field4(0), m_field8(0), m_fieldC(0),
          m_field10(0), m_field14(0), m_content(0), m_name(0), m_item() {}

    unsigned char m_isSystemMail; // +0x00 → SIG +0x15f（系统邮件标志）
    int m_field4;                 // +0x04 → SIG +0x26
    int m_field8;                 // +0x08 → SIG +0x15b
    int m_fieldC;                 // +0x0c → SIG +0x22
    int m_field10;                // +0x10 → SIG +0x1e
    int m_field14;                // +0x14 → SIG +0x59（short）
    char* m_content;              // +0x18 → memset/strncpy 到 SIG +0x5b [0x100]
    char* m_name;                 // +0x1c → strncpy 到 SIG +0x00 前 0x1d 字节
    AradMailItemInfo m_item;      // +0x20 内嵌邮件物品
};
}  // namespace ARAD_DATAS

namespace ARAD
{
class CeraShopPurcahseCountBonus;  // 完整定义在 ItemVendingMachine.h

// ORIG _ZN4ARAD16CheckAccountItemEP5CUserjRN17PurchaseLimitItem4InfoE T @0x08196ac8
// Singleton<Arad_DataManager>::Get()->getPurchaseLimitItem()->find(itemIdx, out)
// 按汇编原样：命中（true）→ 返回 false；未命中 → 返回 true。
bool CheckAccountItem(CUser* user, unsigned int itemIndex,
                      class PurchaseLimitItem::Info& out);

// ORIG _ZN4ARAD9send_mailERN10ARAD_DATAS12AradMailInfoE T @0x08196b03（返回 1）。
// 定义：Arad_ItemBonus.cpp（实现已在）。
// [N1D 2026-09-11 签名轴·父泳道批准] 返回类型 int → bool：其唯一调用点
//   LinkClosureE.cpp:617 `return ARAD::send_mail(info);` 在 ORIG 0x081a3407 之后
//   直落 add/pop/ret（无 test/setne 截断）⇒ 与 Arad_GiftItem_Set::send_mail 同为
//   bool。mangled 不变（返回类型不入 mangled）；定义侧由 Arad_ItemBonus 属主同批落地。
bool send_mail(ARAD_DATAS::AradMailInfo& info);

// ORIG _ZN4ARAD35importCashShopPurcahseBonusItemListEPcRSt3mapIiSt6vectorINS_13PurcahseBonusESaIS3_EESt4lessIiESaISt4pairIKiS5_EEE
// T @0x088b62ec —— "CashShop/PurcahseBonusItem.shp" 解析器（LoadScript 直接依赖）。
int importCashShopPurcahseBonusItemList(
    char* path,
    std::map<int, std::vector<PurcahseBonus> >& out);

class ARAD_ITEM
{
public:
    // T @0x081973fa —— 走 CCeraShop::ProcessSpecialItem 发放特殊商店物品。
    static bool arad_process_item(CUser* user, Inven_Item const& item);
    // T @0x08196dd3 —— 秘密商店事件 buff 包广播。
    static void arad_secret_shop_event_buff(CUser* user, unsigned long param);
    // T @0x08196ed7 —— 秘密商店事件物品发放（CRabbitBox 列表 / 事件物品分支）。
    static void arad_secret_shop_event_item(CUser* user, unsigned long param,
                                            CStackableItem* stackable);
};
}  // namespace ARAD

#endif  // GAME_ARAD_ITEM_BONUS_H_
