bool CUserCharacInfo::FindItemLock(unsigned char b, ENUM_ITEMSPACE& space, int& slot,
                                   CUser* user) const
{
    if (user == 0)
        return false;
    if (m_selected == 0)
        return false;
    if (((const CCargo&)CUR->m_cargo).FindItemLock(b, space, slot))
        return true;
    if (((const CharacInvenView*)CUR)->m_inven.FindItemLock(b, space, slot))
        return true;
    CExpandEquipslot* expand = user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
    if (expand->FindItemLock(b, space, slot))
        return true;
    if (((CTitleBook*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)14))
            ->findItemLock(b, space, slot))
        return true;
    return false;
