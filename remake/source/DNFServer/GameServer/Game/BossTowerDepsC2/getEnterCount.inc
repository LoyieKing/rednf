// ============================================================================
// TOD_UserState::{getEnterCount,getLastClearTime} —— ORIG T 0x08643872/0x0864387e
// （两报告均为 2 条指令字段读；声明/字段见 DataManagerInitDepends.h）。
// ============================================================================
int TOD_UserState::getEnterCount() const
{
    return m_data.m_enterCount;      // ORIG：*(int*)(this+0x10)
}