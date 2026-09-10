void CParty::useItem(CUser* user, Inven_Item const& item)
{
    reinterpret_cast<CBattle_Field*>(&m_battleField)->onUseItem(user, item);
