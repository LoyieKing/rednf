void CParty::IncreaseSetPlayResultCount(CUser* user)
{
    int slot = GetMemberSlotNo(user);
    if (slot >= 0)
        m_member[slot].m_field4 = 1;
