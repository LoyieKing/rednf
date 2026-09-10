int Inter_LoadCerashopAddRestrictType::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0 || data == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x200, 5)("[%s] pUser or p is null.",
              __PRETTY_FUNCTION__);
        return 0x201;
    }

    Inter_LoadCerashopAddRestrictTypePktView* pPkt = reinterpret_cast<Inter_LoadCerashopAddRestrictTypePktView*>(data);

    if (data == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x207, 5)("[%s] internal data is null.",
              __PRETTY_FUNCTION__);
        return 0x208;
    }

    CerashopAddRestrict::UserInfo& info = user->m_ceraUserInfo;
    const int type = pPkt->m_4;
    if (type == 0)
    {
        CerashopAddRestrict::Manager::LoadProcess(
            info.m_field30, info.m_accountDaily,
            reinterpret_cast<SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE*>(data));
    }
    else if (type == 1)
    {
        CerashopAddRestrict::Manager::LoadProcess(
            info.m_field48, info.m_characDaily,
            reinterpret_cast<SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE*>(data));
    }
    CerashopAddRestrict::Manager::SendSyncPacket(user);
    return 0;
