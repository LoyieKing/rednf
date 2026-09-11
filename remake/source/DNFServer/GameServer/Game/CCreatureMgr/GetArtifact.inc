// ORIG _ZN13user_creature12CCreatureMgr11GetArtifactEi T 0x833be56（GetArtifact.md）：
// idx=slot-0x17，仅上界检查（>3 → 0）；返回 &m_artifacts[idx]
// （ORIG this+(idx+8)*4 与镜像 m_artifacts 基址 +0x20 一致）。
CArtifact* CCreatureMgr::GetArtifact(int slot)
{
    // ORIG 0x833be56：slot-0x17 两次独立现算（无局部槽、无帧 pop 收尾）。
    if (slot - 0x17 > 3)
        return 0;
    return &m_artifacts[slot - 0x17];
}
