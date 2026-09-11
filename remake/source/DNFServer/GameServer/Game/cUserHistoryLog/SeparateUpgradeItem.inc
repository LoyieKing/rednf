// ============================================================================
// cUserHistoryLog::SeparateUpgradeItem @ 0x8684d0a
//   记录分离强化日志 "UpgradeSeparate+,%d,%d,%d" / "-..."。
//   [L3Free25to36.L3AUserHistoryAradScript 2026-09-11 §6.5] 原写 "SeparateUpgrade±" 系单词序
//   颠倒（该串在 ORIG 全库 0 命中）。直证：ORIG 0x08684d36/0x08684d62 两处
//   `movl $0x8cf0830/$0x8cf084a,0x4(%esp); call CUser::LogHistory`，两地址 .rodata
//   逐字节 = `UpgradeSeparate+,%d,%d,%d` / `UpgradeSeparate-,%d,%d,%d`（objdump -s 实证）。
// ============================================================================
void cUserHistoryLog::SeparateUpgradeItem(int itemIdx, int from, int to,
                                          bool isUpgrade)
{
    if (isUpgrade)
        m_user->LogHistory("UpgradeSeparate+,%d,%d,%d", itemIdx, from, to);
    else
        m_user->LogHistory("UpgradeSeparate-,%d,%d,%d", itemIdx, from, to);
}