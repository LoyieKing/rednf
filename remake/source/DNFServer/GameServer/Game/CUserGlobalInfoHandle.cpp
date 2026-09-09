// ============================================================================
// df_game_r 还原 —— CUserGlobalInfoHandle（G2-4 批次，用户全局信息句柄）。
// 依据 docs/class_func_reports/CUserGlobalInfoHandle.md + ORIG 反汇编。
// 布局：+0x00 bool m_used[0x10000] | +0x10000 int m_next（总尺寸 0x10004）。
// ORIG 本 TU 符号区间 0x086ad820..0x086ad9d3（含 __ioinit/static_init/GI，见下方
// include 注释）；ORIG 区间内 0 个 `call *`/`jmp *` 间接调用 ⇒ 无虚槽核对项。
// 四数（check_tu_routed fresh）：identical=1 ae=6 合计7 near=0 diff=1 missing=2；
// diff=1=N123 撞名 static_init（真副本 16=16 全等），missing=2=P93 C2/D2 同址别名。
// ============================================================================

#include <string.h>
#include <iostream>   // [P338] ORIG 本 TU 有 __ioinit（static_init 0x086ad978 调
                      // _ZNSt8ios_base4InitC1Ev @0x94739fe）+ _GLOBAL__I_...C2Ev
                      // @0x086ad9b8（体 sub $0x18 标准形）；缺此 include 则
                      // 4.4 不发射 GI/static_init ⇒ missing_in_game=2。

#include "CUserGlobalInfoHandle.h"

CUserGlobalInfoHandle::CUserGlobalInfoHandle()
{
    m_next = 1;
    for (int i = 0; i <= 0xfffe; ++i)
    {
        m_used[i] = 0;
    }
}

CUserGlobalInfoHandle::~CUserGlobalInfoHandle() {}

int CUserGlobalInfoHandle::find_uniqueid()
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
// ============================================================================
int CUserGlobalInfoHandle::get_uniqueid()
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

void CUserGlobalInfoHandle::reset_uniqueid_flag(unsigned short uniqueid)
{
    m_used[uniqueid] = 0;
}

CUserGlobalInfoHandle* CUserGlobalInfoHandleInstance()
{
    static CUserGlobalInfoHandle instance;
    return &instance;
}
