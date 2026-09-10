CArtifact* CCreatureMgr::GetArtifact(int slot)
{
    // ORIG 0x833be56：slot-0x17 两次独立现算（无局部槽、无帧 pop 收尾）。
    if (slot - 0x17 > 3)
        return 0;
    return &m_artifacts[slot - 0x17];
