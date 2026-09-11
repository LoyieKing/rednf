// ---- 六个公开谓词 -----------------------------------------------------------
// Check/Register{BossKill,NamedKill,APCBossKill} 分别转发至 +0x8/+0x20/+0x38
// 槽 map；Register 的 APC 标志按 ORIG：Boss=false、Named=false、APCBoss=true
// （见各报告反编译 C）。槽地址由 this 直接偏移（与 ORIG add $0x8/0x20/0x38 同形）。
bool CKillMonsterInfo::CheckBossKill(unsigned int mobIdx, unsigned char diff)
{
    return CheckMonsterKill(m_bossKills, mobIdx, diff);
}