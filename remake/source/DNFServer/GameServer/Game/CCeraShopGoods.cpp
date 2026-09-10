// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCeraShopGoods/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "CCeraShopGoods/STCeraShopItem.inc"

// ---- clear @ 0x88fd9c4 ----
// [b28d-35] 翻正（推翻原 N18a 判词）：m_field44 写 ORIG 为 `mov $0x0,%edx;
// mov %edx,0x44(%eax)` 两段（4.1 常数寄存器中转）——按 P238 用 float 视图
// `m_field44f = 0.0f;` 即复现该 edx 中转形（int 视图恒折 `movl $0x0,
// 0x44(%eax)` 单段）。字段经匿名 union 双视图，sizeof/偏移/mangled 不变。
#include "CCeraShopGoods/clear.inc"

// ---- operator== @ 0x891a89e ----
bool STCeraShopItem::operator==(const STCeraShopItem& other)
{
    // ORIG 0x0891a8a1：先载 other（0xc(%ebp)）后载 this，cmp %eax,%edx。
    if (other.m_field8 == m_field8)
        return true;
    return false;
}

// ---- 访问器 ----
#include "CCeraShopGoods/getCeraPrice.inc"

#include "CCeraShopGoods/getGoldPrice.inc"

#include "CCeraShopGoods/isLimitSaleItem.inc"

// ============================================================================
// CCeraShopGoods
// ============================================================================

// ---- SetItem @ 0x8510c1a ----
// [b28d-solo-CCeraShopGoods] AE：m_itemIdx 读 ORIG 0x08510c24 为
// `mov 0x14(%eax),%eax; mov %eax,%edx` 两段物化 ⇒ 按 P254 把
// STCeraShopItem::m_field14 改为 ENUM_CERA_SHOP_ITEM_NO（enum 源字段→int
// 目标保留 eax→edx 中转；int 字段 4.4 恒折单段）。原 N18a 判词作废
// （P248 档③类型旋钮）。sizeof/偏移/mangled 不变。
#include "CCeraShopGoods/SetItem.inc"

// ---- C1 @ 0x8514ea0 ----
#include "CCeraShopGoods/CCeraShopGoods.inc"

// ---- weak 访问器 ----
#include "CCeraShopGoods/GetItemIdx.inc"

#include "CCeraShopGoods/GetStackCount.inc"

#include "CCeraShopGoods/GetGoldPrice.inc"

#include "CCeraShopGoods/GetWinPoint.inc"

#include "CCeraShopGoods/GetCeraPrice.inc"

#include "CCeraShopGoods/GetMileagePrice.inc"

#include "CCeraShopGoods/GetSubGroupIndex.inc"

#include "CCeraShopGoods/GetGroupIdx.inc"

#include "CCeraShopGoods/GetFeaturedIdx.inc"

#include "CCeraShopGoods/GetSticker.inc"

#include "CCeraShopGoods/GetExpiryDate.inc"

#include "CCeraShopGoods/GetBonusWeight.inc"

#include "CCeraShopGoods/GetRelatedItem.inc"

#include "CCeraShopGoods/isGiftDisallowance_.inc"

#include "CCeraShopGoods/isCeraPointDisallowance_.inc"

#include "CCeraShopGoods/isStackBuyable.inc"

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
//     差异（无 REAL-BUG）：三处改写均为无依赖等价重排，(1) 的镜像视图
//     与原类型同字段序同尺寸（static_assert 锁死）。
// [SweepSpecialItem 本波] dual_rank 候选 2 件复核：
//  ① 本函数：probe 声明位置修正 —— ORIG 0x088fed48 C1Ev 在循环**外**
//     （回边 0x088fee67 `jmp 0x088fed56`=ScanInt 不重经 ctor；break 出口
//     0x088fee6c 之后 0x088fee6d 才 D1Ev；Ghidra C 亦为 ctor 在 while 前、
//     dtor 在 while 后）。改后三段 [cart]/[gift]/[cerapoint disallowance] 的
//     call 序列与段指令数**完全对齐**（段差 -7/+5/-1/+2 四组×3 全归零；
//     CALLMISM 9→3，余 3 = printf@plt 归一 + const_iterator C1/C2 同址别名）。
//     读数：clean 1733→**1730** vs ORIG 1739、raw 1760→**1757** vs 1768；
//     差值全部落在 probe 段之外（改前其它段净差即 9，probe 段净多 3 属假抵消）
//     ⇒ 本改正是结构归位，余 9 为 N82/N121/N89 形态族。
//     p167：274/274；TU 四数 140/279/near3/diff4/missing95 **逐位不变**
//     （check_tu_routed COMPILE OK）。
//  ② new_allocator<stCeraShopBonusItemInfo_t>::construct<T>(T*, T&&)（右值版
//     0x089361e6 size 0x4e）：ORIG 27 vs 我方 25。ORIG 用 3 个栈槽保存
//     forward 结果（-0x14/-0x10/-0xc）再拷进 placement-new 目标，4.4 用
//     callee-saved %ebx 直存（push ebx/sub $0x14 vs sub $0x28）。模板体在
//     <memory>（libstdc++ 头，禁改），且 ORIG 该右值重载真实存在（改传左值
//     会丢符号）⇒ 跨版本 codegen，N89/N2 封顶，无语义差。
#include "CCeraShopGoods/importCashShopItemList.inc"
