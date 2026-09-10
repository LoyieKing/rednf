void CRequestDisjointCmd::execute()
{
    register char const* name = m_str0.c_str();   // ORIG ebx 缓存跨 G_GameWorld
    CUser* target = G_GameWorld()->find_user_by_charac_name(name);
    if (target != 0)
    {
        int type = 9;
        DisPatcher_DisJointItem::_disjoint(GetUser(), type, 0,
                                           static_cast<ENUM_CMDPACKET>(0x1c),
                                           target, 0xffff);
    }
    else
    {
        return;
    }
