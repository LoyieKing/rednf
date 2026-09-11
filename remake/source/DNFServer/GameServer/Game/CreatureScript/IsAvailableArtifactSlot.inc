// ORIG 0x833cb38：slot-0x17 在 [0,4) 内返回 m_artifactSlotFlags[slot-0x17]，否则 0。
// 返回 bool + 字段 bool*：callee 尾 movzbl 直落 + caller 0x833771d call 后
// 无物化 = bool→bool 透传（int 返回在 4.4 caller 必插 test+setne）。
bool CCreatureScript::IsAvailableArtifactSlot(int slot)
{
    // ORIG 0x833cb38：仅上界 cmp $3;jle（无下界/判空，负 idx 直读为 ORIG 实况）；
    // slot-0x17 两次现算，无局部槽，pop 收尾。
    if (slot - 0x17 > 3)
        return false;
    return m_artifactSlotFlags[slot - 0x17];
}