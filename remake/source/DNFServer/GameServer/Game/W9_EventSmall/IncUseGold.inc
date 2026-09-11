void CCharacBloodDungeon::IncUseGold(int gold)       // ORIG 0x85bfef6 (W)
{
    // b28d-lane6 修复（AE）：同 IncPoint，ORIG 0x85bfeff 段（字段 unsigned 旋钮）。
    charac_expand::CData::alter();
    m_useGold += gold;
}