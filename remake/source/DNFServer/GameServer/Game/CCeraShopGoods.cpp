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

// [BigRndOpt] 本 TU 内 stCeraShopBonusItemInfo_t 的 trivial 视图（用途与依据见
// importCashShopItemList 的 [bonus item info] 段判词）。ORIG 0x088ff2c4..0x088ff36f
// 该段全程 0 ctor / 0 dtor：三字段（u32@0 / u16@4 / u32@8，sizeof 0xc）直落栈槽
// -0xc0/-0xbc/-0xb8，再以槽地址作 push_back 实参。共享头 CItemList.h 为补发弱符号
// _ZN25stCeraShopBonusItemInfo_tD1Ev（ORIG W 0x089363da，宿主 W9_StructMisc.cpp）
// 声明了用户析构 ⇒ 具名 bi 会多发 2 处 D1Ev（p167 EXTRA +2 的唯一来源）。
// 删共享头声明属跨 TU（R1 禁区），故此处改用同布局 POD 镜像承担局部量：
// 两侧皆 standard-layout、公共初始序列一致，reinterpret_cast 取引用合法，
// 零导出符号、零可观察行为变化。
namespace
{
struct BonusItemView
{
    unsigned int m_field0;        // +0x00（ORIG `mov %eax,-0xc0(%ebp)`）
    unsigned short m_field4;      // +0x04（ORIG `mov %ax,-0xbc(%ebp)`）
    unsigned int m_field8;        // +0x08（ORIG `mov %eax,-0xb8(%ebp)`）
};
// 0xc 的三重实证（详见 CItemList.h:156 同题注释）：max_size@0x08936166 = UINT_MAX/12、
// allocate@0x08945d62 = n*3*4、使用点元素槽 -0xc0..-0xb4。
static_assert(sizeof(BonusItemView) == 0xc, "ORIG stCeraShopBonusItemInfo_t = 0xc (0x08936166/0x08945d62)");
static_assert(sizeof(BonusItemView) == sizeof(stCeraShopBonusItemInfo_t),
              "镜像视图须与共享头同尺寸（偏移 0/4/8 逐条按 ORIG 反汇编核对）");
}   // namespace

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
//
// [BigRndOpt 本波] 四套配平复核 + 三处等价改写（先定性质后动手）：
//  基线：p167 `MISSING 0 / EXTRA +2 _ZN25stCeraShopBonusItemInfo_tD1Ev`、
//        structdiff struct=131 / ndiff=1664 / clean 1739 vs 1753（+14）、
//        STR 33/33（ORIG 侧 33 个 unique 串我方全有，零 MISSING 零 EXTRA）。
//  ⇒ 指令数基本齐 ⇒ 属形态族，但 EXTRA +2 是真语义差（伪析构），逐处修：
//  (1) **镜像视图消伪 dtor**（拟 P334）：局部 bi 改 TU 内 trivial 镜像
//      BonusItemView（定义在本函数上方），push_back 实参位
//      `*reinterpret_cast<const stCeraShopBonusItemInfo_t*>(&bi)` 取引用不造
//      临时 ⇒ ORIG 该 TU 的 trivial 视图被复现，p167 EXTRA +2 → **0**。
//      依据：ORIG 0x088ff2c4/0x088ff2f5/0x088ff327 三字段直落 -0xc0/-0xbc/
//      -0xb8，全程无 ctor/dtor 站点；sizeof=0xc 三重实证见 CItemList.h:156。
//  (2) **[bonus item info] 尾组双判合单判**：ORIG 0x088ff389 `cmpl $0x3e8,
//      -0x2c(%ebp)` + 0x088ff390 `je 0x088ff3b1` 为**单判**——fall-through 侧
//      printf 后 0x088ff3a5 `mov $0x0,%esi; mov $0x1,%ebx`（return false 清理
//      链），跳走侧 0x088ff3b1 才 push_back(bonusList,info)、0x088ff3c6 置
//      state 2 → 0x088ff1f2 回 while 体首。原码「if(==1000) push_back else
//      printf; if(!=1000) return 0」在 -O0 复现两次 `cmpl $0x3e8`，第二次
//      `je` 恒不成立 = 死分支。合并后两判定同值，可观察行为不变。
//  (3) **[package] 尾组 `item.m_name = ""` 后移**：语句书写序 = ORIG 叶块物理序。
//      · m_name=""（`movl $<A>,0x4(%esp)` / `lea item; add $0x40,%eax; call
//        _ZNSsaSEPKc`）在 ORIG 位于 flag0/m_fieldc/m_field1c/m_field0 四组
//        赋值**之后**；两组写无数据依赖。
//      · **[cart]/[gift]/[cerapoint disallowance] probe 声明上提已试并撤回**
//      （双判据 A/B 实测，Main 口径「①指令缺口为主 / ②call 多重集为辅」）：
//        ORIG 的 ctor 物理位确在首个 ScanInt **之前**（0x088fed48 `lea -0x180`
//        / 0x088fed51 `call _ZN14STCeraShopItemC1Ev` → 0x088fed5c `call
//        _Z7ScanIntPb` → 0x088fed64 `mov %eax,-0x8c`），且 break 出口 ORIG 带
//        析构（0x088fed17 `lea -0x180; call _ZN14STCeraShopItemD1Ev`）。
//        上提形读数：struct **80** / ndiff **1644** / clean **1748**（gap +9）
//        / p167 **274/274**；
//        原写法读数：struct **86** / ndiff **1636** / clean **1733**（gap −6）
//        / p167 **274/274**。
//        ⇒ ②两形同为零差（4.4 把 break 出口的 D1Ev 折进共享清理块，站点数
//        不变），①上提反而把缺口从 −6 拉到 +9（多出的 9 条 = 3×`mov $state,
//        %ebx; jmp 共享块` 跳板）⇒ **按双判据撤回**，只留 m_name 后移。
//  现读数：p167 **274/274 双向零差**；struct 131→**86**；ndiff 1664→**1636**；
//  raw 1768 vs **1775**（基线 1780）、clean 1739 vs **1733**（gap +14 → −6）、
//  strict(autojunk=False) 双向 ndiff 1897→**1883**；TU 四数 139/279/near3/
//  diff5/missing95 **逐位不变**（两路径均 COMPILE OK：check_tu_routed
//  + build/game 单对象）。
//  残余 86 归因（封 N，性质 = ①缺口 −6 / ②call 零差 双小 ⇒ 直接归 N）：
//     ① 三组同模式 = cart(0x088fed17/0x088fed48)/gift/cerapoint 各
//     `delete 3 + replace 1↔5 + insert 4`，即 probe 的 ctor/D1Ev 叶块与
//     break 出口 eh state 跳板的排布（ORIG 单点内联 vs 4.4 汇合共享）= N121 jump-
//     threading + N82 帧槽；② `je/jne` 反转与 `xor $0x1,%eax; test` 对
//     `test; je` 的系统差（4.1 的 `!bool` 恒经 xor）= N18b/N209；③ 主链
//     `nop; jmp` 岛、section 比较链叶块排列 = N82/N89。**未发现可观察行为
//     差异**（无 REAL-BUG）：三处改写均为无依赖等价重排，(1) 的镜像视图
//     与原类型同字段序同尺寸（static_assert 锁死）。
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
                flag0 = 7;                             // ORIG 尾组经 -0x54 int 槽
                item.m_fieldc = 7;
                item.m_field1c = 1;
                item.m_field0 = flag0;
                // [BigRndOpt] ORIG 0x088fe0xx：`item.m_name = ""`（`movl $<A>,0x4(%esp)`
                // + `lea item; add $0x40,%eax; call _ZNSsaSEPKc`）在 ORIG 的物理位是
                // flag0 / m_fieldc / m_field1c / m_field0 四组赋值**之后**（语句书写序
                // = ORIG 叶块物理序）。两组写无数据依赖 ⇒ 纯书写序调整，行为不变。
                item.m_name = "";
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
                // [BigRndOpt·双判据 A/B] 曾把本声明上提至 ScanInt 之前（ORIG 的
                // ctor 物理位确在前：0x088fed48 `lea -0x180` / 0x088fed51 `call
                // _ZN14STCeraShopItemC1Ev` → 0x088fed5c `call _Z7ScanIntPb`），
                // 实测劣于现状 ⇒ 撤回：上提后 clean 1748（gap +9）、ndiff 1644、
                // 且 break 出口改为 4.4 的 `mov $state,%ebx; jmp 共享清理块` 跳板
                // （3×5 条），call 多重集两种写法都是 274/274 ⇒ ①变差 ②无改善。
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
                STCeraShopItem probe;   // [BigRndOpt] 上提已按双判据撤回，见 [cart disallowance]
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
                STCeraShopItem probe;   // [BigRndOpt] 上提已按双判据撤回，见 [cart disallowance]
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
                    // [BigRndOpt] 改用 trivial 镜像视图，见函数前判词
                    BonusItemView bi;
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
                    info.m_items.push_back(
                        *reinterpret_cast<const stCeraShopBonusItemInfo_t*>(&bi));
                }
                // [BigRndOpt] ORIG 0x088ff389..0x088ff3c1 是**单判**结构：
                //   `cmpl $0x3e8,-0x2c(%ebp)` / `je 0x088ff3b1`；fall-through 侧 printf
                //   后 `mov $0x0,%esi; mov $0x1,%ebx`（return false 的清理链），
                //   跳走侧 0x088ff3b1 才 `push_back(bonusList, info)`（0x088ff3c6 置
                //   state 2 → 0x088ff1f2 回 while 体首）。原「双判」写法在 -O0 复现
                //   两次 `cmpl $0x3e8`，且第二次 `je` 分支恒不成立（死分支）。
                //   两次判定条件同值 ⇒ 合并为单判不改变可观察行为。
                if (total != 1000)
                {
                    printf("bonus rate error %d", total);
                    return 0;
                }
                bonusList.push_back(info);
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
