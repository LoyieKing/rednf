// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUserGlobalInfoHandle/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string.h>
#include <iostream>   // [P338] ORIG 本 TU 有 __ioinit（static_init 0x086ad978 调
                      // _ZNSt8ios_base4InitC1Ev @0x94739fe）+ _GLOBAL__I_...C2Ev
                      // @0x086ad9b8（体 sub $0x18 标准形）；缺此 include 则
                      // 4.4 不发射 GI/static_init ⇒ missing_in_game=2。

#include "CUserGlobalInfoHandle.h"

#include "CUserGlobalInfoHandle/CUserGlobalInfoHandle.inc"
{
    m_next = 1;
    for (int i = 0; i <= 0xfffe; ++i)
    {
        m_used[i] = 0;
    }
}

#include "CUserGlobalInfoHandle/dtor_CUserGlobalInfoHandle.inc"

#include "CUserGlobalInfoHandle/find_uniqueid.inc"
{
    int idx = m_next;
    while (idx <= 0xfffe)
    {
        if (!m_used[idx])
        {
            m_used[idx] = 1;
            return idx;
        }
        ++idx;
    }
    return -1;
}

// ============================================================================
// CUserGlobalInfoHandle::get_uniqueid —— ORIG 0x086ad910 T size 0x4e
// 报告：docs/class_func_reports/CUserGlobalInfoHandle/get_uniqueid.md（## 汇编 +
// ## 反编译 C 两段已完整阅读；Ghidra C 的 local_8 == 我方 idx）。
// [P225 2026-09-09] ORIG 0x086ad924..2d 为 4.1 的 bool 物化岛
// `cmpl $0xffffffff,-0x4(%ebp); sete %al; test %al,%al; je`；4.4 对裸 `if` 恒折
// `cmpl $-1,-0x4(%ebp); jne` 直跳（少 sete+test 两条，DIFF orig24/our22）。按
// P225（CVillageAttacked/CDropItem 实证）改写 `while (idx == -1) { …; break; }`
// 后 setCC 四段逐条复现；残留 1 条 nop（4.4 循环出口对齐，§6.1/N112 剥离口径）
// ⇒ verdict=AE orig24/our24。
// 历史：[N96 清绑] 已删 `register bool failed __asm__("al")`（P0 限定②/N96 全域禁绑，
// P248 尾条：任何情形不得恢复）。
// 读数：清绑后 if 形 TU [identical=1 ae=4 合计5 near=0 diff=1 missing=2]；本形
//       [identical=1 ae=6 合计7 near=0 diff=1 missing=2]，其中 diff=1 为 N123 撞名
//       `_Z41__static_initialization_and_destruction_0ii`（本 TU 真副本 0x086ad978
//       16 条助记符序列 16=16 全等），missing=2 为 P93 同址别名 C2/D2。
// [N6S2NegB 2026-09-11 复核] ORIG 0x086ad94a `je` 目标前 / 函数尾各有 1 条对齐 nop，
// 我方物理 25 行 vs ORIG 24 ⇒ 归一口径（compare_common.strip_nop_padding）已剥 ⇒
// n1_v verdict=AE orig=24 our=24。消 nop 穷举（/tmp/probe_gid/p4..p12 + 446/444 后端）：
// 裸 if=p5 `cmpl;jne`、具名 bool=p4/p7（多 -0x9 槽 + cmpb）、while+break=p6、while+goto=p11、
// do-while=p12、while(true)+if(!=)break=p8（jne+回跳）——**全部复现该 nop**，无一形产出
// 无 nop 的 `sete/test/je`；该 nop 为 4.4 对物化条件的固有产物，源码不可消。
// 结论：保持 while+break 形（唯一复现 ORIG 0x086ad924..2d 四段的形），本件已到 .o 口径上限。
// ============================================================================
#include "CUserGlobalInfoHandle/get_uniqueid.inc"
{
    int idx = find_uniqueid();
    // [P225 while+break] ORIG 0x086ad924 `cmpl $0xffffffff,-0x4(%ebp); sete %al;
    // test %al,%al; je` 是 4.1 的 bool 物化后条件分支；4.4 对裸 `if` 恒折
    // `cmpl $-1,-0x4(%ebp); jne` 直跳。按 P225（CVillageAttacked/CDropItem 实证）
    // 改用 `while (cond) { body; break; }` 精确复现 setCC+test+je 四段物化。
    while (idx == -1)
    {
        m_next = 1;
        idx = find_uniqueid();
        break;
    }
    m_next = idx + 1;
    return idx;
}

#include "CUserGlobalInfoHandle/reset_uniqueid_flag.inc"
{
    m_used[uniqueid] = 0;
}

CUserGlobalInfoHandle* CUserGlobalInfoHandleInstance()
{
    static CUserGlobalInfoHandle instance;
    return &instance;
}
