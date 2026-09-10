// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ActiveStaticInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "ActiveStaticInfo.h"
#include <cstring>

// ActiveStaticInfo 实现（ORIG：ctor 0x8150ec6 / dtor 0x8150ef6 /
// clear 0x81509aa / getHpMax 0x85bedb0 / getMpMax 0x88ba4be；
// 实例计数与累计尺寸为 unmangled 全局符号）
int g_ActiveStaticInfoCount asm("g_ActiveStaticInfoCount") = 0;
int g_ActiveStaticInfoSize asm("g_ActiveStaticInfoSize") = 0;

#include "ActiveStaticInfo/ActiveStaticInfo.inc"
{
    ++g_ActiveStaticInfoCount;
    g_ActiveStaticInfoSize = g_ActiveStaticInfoCount * 0x2f0;
    clear();
}

#include "ActiveStaticInfo/dtor_ActiveStaticInfo.inc"
{
    --g_ActiveStaticInfoCount;
    g_ActiveStaticInfoSize = g_ActiveStaticInfoCount * 0x2f0;
}

#include "ActiveStaticInfo/clear.inc"
{
    // [A24TailBatch2] P238：ORIG 0x08150a.. 对 +0x180/+0x18c../+0x1d4/+0x2cc/+0x2d0 用 float
    // 常量 store 中转形 `mov $0x0,%edx; mov %edx,N(%eax)` ⇒ 这 14 个成员真型 float（头已改型，
    // 4 字节同宽、偏移/sizeof 断言不变）。
    // 循环槽（ORIG）：-0xc = loop1/loop2 共用（同一变量）、-0x8 = 18 次表循环、-0x4 = 尾 5 次循环。
    int i;
    m_level = 0;
    m_hpBase = 0;
    m_hpRatio = 1.0f;
    m_mpBase = 0;
    m_mpRatio = 1.0f;
    m_flag14 = 1;
    m_attack = 0;
    m_attackRatio = 1.0f;
    m_magicAttack = 0;
    m_magicAttackRatio = 1.0f;
    m_defense = 0;
    m_defenseRatio = 1.0f;
    m_magicDefense = 0;
    m_magicDefenseRatio = 1.0f;
    for (i = 0; i < 4; ++i)
    {
        m_elemAttack[i] = 0;
        m_elemAttackRatio[i] = 1.0f;
        m_elemDefense[i] = 0;
        m_elemDefenseRatio[i] = 1.0f;
    }
    for (i = 0; i < 17; ++i)
    {
        m_attr17Base[i] = 0;
        m_attr17Ratio[i] = 1.0f;
    }
    m_statBase0 = 0;
    m_statRatio0 = 1.0f;
    m_statBase1 = 0;
    m_statRatio1 = 1.0f;
    m_statBase2 = 0;
    m_statRatio2 = 1.0f;
    m_statBase3 = 1000;
    m_statRatio3 = 1.0f;
    m_statBase4 = 1000;
    m_statRatio4 = 1.0f;
    m_statBase5 = 1000;
    m_statRatio5 = 1.0f;
    m_statBase6 = 0;
    m_statRatio6 = 1.0f;
    m_statBase7 = 0;
    m_statRatio7 = 1.0f;
    m_statBase8 = 0;
    m_statRatio8 = 1.0f;
    m_statBase9 = 100;
    m_statRatio9 = 1.0f;
    m_statBase10 = 0;
    m_statRatio10 = 1.0f;
    m_statBase11 = 0;
    m_statRatio11 = 1.0f;
    m_statBase12 = 0;
    m_statRatio12 = 1.0f;
    m_statBase13 = 0;
    m_statRatio13 = 1.0f;
    m_statBase14 = 0;
    m_statRatio14 = 1.0f;
    m_statBase15 = 0;
    m_statBase16 = 0;
    m_statBase17 = 0.0f;
    m_statBase18 = 0;
    m_statBase19 = 0;
    m_statBase20 = 0.0f;
    m_statBase21 = 0.0f;
    m_statBase22 = 0.0f;
    m_statBase23 = 0.0f;
    m_statBase24 = 0.0f;
    m_statBase25 = 0.0f;
    m_statBase26 = 0.0f;
    m_statBase27 = 0.0f;   // +0x1a8 float
    m_statBase28 = 0.0f;
    m_statBase29 = 0.0f;
    m_statBase30 = 0.0f;
    m_statRatio15 = 1.0f;
    m_statBase31 = 0;
    m_statBase32 = 0;
    m_statBase33 = 0;
    m_statBase34 = 0;
    m_statRatio16 = 1.0f;
    m_statBase35 = 0;
    m_statBase36 = 0.0f;
    m_statRatio17 = 1.0f;
    m_statBase37 = 0;
    for (int k = 0; k < 18; ++k)
    {
        m_table1[k] = 0;
        m_table2[k] = 0;
        m_table3[k] = 0;
    }
    // [A24TailBatch2] ORIG 0x08150e1d 尾循环（i<=4，@+0x2b8 共 5 项）此前缺失 ⇒ 补回。
    for (int j = 0; j <= 4; ++j)
        m_field2b8[j] = 0;
    m_f2cc = 0.0f;
    m_f2d0 = 0.0f;
    m_f2d4 = 0;
    m_f2d8 = 0;
    m_f2dc = 1.0f;
    m_f2e0 = 1.0f;
    m_f2e4 = 0;
    m_f2e8 = 0;
    m_f2ec = 1.0f;
}

#include "ActiveStaticInfo/getHpMax.inc"
{
    return static_cast<int>(static_cast<double>(m_hpBase) *
                            static_cast<double>(m_hpRatio));
}

#include "ActiveStaticInfo/getMpMax.inc"
{
    return static_cast<int>(static_cast<double>(m_mpBase) *
                            static_cast<double>(m_mpRatio));
}

