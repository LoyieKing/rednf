// ============================================================================
// cUserHistoryLog::ExpandInventory @ 0x08683cbc
//   记录背包扩容日志 "ExpandInventory+,%d,%d,%d"（cap, newCap, a）。
//   经 CUser::LogHistory 输出（与其余日志方法同构）。
// ============================================================================
void cUserHistoryLog::ExpandInventory(int cap, int newCap, int a)
{
    m_user->LogHistory("ExpandInventory+,%d,%d,%d", cap, newCap, a);
}