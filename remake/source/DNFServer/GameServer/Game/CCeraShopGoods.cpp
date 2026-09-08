// ============================================================================
// df_game_r 还原 —— STCeraShopItem / CCeraShopGoods（商城商品，G4 商城/交易域）
// 参照 docs/class_func_reports/STCeraShopItem.md、CCeraShopGoods.md +
// ORIG 反汇编逐函数核对。布局见 CItemList.h。
// ============================================================================

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "CItemList.h"
#include "CDataManager.h"

// ============================================================================
// STCeraShopItem
// ============================================================================

// ---- C1 @ 0x88fd974 ----
STCeraShopItem::STCeraShopItem()
    : m_name()
{
    clear();
    m_field84 = 0;
}

// ---- clear @ 0x88fd9c4 ----
// [b28d-35] 翻正（推翻原 N18a 判词）：m_field44 写 ORIG 为 `mov $0x0,%edx;
// mov %edx,0x44(%eax)` 两段（4.1 常数寄存器中转）——按 P238 用 float 视图
// `m_field44f = 0.0f;` 即复现该 edx 中转形（int 视图恒折 `movl $0x0,
// 0x44(%eax)` 单段）。字段经匿名 union 双视图，sizeof/偏移/mangled 不变。
void STCeraShopItem::clear()
{
    m_field0 = (char)0xff;
    m_field4 = 0;
    m_field8 = 0xfffffffe;
    m_fieldc = 0xffffffff;
    m_field10 = 0;
    m_field14 = (ENUM_CERA_SHOP_ITEM_NO)0xffffffff;
    m_field18 = (char)0xff;
    m_field1c = 1;
    m_field20 = 0;
    m_field22 = 0;
    m_field24 = 0;
    m_field2c = 0;
    m_field30 = 0;
    m_field38 = 0;
    m_field34 = 0;
    m_field3c = (char)0xff;
    m_name = "";
    m_field80 = 0;
    m_field44 = 0.0f;
    m_field48 = 0;
    m_field4c = 0;
    m_field50 = 0;
    m_field51 = 0;
    m_field52 = 0;
    m_field53 = 1;
    m_field54 = 0xfffffffe;
    m_field58 = 0xffffffff;
    m_field5c = 0;
    m_field60 = (char)0xc;
    m_field62 = 0;
    m_field64 = 0;
    m_field68 = 0;
    m_field6c = 0;
    m_field70 = 0;
    m_field74 = 0;
    m_field78 = 0;
    m_field28 = 0;
    m_field7c = 0xffffffff;
}

// ---- operator== @ 0x891a89e ----
bool STCeraShopItem::operator==(const STCeraShopItem& other)
{
    // ORIG 0x0891a8a1：先载 other（0xc(%ebp)）后载 this，cmp %eax,%edx。
    if (other.m_field8 == m_field8)
        return true;
    return false;
}

// ---- 访问器 ----
int STCeraShopItem::getCeraPrice()
{
    return m_field38;
}

int STCeraShopItem::getGoldPrice()
{
    return m_field2c;
}

bool STCeraShopItem::isLimitSaleItem()
{
    if (m_field54 == -2)
        return false;
    return true;
}

// ============================================================================
// CCeraShopGoods
// ============================================================================

// ---- SetItem @ 0x8510c1a ----
// [b28d-solo-CCeraShopGoods] AE：m_itemIdx 读 ORIG 0x08510c24 为
// `mov 0x14(%eax),%eax; mov %eax,%edx` 两段物化 ⇒ 按 P254 把
// STCeraShopItem::m_field14 改为 ENUM_CERA_SHOP_ITEM_NO（enum 源字段→int
// 目标保留 eax→edx 中转；int 字段 4.4 恒折单段）。原 N18a 判词作废
// （P248 档③类型旋钮）。sizeof/偏移/mangled 不变。
void CCeraShopGoods::SetItem(const STCeraShopItem& item)
{
    m_itemIdx = item.m_field14;
    m_stackCount = item.m_field1c;
    m_goldPrice = item.m_field2c;
    m_winPoint = item.m_field30;
    m_ceraPrice = item.m_field38;
    m_groupIdx = item.m_fieldc;
    m_featuredIdx = item.m_field10;
    m_sticker = (int)(signed char)item.m_field20;
    m_expiryDate = (int)(short)item.m_field22;
    m_bonusWeight = item.m_field44;
    m_field30 = item.m_field4c;
    m_giftDisallow = item.m_field50;
    m_ceraPointDisallow = item.m_field51;
    m_stackBuyable = item.m_field53;
    m_pItem = G_CDataManager()->find_item(m_itemIdx);
    m_mileagePrice = item.m_field34;
    m_subGroupIdx = (int)(signed char)item.m_field0;
}

// ---- C1 @ 0x8514ea0 ----
CCeraShopGoods::CCeraShopGoods(const STCeraShopItem& item)
{
    SetItem(item);
}

// ---- weak 访问器 ----
int CCeraShopGoods::GetItemIdx() const
{
    return m_itemIdx;
}

int CCeraShopGoods::GetStackCount() const
{
    return m_stackCount;
}

int CCeraShopGoods::GetGoldPrice() const
{
    return m_goldPrice;
}

int CCeraShopGoods::GetWinPoint() const
{
    return m_winPoint;
}

int CCeraShopGoods::GetCeraPrice() const
{
    return m_ceraPrice;
}

int CCeraShopGoods::GetMileagePrice() const
{
    return m_mileagePrice;
}

int CCeraShopGoods::GetSubGroupIndex() const
{
    return m_subGroupIdx;
}

int CCeraShopGoods::GetGroupIdx() const
{
    return m_groupIdx;
}

int CCeraShopGoods::GetFeaturedIdx() const
{
    return m_featuredIdx;
}

int CCeraShopGoods::GetSticker() const
{
    return m_sticker;
}

int CCeraShopGoods::GetExpiryDate() const
{
    return m_expiryDate;
}

float CCeraShopGoods::GetBonusWeight() const
{
    // [b28d-solo-CCeraShopGoods] N85 封顶（10 vs 9，助记符序列差 2 条 mov）：
    // ORIG 0x0854c588 `mov 0x2c(%eax),%eax; mov %eax,-0x4(%ebp); flds
    // -0x4(%ebp)`（sub $0x4，具名 float 局部被直接 flds）。4.4-O0 对
    // 「具名 float 局部 + return」恒多发 `mov -0x4,%eax; mov %eax,-0x14;
    // flds -0x14`（返回临时槽，sub $0x14）。五形实测同形不可翻：①具名
    // float 局部（本形）②声明后赋值两句 ③register float ④float[1] 元素
    // ⑤P226 语句表达式；直写 `return m_bonusWeight;` 折 `flds disp`（5 条，
    // 更远）。语义/值域一致，取最短可读形。
    float v = m_bonusWeight;
    return v;
}

CItem* CCeraShopGoods::GetRelatedItem() const
{
    return m_pItem;
}

unsigned char CCeraShopGoods::isGiftDisallowance_() const
{
    return m_giftDisallow;
}

unsigned char CCeraShopGoods::isCeraPointDisallowance_() const
{
    return m_ceraPointDisallow;
}

unsigned char CCeraShopGoods::isStackBuyable() const
{
    return m_stackBuyable;
}

// ============================================================================
// importCashShopItemList（商城脚本装载，ORIG 0x88fdcb5）
// 语义：加载 RDAR 商城脚本；按分区扫描商品行（每行首字段为条目序号），
// 以 STCeraShopItem 逐项填入商品向量；[limit] 类分区填限购映射。
// ============================================================================
// 全局 C++ 权威签名（由 DNFLexWrapperHelpers.cpp 提供，匹配 ORIG mangled 符号）：
bool loadRDARScriptFile(const char* directory, const char* path);
bool ScanType(std::string& token, bool consume);
int ScanInt(bool* ok);
bool ScanStr(std::string* value);

// [b28d-solo-CCeraShopGoods] N 封顶（§6.2 三件套口径，ORIG @0x088fdcb5
// size 0x1df4 = 1768 insn）：
//  ① gap = 1768 vs 1778（我方多 10 条，0.6%）；
//  ② 调用点差集（p167）：MISSING 0；EXTRA 仅 +2 `stCeraShopBonusItemInfo_t::
//     D1Ev` —— ORIG 该 TU 中该类型为 trivial（bi 直接落栈槽 -0xc0/-0xbc/
//     -0xb8，无 ctor/dtor 调用，0x088ff2c4..0x088ff36f 实证）；重建共享头
//     CItemList.h:148 声明了用户析构（ORIG W 0x089363da，W9_StructMisc.cpp:23
//     定义）⇒ 局部 bi 析构 2 处调用。消除须删共享头析构声明并同步
//     W9_StructMisc.cpp 定义（跨 TU，非本泳道）。原 24 项差集已全部闭合：
//     [cart/gift/cerapoint disallowance] 拆为三个独立分支（各 probe/find/
//     ScanInt/eq 条件）、[not stackable buy] 与尾扫描用具名 endIt 复现
//     ne<iterator,const_iterator>+转换构造、[npc info] 改函数级 str2、
//     [exceptional...] 补 pair<unsigned int,int> 局部；
//  ③ 帧/槽：ORIG sub $0x190、section@-0x84 vs 4.4 sub $0x180、section@-0x7c
//     = N82（4.1 不按 lifetime 复用槽 / 4.4 槽按首次使用序分配）＋主链空
//     else nop;jmp 岛 jump-threading ＋ N89（STL 内联实例）族；逐块语义
//     比对一致（全部 ScanInt/ScanStr/push_back/find/迭代器调用点匹配）。
bool importCashShopItemList(
    char* path, std::vector<STCeraShopItem>* itemList,
    std::vector<stCeraShopBonusInfo_t>& bonusList, void* param4, int* param5,
    std::map<int, std::vector<std::vector<int> > >* limitMap,
    std::vector<int>* param7)
{
    // [b28d-solo-CCeraShopGoods] 判词见函数头：calls 差集仅 +2（共享头
    // 析构声明 vs ORIG trivial 视图），gap -10，帧/槽 N82 族。
    if (!loadRDARScriptFile("", path))
        return 0;

    // 按 ORIG 0x88fdcb5 完整转写。itemList 指向 16 个连续 vector<STCeraShopItem>
    // 桶（stride 0xc，调用方 shopList[0x10]）。分区→槽位：avatar=0 coin=1
    // item=2 creature=3 package=7 premium=9 mileage=11 avatar-dye=15。
    // 头部仅清 mileage 桶（0x088fdcef `add $0x84` = itemList[11]）；其余桶
    // 由各自分区块进入时 clear（0x088fdea2 avatar、0x088fe14c coin …）。
    itemList[11].clear();

    // ORIG 0x088fdcf4..0x088fdd28：两个串（-0x84/-0x88）+ itemIdx(-0x8c)=0、
    // ok(-0x8d)=0、flag0(-0x54)=0、seq(-0x50)=0 全为函数级槽，各分区共享。
    std::string section;       // -0x84
    std::string str2;          // -0x88（[type]/局部串段消费）
    int itemIdx = 0;           // -0x8c（各分区首列暂存）
    bool ok = false;           // -0x8d
    int flag0 = 0;             // -0x54（尾组 field0 经此 int 槽 byte 化）
    int seq = 0;               // -0x50（avatar 族 m_field4 序数）

    while (true)
    {
        if (!ScanType(section, true))
            break;

        if (section == "[cerashop banner]")
        {
            
            ScanInt(&ok);                     // local_50（仅消费）
        }
        else if (section == "[npc info]")
        {
            for (int i = 0; i < 0x10; ++i)
            {
                // [b28d-solo-CCeraShopGoods] ORIG 0x088fddb0：ScanStr 写函数级
                // str2（-0x88），无局部 string（ORIG 仅 2 次 SsC1Ev）。
                ScanStr(&str2);
                
                ScanInt(&ok);
            }
        }
        else if (section == "[category event tag]")
        {
            
            while (true)
            {
                int v = ScanInt(&ok);
                if (ok != true)
                    break;
                if (param7)
                    param7->push_back(v);
            }
        }
        else if (section == "[avatar]" || section == "[national avatar]")
        {
            if (section == "[avatar]")
                itemList[0].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                // ORIG 0x088fdee3..0x088fdf75：字段列 ScanInt 传 NULL
                // （`movl $0x0,(%esp)`），写序 field4←seq、field8←itemIdx、
                // 14/22/20/10/3c、随后 flag0 槽(-0x54)置 0 → fieldc=0、
                // field1c=1、field0=flag0（byte 化自 int 槽）。
                STCeraShopItem item;
                item.m_field4 = seq;
                ++seq;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field22 = ScanInt(static_cast<bool*>(0));
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_field3c = ScanInt(static_cast<bool*>(0));
                flag0 = 0;
                item.m_fieldc = 0;
                item.m_field1c = 1;
                item.m_field0 = flag0;
                itemList[0].push_back(item);
            }
        }
        else if (section == "[dont trade avatar]")
        {
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field4 = seq;
                ++seq;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field22 = ScanInt(static_cast<bool*>(0));
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_field3c = ScanInt(static_cast<bool*>(0));
                flag0 = 0;
                item.m_fieldc = 0;
                item.m_field1c = 1;
                item.m_field0 = 0;                     // 0x088fe2…：本块尾写为立即数
                item.m_field7c = ScanInt(static_cast<bool*>(0));   // 仅消费，不入桶
            }
        }
        else if (section == "[coin]" || section == "[national coin]")
        {
            if (section == "[coin]")
                itemList[1].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                // ORIG 0x088fe182..0x088fe2bf：m_field0 立即数字节写在前，
                // 字段列 ScanInt 传 NULL，尾组 = m_name="" + flag 槽(-0x54)
                // → m_fieldc → m_field0 再写。
                STCeraShopItem item;
                item.m_field0 = 1;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field1c = ScanInt(static_cast<bool*>(0));
                item.m_field2c = ScanInt(static_cast<bool*>(0));
                item.m_field30 = ScanInt(static_cast<bool*>(0));
                item.m_field38 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_name = "";
                flag0 = 1;
                item.m_fieldc = 1;
                item.m_field0 = flag0;
                itemList[1].push_back(item);
            }
        }
        else if (section == "[avatar dye]")
        {
            itemList[15].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field0 = 2;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field1c = ScanInt(static_cast<bool*>(0));
                item.m_field2c = ScanInt(static_cast<bool*>(0));
                item.m_field30 = ScanInt(static_cast<bool*>(0));
                item.m_field38 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_name = "";
                flag0 = 2;
                item.m_fieldc = 2;
                item.m_field0 = flag0;
                itemList[15].push_back(item);
            }
        }
        else if (section == "[item]" || section == "[national item]")
        {
            if (section == "[item]")
                itemList[2].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field0 = 2;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field1c = ScanInt(static_cast<bool*>(0));
                item.m_field2c = ScanInt(static_cast<bool*>(0));
                item.m_field30 = ScanInt(static_cast<bool*>(0));
                item.m_field38 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_name = "";
                flag0 = 2;
                item.m_fieldc = 2;
                item.m_field0 = flag0;
                itemList[2].push_back(item);
            }
        }
        else if (section == "[premium]" || section == "[national premium]")
        {
            if (section == "[premium]")
                itemList[9].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field0 = 9;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field1c = ScanInt(static_cast<bool*>(0));
                item.m_field2c = ScanInt(static_cast<bool*>(0));
                item.m_field30 = ScanInt(static_cast<bool*>(0));
                item.m_field38 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_name = "";
                flag0 = 9;
                item.m_fieldc = 9;
                item.m_field0 = flag0;
                itemList[9].push_back(item);
            }
        }
        else if (section == "[creature]" || section == "[national creature]")
        {
            if (section == "[creature]")
                itemList[3].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field0 = 3;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field1c = ScanInt(static_cast<bool*>(0));
                item.m_field2c = ScanInt(static_cast<bool*>(0));
                item.m_field30 = ScanInt(static_cast<bool*>(0));
                item.m_field38 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_name = "";
                flag0 = 3;
                item.m_fieldc = 3;
                item.m_field0 = flag0;
                itemList[3].push_back(item);
            }
        }
        else if (section == "[NPC]")
        {
            
            ScanInt(&ok);                          // local_90
        }
        else if (section == "[type]")
        {
            ScanStr(&str2);                        // 0x088f??：写函数级 -0x88
        }
        else if (section == "[item group index]")
        {
            
            ScanInt(&ok);                          // local_44
        }
        else if (section == "[mileage item]")
        {
            itemList[11].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field8 = itemIdx;
                item.m_field0 = ScanInt(static_cast<bool*>(0));   // +0x00 类型
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field1c = ScanInt(static_cast<bool*>(0));
                item.m_field34 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_fieldc = 0xb;
                item.m_name = "";
                itemList[11].push_back(item);
            }
        }
        else if (section == "[package]" || section == "[national package]")
        {
            if (section == "[package]")
                itemList[7].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field2c = ScanInt(static_cast<bool*>(0));
                item.m_field30 = ScanInt(static_cast<bool*>(0));
                item.m_field38 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_field22 = ScanInt(static_cast<bool*>(0));
                item.m_field3c = ScanInt(static_cast<bool*>(0));
                item.m_name = "";
                flag0 = 7;                             // ORIG 尾组经 -0x54 int 槽
                item.m_fieldc = 7;
                item.m_field1c = 1;
                item.m_field0 = flag0;
                itemList[7].push_back(item);
            }
        }
        else if (section == "[cart disallowance]")
        {
            // [b28d-solo-CCeraShopGoods] ORIG 0x088fed3b..0x088feea0：三个分区各为
            // 独立分支（各自 probe C1/D1 + find + ScanInt），循环条件用
            // `it == end()`（__gnu_cxx::eq）而非 !=（0x088fee36 实证）。
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem probe;
                probe.m_field8 = itemIdx;
                for (int i = 0; i < 0x10; ++i)
                {
                    std::vector<STCeraShopItem>::iterator it =
                        std::find(itemList[i].begin(), itemList[i].end(), probe);
                    if (it == itemList[i].end())
                        continue;
                    it->m_field52 = 1;
                }
            }
        }
        else if (section == "[gift disallowance]")
        {
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem probe;
                probe.m_field8 = itemIdx;
                for (int i = 0; i < 0x10; ++i)
                {
                    std::vector<STCeraShopItem>::iterator it =
                        std::find(itemList[i].begin(), itemList[i].end(), probe);
                    if (it == itemList[i].end())
                        continue;
                    it->m_field50 = 1;
                }
            }
        }
        else if (section == "[cerapoint disallowance]")
        {
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem probe;
                probe.m_field8 = itemIdx;
                for (int i = 0; i < 0x10; ++i)
                {
                    std::vector<STCeraShopItem>::iterator it =
                        std::find(itemList[i].begin(), itemList[i].end(), probe);
                    if (it == itemList[i].end())
                        continue;
                    it->m_field51 = 1;
                }
            }
        }
        else if (section == "[max gift count]")
        {
            
            int v = ScanInt(&ok);
            if (ok != true)
                break;  // ORIG 0x88ff1a5：ScanInt 失败 goto LAB_088ff96d（退出主循环走尾扫描）
            if (param5)
                *param5 = v;
        }
        else if (section == "[bonus item info]")
        {
            while (true)
            {
                stCeraShopBonusInfo_t info;
                
                info.m_field0 = ScanInt(&ok);
                if (ok != true)
                    break;
                info.m_field4 = ScanInt(&ok);
                if (ok != true)
                    return 0;   // ORIG 0x88ff23a：第 2 个 ScanInt 失败 → return 0
                int count = ScanInt(&ok);
                // ORIG 0x88ff26b：第 3 个 ScanInt 失败 → return 0
                if (ok != true)
                    return 0;
                info.m_items.reserve(count);
                int total = 0;
                for (int i = 0; i < count; ++i)
                {
                    stCeraShopBonusItemInfo_t bi;
                    bi.m_field0 = ScanInt(&ok);
                    if (ok != true)
                        return 0;   // ORIG 0x88ff2c4：失败 → return 0
                    bi.m_field4 = (unsigned short)ScanInt(&ok);
                    if (ok != true)
                        return 0;   // ORIG 0x88ff2f5
                    bi.m_field8 = ScanInt(&ok);
                    if (ok != true)
                        return 0;   // ORIG 0x88ff327
                    total += bi.m_field8;
                    info.m_items.push_back(bi);
                }
                if (total == 1000)
                    bonusList.push_back(info);
                else
                    printf("bonus rate error %d", total);
                // ORIG 0x88ff3a5：总概率≠1000 → 打印后 return 0
                if (total != 1000)
                    return 0;
            }
        }
        else if (section == "[not stackable buy]")
        {
            std::vector<int> indexList;
            
            while (true)
            {
                int v = ScanInt(&ok);
                if (ok != true)
                    break;
                indexList.push_back(v);
            }
            for (int i = 0; i < 0x10; ++i)
            {
                // [b28d-solo-CCeraShopGoods] ORIG 0x088ff49f begin → 0x088ff4c2
                // end → 0x088ff4da const_iterator 转换构造 → 0x088ff56a
                // ne<iterator,const_iterator>；具名 endIt 复现混合比较。
                std::vector<STCeraShopItem>::iterator it = itemList[i].begin();
                std::vector<STCeraShopItem>::const_iterator endIt = itemList[i].end();
                for (; it != endIt; ++it)
                {
                    for (unsigned int k = 0; k < indexList.size(); ++k)
                    {
                        if (it->m_field8 == indexList[k])
                            it->m_field53 = 0;
                    }
                }
            }
        }
        else if (section == "[recommended avatar list]")
        {
            
            while (true)
            {
                int mapKey = ScanInt(&ok);
                if (ok != true)
                    break;
                std::vector<int> list;
                for (int i = 0; i < 10; ++i)
                {
                    
                    int v = ScanInt(&ok);
                    if (v == 0)
                        v = -1;
                    list.push_back(v);
                }
                if (mapKey >= 0 && mapKey < 0xb && limitMap)
                    (*limitMap)[mapKey].push_back(list);
            }
        }
        else if (section == "[restricted premium map]")
        {
            
            do {
                int a = ScanInt(&ok);
                if (ok != true)
                    break;
                int b = ScanInt(&ok);
            } while (ok);
        }
        else if (section == "[immediately adaptive product]")
        {
            
            do {
                ScanInt(&ok);
            } while (ok);
        }
        else if (section == "[avatar coin]")
        {
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field1c = ScanInt(static_cast<bool*>(0));
                item.m_field2c = ScanInt(static_cast<bool*>(0));
                item.m_field30 = ScanInt(static_cast<bool*>(0));
                item.m_field38 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_field0 = 2;
                item.m_fieldc = 2;
                item.m_name = "";
                itemList[0].push_back(item);
            }
        }
        else if (section == "[exceptional product price in first server]")
        {
            
            do {
                // [b28d-solo-CCeraShopGoods] ORIG 0x088ff915 pair<unsigned int,int>
                // 默认构造（C1Ev），0x088ff923/0x088ff94b 两次 ScanInt 逐字段写。
                std::pair<unsigned int, int> p;
                p.first = (unsigned int)ScanInt(&ok);
                if (ok != true)
                    break;
                p.second = ScanInt(&ok);
            } while (ok);
        }
    }

    // LAB_088ff96d：全部 16 桶中，item->m_field10 != 0 的条目置 m_field50 = 1。
    for (int i = 0; i < 0x10; ++i)
    {
        // ORIG 0x088ff997 begin → 0x088ff9ba end → 0x088ff9d2 const_iterator
        // 转换构造 → 0x088ffa26 ne<iterator,const_iterator>。
        std::vector<STCeraShopItem>::iterator it = itemList[i].begin();
        std::vector<STCeraShopItem>::const_iterator endIt = itemList[i].end();
        for (; it != endIt; ++it)
        {
            if (it->m_field10 != 0)
                it->m_field50 = 1;
        }
    }
    return 1;
}
