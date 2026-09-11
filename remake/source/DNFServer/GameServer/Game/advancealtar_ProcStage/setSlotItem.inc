// ORIG T 0x0812d246：m_user 非空时经其 CharacAdvanceAltarManager（CUser +0x8df60）
//   getSlotItem 同步 m_slotItems（map<uchar,_SlotItem>）。
void CharacData::setSlotItem()
{
    if (m_user == 0)
        goto L_done;
    m_user->m_advanceAltar.getSlotItem(m_slotItems);
L_done:;
}