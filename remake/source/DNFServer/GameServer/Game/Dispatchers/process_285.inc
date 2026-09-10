int Dispatcher_GrowthWeaponEventUseMaterial::process(CUser* user, MSG_BASE& msgBase, ParamBase& paramBase)
{
    // ORIG 0x081e81a4：消耗成长武器活动材料，累加怀表计数并发 DB 更新。
    MSG_GROWTH_EQUIPMENT& msg = *reinterpret_cast<MSG_GROWTH_EQUIPMENT*>(&msgBase);
    stGrowthEquipment& param = *reinterpret_cast<stGrowthEquipment*>(&paramBase);
    param.m_4 = check_error(user, msgBase);
    if (param.m_4 >= 1)
    {
        return 0;
    }
    if (param.m_4 < 0)
    {
        return LineFunc(0x647c, __PRETTY_FUNCTION__, param.m_4, 0);
    }
    CInventory* inven = user->getCurCharacInvenW();
    int ret = inven->delete_item((INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)msg.m_0d),
                                 msg.m_11, 1, (eItemDelReason)0x36, true);
    if (ret != 1)
    {
        param.m_4 = 0x11;
        return 0;
    }
    short* timepiece = reinterpret_cast<short*>(reinterpret_cast<char*>(user->getGrowthWeaponEventdata()) + 0x10);
    *timepiece = static_cast<short>(*timepiece + 1);
    DB_EventUpdateGrowthWeaponTimepiece::makeRequest(user->get_acc_id(), *timepiece, 0);
    param.m_10 = *timepiece;
    param.m_8 = msg.m_0d;
    param.m_c = msg.m_11;
    return 0;
