bool WarField::HandleMonsterKill(int uid, PacketGuard& packet, map_monster& mob,
                                 CUser* user, const CDungeon* dungeon)
{
    int userUid = 0xffff;
    int moneyRatio = 0;
    if (user != 0)
    {
        userUid = (unsigned short)user->get_unique_id();
        int moneyLimit = (int)((const CDataManager*)G_CDataManager())
                             ->GetMoneyLimitPerLevel(user->get_charac_level(),
                                                     user->get_acc_name());
        if (moneyLimit < 1)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x178, 5);
            trace("WarField::HandleMonsterKill limitMoney(%d) is under 0", moneyLimit);
        }
        else
        {
            moneyRatio = (user->getCurCharacMoney() * 100) / moneyLimit;
        }
    }
    MapInfo* mi = &m_mapInfo;
    std::map<int, map_monster>::iterator it = mi->m_monsterMap.find(uid);
    if (it == mi->m_monsterMap.end())
    {
        return 0;
    }
    map_monster* fieldMob = &it->second;
    CMonster* monster = (CMonster*)((const CDataManager*)G_CDataManager())
                            ->find_monster(fieldMob->m_mobIndex);
    if (monster == 0)
    {
        LogManager::logFormat(
            1, "WarField.cpp",
            __PRETTY_FUNCTION__,
            0x18c, "G_CDataManager()->find_monster(%u) fail", fieldMob->m_mobIndex);
        return 0;
    }
    mob = *fieldMob;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x26);
    ((InterfacePacketBuf*)&packet)->put_short(uid);
    std::list<map_item> itemList;
    int curSec = GlobalData::s_systemTime_.getCurSec();
    stGenerateRefData_t refData;
    if (user != 0)
    {
        refData.m_dropRate = 100;
        if (ARAD::Singleton<ServiceRestrictManager>::Get()
                ->isRestricted(RestrictCategory::Enum_3, 1))
        {
            refData.m_dropRate = user->getStdDropRate();
        }
    }
    // 掉落上下文区（宽度逐字段 = HandleMonsterKill 086b98b1-0x298 汇编：movb/movl）。
    refData.m_field34 = fieldMob->m_level;               // movb +0x34
    refData.m_field35 = fieldMob->m_roleType;            // movb +0x35
    refData.m_rateSlot = fieldMob->m_level;              // movb +0x36
    refData.m_pad37 = 0;                                 // movb +0x37
    refData.m_field38 = 1;                               // movb +0x38（int 槽低字节）
    refData.m_field39 = *((char*)&fieldMob->m_field11 + 1);  // movb +0x39（mob +0x12）
    refData.m_field3c = moneyRatio;                      // movl +0x3c..0x3f
    refData.m_field40_byte = 1;                          // movb +0x40（指针槽低字节）
    refData.m_field41 = 0;                               // movb +0x41
    stGenerateResult_t result;
    G_CDataManager()
        ->getItemGenerator()
        ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_0)
        ->generateRandomItem(refData, result);
    // 第二段（specific）：+0x38 movl、+0x3c/0x3d/0x35/0x36/0x3e/0x40/0x41/0x43/0x44 movb
    refData.m_rankBonusIndex = fieldMob->m_mobIndex;     // movl +0x38
    refData.m_field3c_byte = fieldMob->m_roleType;       // movb +0x3c
    refData.m_field3d = *(char*)&fieldMob->m_field11;    // movb +0x3d（mob +0x11）
    refData.m_rateSlot = 0;                              // movb +0x36
    refData.m_field3e_byte = 1;                          // movb +0x3e
    refData.m_field35 = fieldMob->m_level;               // movb +0x35
    refData.m_field40_byte = 0;                          // movb +0x40
    refData.m_field41 = 0;                               // movb +0x41
    refData.m_field43 = 0;                               // movb +0x43
    refData.m_field44_byte = 0;                          // movb +0x44
    if (user != 0 && user->isGMUser())
    {
        refData.m_field24 = user->getForceDropFlag();
    }
    G_CDataManager()
        ->getItemGenerator()
        ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_0)
        ->generateSpecificItem(refData, result);
    dungeon->getDropItems();
    G_CDataManager()
        ->getItemGenerator()
        ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_4)
        ->generateSpecificItem(refData, result);
    refData.m_field34 = 1;                               // movb +0x34
    refData.m_field35 = fieldMob->m_level;               // movb +0x35
    G_CDataManager()
        ->getItemGenerator()
        ->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_3)
        ->generateRandomItem(refData, result);
    for (std::vector<Inven_Item>::iterator itemIt = result.m_dropItems.begin();
         itemIt != result.m_dropItems.end(); ++itemIt)
    {
        map_item item;
        item.m_count = 1;
        item.m_itemIndex = mi->m_dropItemCnt;
        mi->m_dropItemCnt = mi->m_dropItemCnt + 1;
        item.m_dropIndex = userUid;
        item.m_createTick = curSec;
        item.m_item = *itemIt;
        itemList.push_back(item);
        mi->Add_Item(item);
    }
    ((InterfacePacketBuf*)&packet)->put_byte((int)itemList.size());
    stAvatarEmblemInfo_t emblem;
    emblem.init();
    for (std::list<map_item>::iterator itemIt = itemList.begin();
         itemIt != itemList.end(); itemIt++)
    {
        map_item& mi = *itemIt;
        ((InterfacePacketBuf*)&packet)->put_short(mi.m_itemIndex);
        ((InterfacePacketBuf*)&packet)->put_int(mi.m_item.m_addInfo);
        ((InterfacePacketBuf*)&packet)->put_byte((int)mi.m_item.GetItemAttr());
        if (mi.m_item.isAvatarItemType())
        {
            CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(mi.m_item.m_addInfo);
            if (equip == 0)
            {
                ((InterfacePacketBuf*)&packet)->put_int(mi.m_item.get_add_info());
            }
            else
            {
                ((InterfacePacketBuf*)&packet)->put_int(
                    equip->getAvatarPeriod((unsigned char)mi.m_item.m_fieldb) * 0x15180);
                emblem.init();
                equip->getAvatarSocket(mi.m_item.m_fieldb, emblem);
            }
        }
        else
        {
            ((InterfacePacketBuf*)&packet)->put_int(mi.m_item.get_add_info());
        }
        ((InterfacePacketBuf*)&packet)->put_short(mi.m_item.m_fieldb);
        ((InterfacePacketBuf*)&packet)->put_byte((int)mi.m_item.m_amp.getAbilityType());
        ((InterfacePacketBuf*)&packet)->put_short(mi.m_item.m_amp.getAbilityValue());
        ((InterfacePacketBuf*)&packet)->put_packet(mi.m_item);
        if (mi.m_item.isAvatarItemType())
        {
            ((InterfacePacketBuf*)&packet)->put_byte(1);
            ((InterfacePacketBuf*)&packet)->put_int(0x1e);
            ((InterfacePacketBuf*)&packet)->put_binary((const char*)&emblem, 0x1e);
            stAvatarExpansionInfo_t expansion;
            expansion.init();
            ((InterfacePacketBuf*)&packet)->put_int(4);
            ((InterfacePacketBuf*)&packet)->put_binary((const char*)&expansion, 4);
        }
        else
        {
            ((InterfacePacketBuf*)&packet)->put_byte(0);
        }
        ((InterfacePacketBuf*)&packet)->put_short(mi.m_dropIndex);
    }
    mi->m_monsterMap.erase(it);
    return 1;
