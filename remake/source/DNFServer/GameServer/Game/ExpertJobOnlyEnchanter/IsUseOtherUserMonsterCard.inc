// ---------------------------------------------------------------------------
// IsUseOtherUseMonsterCard（ORIG W 0x849f324 sz 0x27）【objdump】：
// 读 m_peerRequestType(+0x44)，==7 或 ==8 时返真。
// ---------------------------------------------------------------------------
bool CEnchanter::IsUseOtherUserMonsterCard()
{
    return (m_peerRequestType == 7) || (m_peerRequestType == 8);
}