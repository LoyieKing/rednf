// CParty::IncreaseSetPlayResultCount —— ORIG T 0x085b22e2。
// P210：定义置于 callee GetMemberSlotNo 之后，4.4 精确继承 outgoing（帧 0x28→0x18=ORIG）。
void CParty::IncreaseSetPlayResultCount(CUser* user)
{
    int slot = GetMemberSlotNo(user);
    if (slot >= 0)
        m_member[slot].m_field4 = 1;
}