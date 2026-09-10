int CBossTower::handleUseItem(CUser* user, unsigned long itemId, unsigned char param)
{
    const int slot = static_cast<int>(m_party->GetMemberSlotNo(user));
    if (slot == -1)
        return 0;

    if (itemId == 0xbdd)
    {
        ++m_useItemCount[slot];
    }
    if (param == 2)
    {
        ++m_memberField[slot];
    }
    return 1;
