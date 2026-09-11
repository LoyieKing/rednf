// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RandomOptionScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "RandomOptionScript.h"

#include <string.h>

#include "DNFLexWrapper.h"
#include "CCubeStatistic.h"   // getItemGroupNameTable()
#include "LogManager.h"       // logFormat（ORIG duplicate-key 报错）

// ===================== importRandomOptionScript =====================
// ORIG 0x08a70a90（0x2e10 字节脚本解析器，2754 条反汇编逐段核对）。
//
// 【batch-14 收敛】we-bigger 221 → 1（ORIG 2754 / ours 2755），P167 调用点差集
// 双向 0（361/361 逐符号 Counter 全等）。削减手段两类：
//  ① 键拼装改「扫描即直存键槽」：4 处 map 键（[option quantity] int 键 -0x4a4、
//     [choose option group]/[modified option selection] u64 键 -0x4b0/-0x4e0、
//     [break seal cost] int 键 -0x588、[different weight] short 键 -0x592）原用
//     移位或链拼装在 -O0 每站 40-49 insns，ORIG 实为逐 byte/word/dword 直存
//     （`mov %al,-0x4b0` / `mov %eax,-0x4ac` / `mov %ax,-0x586` …）⇒ 用文件内
//     anonymous-namespace union 视图（P180，纯类型零符号 + static_assert 尺寸）复现；
//  ② 44 处 `v = ScanInt(&ok); if(!ok)…; dst = (T)v;` 的共享 v 中转改写为
//     `dst = (T)ScanInt(&ok); if(!ok)…`（P183 物化口径：ORIG 每次 Scan 结果
//     `call 后紧接 mov %eax/%ax/%al,目的槽`，随后才 movzbl ok 检查），每站 −2 insns；
//     同口径连带消掉 [part type] itemGroup/weight 与 [option group] weight 中转量。
//  ③ 顺带修正两处语义缺失：[part type] 命中支缺 `field.m_weight = it->second.m_fieldC`
//     累计回写（ORIG 313-317 第 4 次 pt，原缺 1 条 operator-> 调用）；
//     [part type]/[option group] 第二 vector 实收累计权重而非原始 weight。
// 残余 DIFF 归因：N82/P112 帧槽号（4.1 vs 4.4 分配序）+ .o 未 relink 的
// R_386_PC32/R_386_32 显示差（AE 口径链接后核销）；本函数另有 3 站 std::string
// 临时量与 map 迭代器具名实例化产生 C2/D2 别名拼写差（ORIG nm 双侧实存）。
// 同 TU：getPrefix 0x8a73956 已按 ORIG 形重写（第三参拷本地 byte 槽、表指针
// 先无条件取 m_prefixNormal 再条件覆盖、iterator 非 const）→ 99/99 gap 0。
// ============================================================================
// 主循环 ScanType 按段名分发，各段把脚本数值填入对应成员表；
// 返回出错行号（成功 0）——与调用方 CDataManager::init
// `const int err = importRandomOptionScript(...); if (err != 0)` 消费一致。
// ORIG 无 script.clear()（调用方 CDataManager 于加载前已 clear）。
//
// 段名 → 目标表（+ 成员偏移）：
//   [random application]        → m_map0(+0x00)
//   [level limit]               → m_map18(+0x18)
//   [part type]                 → m_map30(+0x30)
//   [base item]                 → m_map48(+0x48)
//   [option type]               → m_map90(+0x90)
//   [quantity ratio]            → m_veca8(+0xa8)
//   [option quantity]           → m_mapb4(+0xb4)
//   [choose option group]       → m_mapcc(+0xcc)
//   [modified option selection] → m_map130(+0x130)
//   [modified option type]      → m_map118(+0x118)
//   [option modification]       → m_costVec10c(+0x10c)
//   [option value ratio]        → m_ratioE4(+0xe4)
//   [option group]              → m_map160(+0x160)
//   [postfix]                   → m_prefixNormal(+0x1a8)/m_prefixPremium(+0x1c0)
//   [choose postfix]            → m_choosePrefix1d8(+0x1d8)
//   [break seal cost]           → m_map178(+0x178)
//   [option text]               → 丢弃（ORIG 仅 ScanInt+ScanStr 消费行）
//   [different weight]          → m_mapf4(+0xf4)
//   [changed postfix]           → m_str1e4(+0x1e4)
//   [postfix grade modification]→ m_map1e8(+0x1e8)
//   [unable to modify postfix grade] → m_set200(+0x200)
//   [gradeless postfix]         → m_set218(+0x218)

// ============================================================================
// 键拼装视图（batch-10 P167 反向差集复核）：ORIG 侧这些 map 键不是「先存若干
// 临时量再移位或拼」，而是**每次 ScanInt 后直接把字节写进键对象自己的槽**，
// 最后以 u64/int 视图作 make_pair 实参：
//   [option quantity]        int  键 -0x4a4：`mov %al,-0x4a4/-0x4a3/-0x4a2/-0x4a1`
//     （报告 ORIG +0xECA..+0xF05；值侧 `mov %ax,-0x4a8/-0x4a6` = m_minA/m_maxB 直存）
//   [choose option group]    u64  键 -0x4b0：`mov %al,-0x4b0/-0x4af/-0x4ae`、
//     `mov %eax,-0x4ac`（高 32）、`mov %al,-0x4ad`（+0x1393.. 区段），
//     make_pair 实参型 `Ry`（unsigned long long&）⇒ 同对象 u64 视图
//   [modified option selection] u64 键 -0x4e0：同形（-0x4e0/-0x4df/-0x4de/
//     -0x4dc(dword)/-0x4dd）
//   [break seal cost]        int  键 -0x588：`mov %al,-0x588`、`mov %ax,-0x586`、
//     `mov %al,-0x587`（+2 word / +1 byte 混序，ORIG 逐条照抄）
// 故用 union 视图复现「具名成员 + 常量偏移直存」形（范式库 P180）；移位或链在
// -O0 会多出 40+ insns/站（本函数 we-bigger 221 的主因）。
// ============================================================================

namespace
{

union OptionQuantityKeyView
{
    int m_key;                      // map<int, OptionQuantityValue> 键
    struct
    {
        unsigned char m_b0;         // +0x00（`mov %al,-0x4a4`）
        unsigned char m_b1;         // +0x01
        unsigned char m_b2;         // +0x02
        unsigned char m_b3;         // +0x03
    } m_bytes;
};
static_assert(sizeof(OptionQuantityKeyView) == 4, "ORIG int 键 4 字节");

union OptionGroupChoiceKeyView
{
    unsigned long long m_key;       // map<u64, OptionGroupChoiceValue> 键
    struct
    {
        unsigned char m_b0;         // +0x00
        unsigned char m_b1;         // +0x01
        unsigned char m_b2;         // +0x02（getItemGroupNameTable 值）
        unsigned char m_b3;         // +0x03
        unsigned int m_hi;          // +0x04（`mov %eax,-0x4ac`）
    } m_bytes;
};
static_assert(sizeof(OptionGroupChoiceKeyView) == 8, "ORIG u64 键 8 字节");

union BreakSealCostKeyView
{
    int m_key;
    struct
    {
        unsigned char m_b0;         // +0x00（`mov %al,-0x588`）
        unsigned char m_b1;         // +0x01（`mov %al,-0x587`）
        unsigned short m_w2;        // +0x02（`mov %ax,-0x586`）
    } m_bytes;
};
static_assert(sizeof(BreakSealCostKeyView) == 4, "ORIG break seal int 键 4 字节");

// [different weight] 键 = short（ORIG 槽 -0x592/-0x591 两字节直存，
// make_pair 实参型 `Rs` = short&）：`mov %al,-0x592` / `mov %al,-0x591`。
union DifferentWeightKeyView
{
    short m_key;
    struct
    {
        unsigned char m_b0;         // +0x00
        unsigned char m_b1;         // +0x01
    } m_bytes;
};
static_assert(sizeof(DifferentWeightKeyView) == 2, "ORIG different weight short 键 2 字节");

}   // namespace

#include "RandomOptionScript/importRandomOptionScript.inc"


#include "RandomOptionScript/clear.inc"


#include "RandomOptionScript/RandomOptionGive.inc"


#include "RandomOptionScript/RandomOptionGive_1.inc"


#include "RandomOptionScript/dtor_RandomOptionGive.inc"


#include "RandomOptionScript/clear_1.inc"


#include "RandomOptionScript/RandomOptionScript.inc"


#include "RandomOptionScript/dtor_RandomOptionScript.inc"


#include "RandomOptionScript/clear_2.inc"


#include "RandomOptionScript/getPrefixGrade.inc"


#include "RandomOptionScript/getPrefix.inc"


#include "RandomOptionScript/getOptionModificationCost.inc"

