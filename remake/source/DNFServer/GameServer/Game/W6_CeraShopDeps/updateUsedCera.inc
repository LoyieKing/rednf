// ORIG T @0x0831ce5e [updateUsedCera.md]：仅转发 DB_UpdateBreakAwayAccruedCera::
// makeRequest；不访问任何成员字段，调用视图本地布局即 ABI 安全。
void break_away_prevent::CBreakAwayPreventSystem::updateUsedCera(unsigned int accId,
