int ExpertJobMaterial::IsRight()
{
    if (Empty())
        return 0x11;

    if (reinterpret_cast<CUser*>(m_user)->getCurCharacR() == 0)
        return 3;
    if (m_field0e != 0)
        return 0x11;

    Inven_Item* invenRef =
        reinterpret_cast<CUser*>(m_user)->GetCurCharacInventoryRef(m_field0e, m_slot);
    if (invenRef == 0)
        return 0x11;
    if (static_cast<unsigned int>(invenRef->m_addInfo) != m_itemIndex)
        return 0x11;
    return 0;
