// ----------------------------------------------------------------------------
// ORIG T 0x081454b8 _ZN8WongWork10CBossTower13handleUseItemEP5CUsermh：
// GetMemberSlotNo==-1 → 0；itemId==0xbdd → m_useItemCount[slot]++（+0x9c8 区）；
// param==2 → m_memberField[slot]++（+0x9d8）；返回 1。
// ----------------------------------------------------------------------------
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
}