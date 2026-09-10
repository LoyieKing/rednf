void cUserHistoryLog::EndPvP(ENUM_PVP_HISTORY_REASON reason, bool flag, unsigned int v)
{
    // ORIG 0x08687f1x：bool 形参 spill 槽 movzbl 直接进 %d 实参（无三目岛）。
    m_user->LogHistory("PVP,%d,%d,%u", (int)reason, flag, v);
