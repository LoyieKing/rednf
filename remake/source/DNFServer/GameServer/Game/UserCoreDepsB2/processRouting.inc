void CSpecialItemRoutingManager::processRouting(CParty* party)
{
    Inven_Item* routingItem = party->GetRoutingInvenItem();
    if (routingItem->m_addInfo == 0)
    {
        ProcessFailEtc(party, 0);
        return;
    }

    int routingItemId = party->GetRoutingItemID();
    CItem* item = G_CDataManager()->find_item(routingItemId);
    if (item == 0)
    {
        ProcessFailEtc(party, 0);
        return;
    }
    CItem* itemDice = item;                           // ORIG -0x2c = -0x30 副本

    std::vector<std::pair<int, int> > diceMembers;    // (成员序号, 权重)
    diceMembers.clear();                              // ORIG 构造后紧跟 clear 调用
    party->GetRoutingDiceMemberList(diceMembers, itemDice);

    CUser* winner = 0;                                // -0x28
    int winnerIdx = 0;                                // -0x24
    PacketGuard packet;
    packet.put_header(0, 0x11a);
    if (diceMembers.empty())
    {
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_int((int)party->GetRoutingItemUniqueID());
    }
    else if (diceMembers.size() == 1)
    {
        winnerIdx = diceMembers[0].first;
        winner = party->get_user(winnerIdx);
    }
    else
    {
        int bestRoll = -1;                            // -0x20
        std::vector<std::pair<int, int> >::iterator it = diceMembers.begin();
        while (it != diceMembers.end())
        {
            std::pair<int, int> member = *it;         // ORIG 单次 operator* 读双字段
            int roll = get_rand_int(100);             // -0x1c
            if (roll > bestRoll)
            {
                bestRoll = roll;
                winnerIdx = member.first;
                winner = party->get_user(member.first);
            }
            else
            {
                if (roll == bestRoll)
                    goto L_skip_inc;                  // ORIG：roll==best 不前进（重掷）
            }
            it++;                                     // ORIG ppEi 后置
        L_skip_inc:;
        }
    }

    if (winner != 0)
    {
        int space = 0;                                // -0x18 ENUM_ITEMSPACE 结果
        if (item->is_stackable())
        {
            if (item->GetItemType() == 9)             // vptr+0x0c
                space = 7;
        }
        else
        {
            if (item->IsCreatureItem())               // vptr+0x14
                space = 7;
            else if (item->IsAvatarItem())            // vptr+0x10
                space = 1;
            else
                space = 0;
        }

        int result = -1;                              // -0x14
        if (!item->IsAvatarItem())
        {
            result = winner->getCurCharacInvenW()->insertItemIntoInventory(
                *routingItem, (eItemAddReason)4, true, true);
        }
        else
        {
            char ipgAgency[0x18];                     // genIPGNo 输出缓冲（Ghidra local_80[24]）
            WongWork::CGenUniqueNo::genIPGNo(
                (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
                (unsigned int)winner->getCurCharacNo(), ipgAgency);
            result = winner->addAvatarItem(
                (unsigned long)routingItem->m_addInfo, 0,
                (char)*(short*)((const char*)routingItem + 0xb), ipgAgency,
                (eAvatarItemAddReason)0xf);
        }

        if (result == -1)
        {
            packet.put_byte(0);
            packet.put_byte(1);
            packet.put_int((int)party->GetRoutingItemUniqueID());
        }
        else
        {
            if (item->IsRandomOption())               // vptr+0x4c
            {
                char* curDate = GlobalData::s_systemTime_.getCurDate();
                int levelSection = winner->get_charac_10_level_section();
                statistc_proxy::add(winner, "log_random_option", "pick_cnt", 1,
                                    "%d,'%s'", levelSection, curDate);
            }
            int routingUniqueId = (int)party->GetRoutingItemUniqueID(); // -0x80
            int deleteResult = 0;                     // -0x10
            int state = winner->get_state();
            if (state == 5)
            {
                deleteResult = party->DeleteDungeonDropItem(routingUniqueId);
            }
            else if (state == 0xc)
            {
                WongWork::CBossTower* tower = winner->getBossTower();
                if (tower != 0)
                {
                    // m_stageData.m_itemMap 位于 CBossTower+0xa18
                    // （CBossStage+0x9f4 的 MapInfo +0x24）。CBossStage 后段字段
                    // 为 private 且无具名访问器，按既有 TU 惯例以地址访问。
                    ((std::map<int, map_item>*)((char*)tower + 0xa18))
                        ->erase(routingUniqueId);
                }
            }
            if (0 < deleteResult)
            {
                LogManager::logFormat(
                    1, "SpecialItemRoutingManager.cpp",
                    __PRETTY_FUNCTION__,
                    0x165, "[EPIC ROUTING] DELETE DUNGEON ITEM FAIL");
            }

            packet.put_byte(1);
            packet.put_byte(winnerIdx);
            packet.put_int(routingUniqueId);
            packet.put_byte((int)(char)space);
            packet.put_short(result);
            winner->SendUpdateItem((CUser::eSendTarget)1,
                                   (ENUM_ITEMSPACE)(int)space, result);
        }
    }

    packet.finalize(true);
    party->send_to_party(packet);
    party->SetEndRouting();
