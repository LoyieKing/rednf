// ORIG T 0x868455a（报告全文）："Private Store+,%d,%d"
void cUserHistoryLog::CreatePrivateStore(int zoneA, int zoneB, char const* name)
{
    // ORIG vararg 只压 zoneA+zoneB 两个实参（0xc/0x10(%esp)），format "%d,%d"，
    // 不传 name；多余 name 实参是 ORIG 未有的（移除以匹配 0x18 帧）。
    m_user->LogHistory("Private Store+,%d,%d", zoneA, zoneB);
}