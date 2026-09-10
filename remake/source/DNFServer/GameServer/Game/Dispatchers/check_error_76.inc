int Dispatcher_GrowthWeaponEventChangeInfinity::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    MSG_GROWTH_EQUIPMENT& req = reinterpret_cast<MSG_GROWTH_EQUIPMENT&>(msg);
    if (user->CheckItemLock(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)req.m_0d), req.m_11))
    {
        return 0xd5;
    }
    if (*reinterpret_cast<int*>(
            static_cast<unsigned char*>(user->getGrowthWeaponEventdata()) + 0x04) !=
        user->get_charac_no(-1))                                    // P48 无 CSE：每字段重调
    {
        return 3;
    }
    if (*reinterpret_cast<int*>(
            static_cast<unsigned char*>(user->getGrowthWeaponEventdata()) + 0x0c) != 0)
    {
        return 3;
    }
    int serverGroup;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverGroup = (int)user->GetServerGroup();
    }
    else
    {
        // ORIG *(int*)(G_CEnvironment()+0x378)（SPremiumInfo.h:33 同址证据：目标服务器代码）
        serverGroup = *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x378);
    }
    if ((int)*reinterpret_cast<short*>(
            static_cast<unsigned char*>(user->getGrowthWeaponEventdata()) + 0x12) != serverGroup)
    {
        return 3;
    }
    if (*reinterpret_cast<short*>(
            static_cast<unsigned char*>(user->getGrowthWeaponEventdata()) + 0x10)
        < (short)G_CDataManager()->get_GrowthWeaponMaterialMax())
    {
        return 0x11;
    }
    CEventBase* repeatEvent = GlobalData::s_event_manager->GetRepeatEvent(0x64);
    // ORIG 无判空直接虚调 vtable+0x34（slot IsEventing）传 0。
    if (repeatEvent->IsEventing(0))
    {
        return 0;
    }
    return 1;
