int Dispatcher_GrowthWeaponEventUseMaterial::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return 0xffffffff;
    }
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    const int slotIdx =
        *reinterpret_cast<const int*>(reinterpret_cast<const char*>(&msg) + 0x11);
    const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(1, slotIdx);
    if (item->m_addInfo == 0)
    {
        return 0x15;
    }
    if (item->m_addInfo != G_CDataManager()->get_GrowthWeaponMaterialItem())
    {
        return 0x11;
    }
    if (*reinterpret_cast<int*>(
            static_cast<char*>(user->getGrowthWeaponEventdata()) + 4) !=
        user->get_charac_no(-1))                                    // P48 无 CSE：每字段重调
    {
        return 3;
    }
    int serverGroup;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() == 0)
    {
        serverGroup =
            *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x378);
    }
    else
    {
        serverGroup = user->GetServerGroup();
    }
    if (static_cast<signed short>(*reinterpret_cast<unsigned short*>(
            static_cast<char*>(user->getGrowthWeaponEventdata()) + 0x12)) !=
        serverGroup)
    {
        return 3;
    }
    if (static_cast<signed short>(*reinterpret_cast<unsigned short*>(
            static_cast<char*>(user->getGrowthWeaponEventdata()) + 0x10)) >=
        G_CDataManager()->get_GrowthWeaponMaterialMax())
    {
        return 0x11;
    }
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x64);
    if (!event->IsEventing(0))
    {
        return 1;
    }
    return 0;
