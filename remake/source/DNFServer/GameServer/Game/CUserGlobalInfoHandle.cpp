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

int CUserGlobalInfoHandle::get_uniqueid()
{
    int idx = find_uniqueid();
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
