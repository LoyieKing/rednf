int Inter_LoadInven::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_LoadInvenPktView* pPkt = reinterpret_cast<Inter_LoadInvenPktView*>(data);
    if (user->get_state() == 0)
        return 0x1835;
    if (pPkt->m_4030 == 1)          // 重复加载保护位
        return 0x183c;
    if (user->getCurCharacR() == 0)
        return 0x1841;

    // P48 去缓存：ORIG 每用点重取 getCurCharacInvenW；set_inventory_capacity 走 InvenR
    // （ORIG (CInventory*)getCurCharacInvenR 丢 const 调非 const 方法）。
    const_cast<CInventory*>(user->getCurCharacInvenR())->set_inventory_capacity(pPkt->m_1533c);
    {   // 背包校验：失败断连（ORIG _Z16_verifyInventory W 0x084f0145）
        CExpandEquipslot* expandData =
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)2);
        item_lock::CItemLock* lockData =
            reinterpret_cast<item_lock::CItemLock*>(expandData);
        std::vector<item_lock::stItemLockRef>& lockVec = lockData->GetItemLockRefVec();
        if (!_verifyInventory(reinterpret_cast<SIG_LOAD_INVENTORY*>(data), lockVec, user))
        {
            user->DisConnSig((DISCONN_SIG)0x24, true, 0);
        }
    }

    user->getCurCharacInvenW()->GetCreatureMgrW()->Reset();
    user->getCurCharacInvenW()->GetCreatureMgrW()->SetUser(user);
    {
        const int creatureCount = pPkt->m_1287c;
        for (int i = 0; i < creatureCount; ++i)
        {
            const int itemOffset = *reinterpret_cast<int*>(data + 0x12880 + i * 0x38);
            Inven_Item* item = reinterpret_cast<Inven_Item*>(
                data + 0xeed1 + itemOffset * 0x3d);
            SIG_CREATURE_ITEM_DETAIL_INFO* detail =
                reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(
                    data + 0x12880 + i * 0x38);
            user->getCurCharacInvenW()->GetCreatureMgrW()->LoadCreatureItem(item, detail);
        }
        const int tempPeriodCount = pPkt->m_15340;
        for (int i = 0; i < tempPeriodCount; ++i)
        {
            SIG_POSTAL_CREATURE_TEMP_PERIOD* period =
                // 权威槽：SIG_LOAD_INVENTORY::m_tempPeriods[0x14] @+0x15344
                //（= (0x2a68*8+4)，元素 8 字节 = sizeof(SIG_POSTAL_CREATURE_TEMP_PERIOD)）。
                &reinterpret_cast<SIG_LOAD_INVENTORY*>(data)->m_tempPeriods[i];
            user->getCurCharacInvenW()->GetCreatureMgrW()->LoadCreatureTempPeriod(period);
        }
    }
    if (!user->SetInventory(reinterpret_cast<SIG_LOAD_INVENTORY*>(data)))
        return 0x1872;

    const unsigned int tagCharacNo =
        pPkt->m_8;
    const int tagIdx = user->get_charac_idx(tagCharacNo);
    if (tagCharacNo != 0 && tagIdx != -1 &&
        !user->setTagCharacInven(reinterpret_cast<TAG_CHARAC_INVEN_DATA*>(data + 0xc)))
        return 0x1892;

    user->getCurCharacInvenW()->SetClearAvatar(
        &pPkt->m_14758);   // §4.3 字段化：视图 m_14758(unsigned long) 取址，ORIG lea 0x14758(%eax)
    (void)tagIdx;
    {
        const int avatarCount = pPkt->m_cd1c;
        for (int i = 0; i < avatarCount; ++i)
        {
            const int avatarItemIdx =
                *reinterpret_cast<int*>(data + i * 0x4b + 0xcd45);
            int addInfo = 0;
            if (avatarItemIdx != 0)
            {
                addInfo = GetItemIdByAddInfo(&pPkt->m_4044,
                                             0x69, avatarItemIdx);
                for (int j = 0; j < 10; ++j)
                {
                    if (*reinterpret_cast<int*>(data + j * 0x3d + 0xa3a8) ==
                        *reinterpret_cast<int*>(data + i * 0x4b + 0xcd20))
                    {
                        user->getCurCharacInvenW()->SetClearAvatar(j, addInfo);
                    }
                }
            }
            AvatarDetailInfo* detail = reinterpret_cast<AvatarDetailInfo*>(
                data + i * 0x4b + 0xcd20);
            user->getCurCharacInvenW()->GetAvatarItemMgrW()->RegistItem(*detail, false);
        }
    }
    {   // maxEquipLevel msg+0x15448 非 0 时设置
        const int maxEquipLevel = pPkt->m_15448;
        if (maxEquipLevel != 0)
            user->SetCurCharacMaxEquipLevel(maxEquipLevel);
    }
    _CheckOverEquipItem(user);
    _CheckUsableExpertJobEquipment(user);
    user->EnableCharacInfo((CACHE_SECTION)1);
    if (G_GameWorld()->GetChannelType() == 7)
    {   // 战场频道强制卸下强化
        Inven_Item* ref = user->getCurCharacInvenW()->GetInvenRef(0, 10);
        ref->SetUpgrade(0);
        for (int slot = 0xb; slot < 0x16; ++slot)
        {
            user->getCurCharacInvenW()->GetInvenRef(0, slot)->SetUpgrade(0);
        }
        for (int slot = 3; slot < 0x39; ++slot)
        {
            Inven_Item* it = user->getCurCharacInvenW()->GetInvenRef(1, slot);
            if (it->m_field1 == '\x01')
            {
                if (G_CDataManager()->find_item(it->m_addInfo) != 0)
                {
                    it->SetUpgrade(0);
                }
            }
        }
    }
    // 扩展槽族槽表导入（ORIG vptr+4 = CExpandEquipslot::loadData 虚槽；类型完整，转真调用）
    // P48：ORIG 两独立显式调用（section 2 / 9），非循环（循环合并发明 helper 形）。
    {
        CExpandEquipslot* e2 =
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)2);
        e2->loadData(user, data + 0x14784);
    }
    {
        CExpandEquipslot* e9 =
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        e9->loadData(user, data + 0x14aa4);
    }
    user->disableSaveInven();
    user->disableSaveCargo();
    _UpdateRentalItems(user, reinterpret_cast<SIG_LOAD_INVENTORY*>(data));
    _UpdateDimensionRentalItems(user, reinterpret_cast<SIG_LOAD_INVENTORY*>(data));
    return 0;
