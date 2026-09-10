void CParty::ClearDungeon()
{
    if (m_tournamentVictory != 0)
        return;
    if (m_battleField.m_bloodState == 1)
    {
        for (int slot = 0; slot < 4; ++slot)
        {
            if (!_checkValidUser(slot))
                continue;
            OnSuccessedDungeonClear(m_member[slot].m_pUser);
        }
        SetEPLPState(2);
        unsigned int key = (unsigned int)gen_timer_key((TIMER_MESSAGE)0x15);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                    (TIMER_MESSAGE)0x15, 0x3c, (int)key, 0);
        set_state(1);
        PacketGuard packet;
        packet.put_header(0, 0xfe);
        packet.finalize(true);
        send_to_party(packet);
        m_battleField.m_dungeonClearTracer.Trace("void CParty::ClearDungeon()");
        m_tournamentVictory = 1;
        return;
    }
    // [b28d-solo-CPartyDepsD2] ORIG 0x085a94e0..0x085a9525：无 now/enterTick/
    // baseTick/elapsed 具名局部（P0）——`sub/sbb` 直对 64 位字段运算、
    // 结果 `mov %eax,0x32c(%ecx); mov %edx,0x330(%ecx)` 双字直存。
    // 已改 BattleData.h 64 位 union 视角（探针 /tmp/probe_ll.cpp D 形）。
    bd(this).m_clearPlayedTime =
        (unsigned long long)GlobalData::s_systemTime_.getCurTickCount()
        - bd(this).m_clearEnterTime - bd(this).m_accTick;
    int memberCount = get_member_count();
    if (m_battleField.m_pDungeon != 0)
    {
        for (int slot = 0; slot < 4; ++slot)
        {
            if (!_checkValidUser(slot))
                continue;
            // [b28d-solo-CPartyDepsD2] P183：ORIG 0x085a9589/0x85a95e0/... 每处调用点
            // 各自重算 `m_member[slot].m_pUser`（9 条 stride 0x18 寻址），无 CUser* 缓存。
            if (m_battleField.m_pDungeon->m_villageAttackEventFlag == 0)
            {
                if (m_battleField.m_pDungeon->get_index() == 100)
                    APSystem::CUserProc::ClearActionAndSendtoUser(
                        m_member[slot].m_pUser, (APSystem::_ActionGroupIndex)0x19, 0,
                        (ENUM_PACKETCLASS)0);
                else if (m_battleField.m_pDungeon->m_89c == 0)
                {
                    if (m_battleField.m_pDungeon->get_dimension_possible() > 0)
                        APSystem::CUserProc::ClearActionAndSendtoUser(
                        m_member[slot].m_pUser, (APSystem::_ActionGroupIndex)0x1c, 0,
                        (ENUM_PACKETCLASS)0);
                }
                else
                    APSystem::CUserProc::ClearActionAndSendtoUser(
                        m_member[slot].m_pUser, (APSystem::_ActionGroupIndex)0x1b, 0,
                        (ENUM_PACKETCLASS)0);
            }
            else
                APSystem::CUserProc::ClearActionAndSendtoUser(
                        m_member[slot].m_pUser, (APSystem::_ActionGroupIndex)0x18, 0,
                        (ENUM_PACKETCLASS)0);
            if (!m_member[slot].m_pUser->IsUsedCoinInDungeon())
                APSystem::CUserProc::ClearActionAndSendtoUser(
                        m_member[slot].m_pUser, (APSystem::_ActionGroupIndex)0xf, 0,
                        (ENUM_PACKETCLASS)0);
        }
    }
    PacketGuard packet;
    if (m_battleField.m_pDungeon->m_bloodDungeonType >= 1)
    {
        bf(this).m_bloodReward.onFinishBloodRound(
            true, (short)bf(this).m_bloodRound.getBloodRoundOrder(), this, m_battleField.m_pDungeon);
        unsigned int key = (unsigned int)gen_timer_key((TIMER_MESSAGE)0x22);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                    (TIMER_MESSAGE)0x22, 5, (int)key, 0x22);
    }
    else
    {
        if (!m_battleField.m_pDungeon->isTowerOfDespairDungeon())
        {
            if (!m_battleField.m_pDungeon->isTournamentDungeon())
            {
                packet.put_header(0, 0x1f);
                packet.finalize(true);
            }
            else
            {
                m_tournamentVictory = 1;
                bf(this).m_tournamentReward.ResetReward();
                MakeTournamentReward(bf(this).m_tournamentReward);
            }
        }
        else
        {
            m_tournamentVictory = 1;
            // [b28d-solo-CPartyDepsD2] P183：ORIG 0x085a93xx.. 每次重调
            // getManager()（4 处 call），无具名 manager 缓存。
            TOD_UserState* todState = reinterpret_cast<TOD_UserState*>(
                getManager()->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xd));
            todState->ClearLayer();
            int layerCount = 5;
            bf(this).m_todReward.m_todPos.first = -1;
            bf(this).m_todReward.m_todPos.second = 0;
            // ORIG 0x085a9330：每次取层都是 todState->getLastEnterLayer()
            //（sret 临时，无 TOD_Layer(ushort) 构造调用点，共 5 处）。
            int apcIdx = reinterpret_cast<TowerOfDespairMgr*>(
                GlobalData::s_pTowerOfDespairMgr)->Get_APCIndexByLayer(
                    (unsigned int)todState->getLastEnterLayer().GetLayer() & 0xffff);
            if (apcIdx != -1)
            {
                const char* apcName = reinterpret_cast<TowerOfDespairMgr*>(
                    GlobalData::s_pTowerOfDespairMgr)->Get_APCNameByLayer(
                        todState->getLastEnterLayer());
                if (apcName != 0 && *apcName != '\0')
                {
                    PacketGuard namePacket;
                    namePacket.put_header(0, 0x56);
                    namePacket.put_byte(8);
                    namePacket.put_short((int)todState->getLastEnterLayer().GetLayer());
                    namePacket.put_int((int)strlen(apcName));
                    namePacket.put_str(apcName, (int)strlen(apcName));
                    const char* myName = getManager()->getCurCharacName();
                    if (myName == 0)
                    {
                        namePacket.put_int(5);
                        namePacket.put_str("Neople", 6);
                    }
                    else
                    {
                        namePacket.put_int((int)strlen(myName));
                        namePacket.put_str(myName, (int)strlen(myName));
                    }
                    namePacket.finalize(true);
                    G_GameWorld()->send_all(namePacket);
                }
                layerCount = 9;
                bf(this).m_todReward.m_todPos.first = 0x4e4;
                bf(this).m_todReward.m_todPos.second = 1;
            }
            if (todState->getLastEnterLayer().GetLayer() == 100)
            {
                bf(this).m_todReward.m_todPos.first = 0xcf2;
                bf(this).m_todReward.m_todPos.second = 1;
            }
            WongWork::stGenerateRefData_t ref;
            WongWork::stGenerateResult_t result;
            std::vector<std::pair<int, int> > itemList;
            ref.m_field34 = 0;
            ref.m_field35 = 0;
            ref.m_rateSlot = 1;
            ref.m_field1c = 1.0f;
            ref.m_dropRate = 100;
            for (int i = 0; i < layerCount; ++i)
            {
                WongWork::CItemGeneratorMgr* genMgr =
                    G_CDataManager()->getItemGenerator();
                // [b28d-SweepCPartyDepsD2-2] 真 bug 修复：ORIG 0x085a9b6e
                // `add $0x8,%edx` = vptr+0x08 = _ZTVN8WongWork14CItemGeneratorE
                // word[4] @0x08c9a798 = 0x0853a400 =
                // CItemGenerator::generateRandomItem（Ghidra C
                // `(**(code **)(*piVar14 + 8))(piVar14, ref, result)`）；
                // 旧码调 generateSpecificItem（vptr+0x0c，word[5]）⇒ 运行时选错虚函数。
                genMgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
                    ->generateRandomItem(ref, result);
            }
            itemList.resize(result.m_dropItems.size());
            for (size_t i = 0; i < result.m_dropItems.size(); ++i)
            {
                const Inven_Item& item = result.m_dropItems[i];
                int itemIdx = item.m_addInfo;
                itemList[i].first = itemIdx;
                CItem* card = G_CDataManager()->find_item(itemIdx);
                if (card != 0 && !card->is_stackable())
                    itemList[i].second = 1;
                else
                    // ORIG 1315 段直接读字段 +0x07（m_addInfo2），无 get_add_info call
                    itemList[i].second = item.m_addInfo2;
            }
            bf(this).m_todReward.m_todMap = itemList;
            if (bf(this).m_todReward.m_todPos.first != -1)
                itemList.push_back(bf(this).m_todReward.m_todPos);
            std::vector<int> slots;
            int putCount = (int)getManager()->getCurCharacInvenW()->insert_event_items(
                itemList, slots, (eMoneyAddReason)0x1f, (eItemAddReason)0x37, true,
                "game_server_msg_191", "game_server_msg_192");
            if (putCount >= 1 && slots.size() >= 3)
            {
                // [b28f-CTS 轴B] ORIG 0x085a9dc9..0x085a9dd7 为
                //   `addl $0x1,-0x14; addl $0x3,-0x10; mov -0x2c,%eax;
                //    cmp -0x14,%eax; seta` ⇒ 原码是**无符号**循环变量
                //   + **独立 unsigned 步进累加器**（off += 3，非 i*3 的
                //   imul/lea 强度归约）。P196 前置已核：ORIG 为 seta
                //   （无符号序）⇒ 允许/要求改 unsigned（禁止改 int）。
                for (unsigned int i = 0, off = 0; i < (unsigned int)putCount;
                     ++i, off += 3)
                {
                    unsigned int slot = (unsigned int)slots[off];
                    getManager()->SendUpdateItem((CUser::eSendTarget)1,
                                                (ENUM_ITEMSPACE)0, (int)slot);
                }
            }
            unsigned int key = (unsigned int)gen_timer_key((TIMER_MESSAGE)0x2c);
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                        (TIMER_MESSAGE)0x2c, 2, (int)key, 0x2c);
            Packet_TowerOfDespair_Statistic_GTS todStat;
            todStat.m_key.m_0 = (int)todState->getLastEnterLayer().GetLayer();
            todStat.m_key.m_4 = 0;
            CStatisticServerProxy* proxy =
                GlobalData::s_statistic_proxy_mgr->GetServerProxy(
                    (ENUM_SERVER_GROUP)0);
            if (proxy != 0)
                proxy->SendPacket((char*)&todStat, 0x17);
        }
    }
    for (int slot = 0; slot < 4; ++slot)
    {
        if (!_checkValidUser(slot))
            continue;
        CUser* user = m_member[slot].m_pUser;
        user->getCurCharacQuestW()->set_authen_data((QUEST_CONDITION)4,
                                                    memberCount, 0);
        // ORIG 0x85aa027..0x85aa057：64 位 elapsed（m_field0 低/ m_field4 高，
        // 即 this+0x32c/+0x330）经 __divdi3 除以 1000 后取低 32 位入第二个
        // set_authen_data（QUEST_CONDITION)2；ORIG 每个 set 前各调一次
        // getCurCharacQuestW（两次 call，互不复用）。
        // [b28d-solo-CPartyDepsD2] ORIG 0x85aa027 段对 +0x32c/+0x330 直接做
        // 64 位 __divdi3（`mov 0x330(%eax),%edx` 单次高半装载，无 sar/or 拼接）
        // ⇒ 用 BattleData.h 的 64 位 union 视角（P180）。
        user->getCurCharacQuestW()->set_authen_data(
            (QUEST_CONDITION)2,
            (int)((long long)bd(this).m_clearPlayedTime / 1000),
            0);
        if (m_battleField.m_pDungeon->m_bloodDungeonType == 0 &&
            !m_battleField.m_pDungeon->isTowerOfDespairDungeon() &&
            !m_battleField.m_pDungeon->isTournamentDungeon())
            user->Send(packet);
        secretshop::SECRET_SHOP_DATA* shopData = &m_secretShop;
        if (shopData->IsOpen() != 0)
        {
            int level = m_battleField.m_pDungeon->get_standard_level();
            int dungeonIdx = m_battleField.m_pDungeon->get_index();
            // +0x2a8+slot*0x1c = SECRET_SHOP_DATA.m_info[slot].m_retailer.m_sales
            //（+0x2a4 SECRET_SHOP_DATA；m_info +0x04，SECRET_SHOP_INFO 0x1c）。
            std::vector<secretshop::SALE_INFO>& shopItems =
                shopData->m_info[slot].m_retailer.m_sales;
            GlobalData::s_secret_shop->LotteryItems(
                shopItems,
                shopData->m_openCode, dungeonIdx, level);
            SendSecretShopItemList(user);
            APSystem::CUserProc::ClearActionAndSendtoUser(
                        user, (APSystem::_ActionGroupIndex)0xd, 0, (ENUM_PACKETCLASS)0);
        }
        if (user->getBreakAwayDungeonClearCnt() > 0)
            user->subBreakAwayDungeonClearCnt();
    }
    memberCount = get_member_count();
    if (memberCount == 1)
    {
        // [b28f-CTS] E03 把 get_hellparty_script_values() 返回类型 void*→
        //   stHellParty_t*（P211 callee 返回类型旋钮）⇒ 到 const float* 的
        //   static_cast 不再合法，改 reinterpret_cast（语义/codegen 不变）。
        const float* hellValues = reinterpret_cast<const float*>(
            G_CDataManager()->get_hellparty_script_values());
        bd(this).SetHellPartyValueTotal((int)*(int*)((char*)hellValues + 8));
    }
    // ORIG 0x85aa27e..0x85aa2b0 比较 CParty+0xcee/0xcef/0xcdc（= CBattle_Field
    // +0x1ca m_killedAllHellGroups / +0x1cb m_field1cb / +0x1b8
    // m_hellPartyActive）三个字节均非 1 时 cal_hellparty_value；
    // 报告 ClearDungeon.md 反编译 C 末尾条件块。
    if (memberCount != 1 && bf(this).m_killedAllHellGroups != 1 &&
        bf(this).m_field1cb != 1 && bf(this).m_hellPartyActive != 1)
    {
        cal_hellparty_value(bf(this).get_dungeon_diff(),
                            get_rate_level_difference(), memberCount - 1);
    }
    m_battleField.m_dungeonClearTracer.Trace("void CParty::ClearDungeon()");
    m_tournamentVictory = 1;
    QuickParty::RewardData_Param rewardParam(this);
    QuickParty::RewardData_Result rewardResult;
    set_random_reward((QuickParty::RandomBuffType)10, rewardParam, rewardResult);
    rewardResult.clear();
    return;
