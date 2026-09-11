void CExpandEquipslot::_reset()
{
    for (int i = 0; i < 0xc; ++i)
    {
        m_equipSlot[i].reset();
        m_expandEquipSlot[i].reset();
        m_expandAvatarSlot[i].reset();
    }
}