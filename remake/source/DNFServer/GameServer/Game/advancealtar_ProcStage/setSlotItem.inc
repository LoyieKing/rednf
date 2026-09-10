void CharacData::setSlotItem()
{
    if (m_user == 0)
        goto L_done;
    m_user->m_advanceAltar.getSlotItem(m_slotItems);
L_done:;
