int RentalMachineDimension::Rent(CUser& user,
                                 const InstanceRentalSystem::RentInput& input,
                                 InstanceRentalSystem::RentOutput& output)
{
    if (!pc_room::CheckRentableEnvironmentAtChannelType(user))
    {
        return 7;   // ENUM_ERROR 7
    }
    if (!checkRentalLevel(user.get_charac_level()))
    {
        return 14;  // ENUM_ERROR 0xe（14）
    }
    if (!checkRentalGrowJob((int)(signed char)user.getCurCharFirstGrowType()))
    {
        return 254;   // ENUM_ERROR 0xfe（254）
    }
    int err = CheckupAllRentalItems(user, input);
    if (err != 0)
    {
        return err;
    }

    pc_room::DeleteAllRentalItemInSameSectionForNewRent(user, output.m_deleted,
                                                        2);
    // ORIG 0x0826f3xx：const_iterator 三段循环（begin/end/ne/deEv/ppEv，
    // p167 MISSING 实证），非 size()/operator[] 下标形。
    for (std::vector<InstanceRentalSystem::RentInput::RentInElem>::const_iterator
             it = input.m_elems.begin();
         it != input.m_elems.end(); ++it)
    {
        const InstanceRentalSystem::RentInElem& elem = *it;
        EventRewardItemInfo info;
        if (!GetRentItemInfo(user, elem, info))
        {
            LogManager::logFormat(
                1, "RentalMachineDimension.cpp",
                "virtual ENUM_ERROR RentalMachineDimension::Rent(CUser&, const "
                "InstanceRentalSystem::RentInput&, "
                "InstanceRentalSystem::RentOutput&)",
                0x43,
                "Rent error. GetDimensionRentItemInfo return false. "
                "scriptSectionIndex(%d), scriptSeqIndex(%d)",
                (int)(signed char)elem.m_sectionIdx,
                (int)(signed char)elem.m_itemIdx);
            return 1;   // ENUM_ERROR 1
        }
        // ORIG 0x0826ed9f `test %al,%al; je <ProcessInsertItem>`（equip 直落）
        // ⇒ 条件取反 + 两臂交换复现 4.1 的 else-first 块布局（P47/P222 族）。
        if (elem.m_bEquip != 0)
        {
            pc_room::ProcessEquipItem(user, elem, info, output, 0x3e);
        }
        else
        {
            pc_room::ProcessInsertItem(user, elem, info, output, 0x3e);
        }
        if (input.m_logFlag != 0)
        {
            log(user, info.m_field0);
        }
    }
    return 0;   // 0=成功
