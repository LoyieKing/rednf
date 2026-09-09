// ============================================================================
// df_game_r 还原 —— CUserGlobalInfoHandle（G2-4 批次，用户全局信息句柄）。
// 依据 docs/class_func_reports/CUserGlobalInfoHandle.md + ORIG 反汇编。
// 布局：+0x00 char m_used[0x10000] | +0x10000 int m_next（总尺寸 0x10004）。
// ============================================================================

#include <string.h>

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
// [N96 清绑 2026-09-09] 删除历史 `register bool failed __asm__("al")`（P0 限定②
// 与 N96 全域禁用；P248 尾条：asm 绑定任何情形不得恢复）。ORIG 0x086ad924..2d 为
// 4.1 的 bool 物化岛 `cmpl $0xffffffff,-0x4(%ebp); sete %al; test %al,%al; je`；
// failed 值全程在 %al、无栈槽 ⇒ 按 P0 双推律原码无该具名局部，还原为
// `if (idx == -1)` 表达式直判。
// 残差 = N96+N18a 封顶（4.4 对槽比零恒折直跳，不再物化 setCC，绑定外不可达）。
// 读数：清绑前 [identical=1 ae=5 合计6 near=0 diff=0]，本函数 verdict=DIFF
//       orig24/our24；清绑后 [identical=1 ae=4 合计5 near=0 diff=1]，
//       verdict=DIFF orig24/our22（缺 sete+test 两条）。
// ============================================================================
int CUserGlobalInfoHandle::get_uniqueid()
{
    int idx = find_uniqueid();
    // [N96 清绑] 原 `register bool failed __asm__("al")` 已删（P0/N96 全域禁绑）。
    // ORIG 0x086ad924 `cmpl $0xffffffff,-0x4(%ebp); sete %al; test %al,%al; je`
    // ——failed 值全程在 %al、无栈槽 ⇒ 按 P0 双推律原码无该具名局部，还原为
    // 表达式直判。4.4 自然形折成 `cmpl $-1,-0x4(%ebp); jne`（残差见函数头判词）。
    if (idx == -1)
    {
        m_next = 1;
        idx = find_uniqueid();
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
