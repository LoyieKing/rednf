// [b28d-lane2] verdict=DIFF ndiff=410（ORIG 0x084c3f92）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-26。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=288（ORIG 0x84c3f92）：N101 imul/lea 折叠+N7 间接调用寄存器+N82/P112 帧槽平移。
int Inter_LoadInven::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2D] verdict=N（本轮未改）：199 行残差三类——(a) 帧 0x40→0x84 的具名局部累积（本件
    //   132 行内我方独有槽往返 20+ 处，逐处改写风险>>收益，帧机制由父泳道统一裁决）；
    //   (b) 地址表达式形态：ORIG 索引式 `sub/lea`（如 084c4fxx `lea 0x12880(%edx),%eax`）
    //   vs 我方 `add $0x12880,%eax; add 0x10(%ebp),%eax`；(c) `mov <S>,%ecx` 类中转。
    //   无可直证的语义/结构单点，留 N。
    SIG_LOAD_INVENTORY* load = reinterpret_cast<SIG_LOAD_INVENTORY*>(data);
    if (user->get_state() == 0)
        return 0x1835;
    if (load->m_result == 1)          // 重复加载保护位
        return 0x183c;
    if (user->getCurCharacR() == 0)
        return 0x1841;

    // P48 去缓存：ORIG 每用点重取 getCurCharacInvenW；set_inventory_capacity 走 InvenR
    // （ORIG (CInventory*)getCurCharacInvenR 丢 const 调非 const 方法）。
    const_cast<CInventory*>(user->getCurCharacInvenR())->set_inventory_capacity(load->m_inventoryCapacity);
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
        // [R5·InterDisp 形态收口 2026-09-11] 去具名槽 + 条件每轮重读（P0/P183）：
        //   ORIG 0x084c4122 `mov -0x34(%ebp),%eax; mov 0x1287c(%eax),%eax; cmp -0x2c(%ebp),%eax`
        //   ⇒ count 无栈槽、每轮重读；item/detail 亦不落槽（0x084c40cc `mov %eax,%ebx; add -0x34(%ebp),%ebx`
        //   直落实参寄存器、0x084c40f8 `lea 0x1(%eax),%esi`）。我方旧形为 -0x5c/-0x4c/-0x48 三个具名槽。
        SIG_LOAD_INVENTORY* sigLoad = reinterpret_cast<SIG_LOAD_INVENTORY*>(data);
        for (int i = 0; i < sigLoad->m_creatureCount; ++i)
        {
            SIG_CREATURE_ITEM_DETAIL_INFO* citem =
                reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(
                    &sigLoad->m_creature[i * 0x38]);
            user->getCurCharacInvenW()->GetCreatureMgrW()->LoadCreatureItem(
                &sigLoad->m_invenType3[citem->m_slotNo],
                citem);
        }
        // 同族：ORIG 0x084c4172 `mov -0x34(%ebp),%eax; mov 0x15340(%eax),%eax` 每轮重读。
        for (int i = 0; i < load->m_creatureCount2; ++i)
        {
            user->getCurCharacInvenW()->GetCreatureMgrW()->LoadCreatureTempPeriod(
                // 权威槽：SIG_LOAD_INVENTORY::m_tempPeriods[0x14] @+0x15344
                //（= (0x2a68*8+4)，元素 8 字节 = sizeof(SIG_POSTAL_CREATURE_TEMP_PERIOD)）。
                &reinterpret_cast<SIG_LOAD_INVENTORY*>(data)->m_tempPeriods[i]);
        }
    }
    if (!user->SetInventory(reinterpret_cast<SIG_LOAD_INVENTORY*>(data)))
        return 0x1872;

    const unsigned int tagCharacNo =
        load->m_field8;
    const int tagIdx = user->get_charac_idx(tagCharacNo);
    if (tagCharacNo != 0 && tagIdx != -1 &&
        !user->setTagCharacInven(&reinterpret_cast<SIG_LOAD_INVENTORY*>(data)->m_tagInven))
        return 0x1892;

    user->getCurCharacInvenW()->SetClearAvatar(
        &load->m_clearAvatar[0]);   // §4.3 字段化：视图 m_14758(unsigned long) 取址，ORIG lea 0x14758(%eax)
    (void)tagIdx;
    {
        SIG_LOAD_INVENTORY* sig = reinterpret_cast<SIG_LOAD_INVENTORY*>(data);
        const int avatarCount = sig->m_avatarCount;
        for (int i = 0; i < avatarCount; ++i)
        {
            const int avatarItemIdx =
                sig->m_avatarDetail[i].m_clearAvatarId;
            int addInfo = 0;
            if (avatarItemIdx != 0)
            {
                addInfo = GetItemIdByAddInfo(&load->m_invenType2[0],
                                             0x69, avatarItemIdx);
                for (int j = 0; j < 10; ++j)
                {
                    if (sig->m_equipMirror[j].m_addInfo2 ==
                        sig->m_avatarDetail[i].m_key)
                    {
                        user->getCurCharacInvenW()->SetClearAvatar(j, addInfo);
                    }
                }
            }
            AvatarDetailInfo* detail = &sig->m_avatarDetail[i];
            user->getCurCharacInvenW()->GetAvatarItemMgrW()->RegistItem(*detail, false);
        }
    }
    {   // maxEquipLevel msg+0x15448 非 0 时设置
        const int maxEquipLevel = load->m_maxEquipLevel;
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
        e2->loadData(user, reinterpret_cast<char*>(
            reinterpret_cast<SIG_LOAD_INVENTORY*>(data)->m_locks));
    }
    {
        CExpandEquipslot* e9 =
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        e9->loadData(user, reinterpret_cast<char*>(
            &reinterpret_cast<SIG_LOAD_INVENTORY*>(data)->m_expandEquip));
    }
    user->disableSaveInven();
    user->disableSaveCargo();
    _UpdateRentalItems(user, reinterpret_cast<SIG_LOAD_INVENTORY*>(data));
    _UpdateDimensionRentalItems(user, reinterpret_cast<SIG_LOAD_INVENTORY*>(data));
    return 0;
}