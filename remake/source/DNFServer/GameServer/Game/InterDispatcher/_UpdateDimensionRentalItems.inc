void Inter_LoadInven::_UpdateDimensionRentalItems(CUser* user,
                                                  SIG_LOAD_INVENTORY* sig)
{
    if (user == 0 || sig == 0)
        return;

    const int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
    {
        user->DeleteRentalItemInfo(2);
        return;
    }

    InstanceRentalSystem::RentInput input;
    input.m_machineKey = 2;
    if (!user->CheckLoadRentalInfoFromExchange())
    {
        std::vector<InstanceRentalSystem::RentalInfo> rentalList;
        if (user->GetCurrentCharacRentalInfo(rentalList, 2))
        {
            for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it =
                     rentalList.begin();
                 it != rentalList.end(); ++it)
            {
                // P167/P0：ORIG 单次 operator*（de）→ 槽往返 → 字段回读 ⇒ 具名引用局部。
                const InstanceRentalSystem::RentalInfo& r = *it;
                InstanceRentalSystem::RentInElem elem;      // ← RentalInfo+8..+0xf
                elem.m_sectionIdx = r.m_field8;             // +0x08
                elem.m_itemIdx = r.m_field9;                // +0x09
                elem.m_bEquip = r.m_inEquip != 0 ? 1 : 0;   // +0x0a
                elem.m_type = r.m_fieldB;                   // +0x0b
                elem.m_rentItemId = r.m_rentItemId;         // +0x0c
                input.m_elems.push_back(elem);
            }
        }
    }
    else
    {
        for (int i = 0; i <= 0xa; ++i)
        {
            char* p = reinterpret_cast<char*>(sig) + (i + 0x2a7e) * 8;
            if (*reinterpret_cast<int*>(p) == 0)
                continue;
            InstanceRentalSystem::RentInElem elem;
            elem.m_sectionIdx = p[4];
            elem.m_itemIdx = p[5];
            elem.m_bEquip = p[6] != 0 ? 1 : 0;
            elem.m_type = p[7];
            elem.m_rentItemId = -1;
            input.m_elems.push_back(elem);
        }
    }

    if (input.m_elems.empty())
    {
        user->DeleteCurrentCharacRentalItemInfo(2);
    }
    else
    {
        InstanceRentalSystem::RentOutput output;
        const int rentRet =
            static_cast<int>(GlobalData::s_rentalSystem->Rent(*user, input, output));
        if (rentRet == 0)
            user->UpdateCurrentCharacRentalInfo(output, 2, true);
    }
