bool CCreatureScript::IsAvailableArtifactSlot(int slot)
{
    // ORIG 0x833cb38：仅上界 cmp $3;jle（无下界/判空，负 idx 直读为 ORIG 实况）；
    // slot-0x17 两次现算，无局部槽，pop 收尾。
    if (slot - 0x17 > 3)
        return false;
    return m_artifactSlotFlags[slot - 0x17];
