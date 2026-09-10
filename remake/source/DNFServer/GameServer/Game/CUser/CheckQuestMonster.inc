bool CUser::CheckQuestMonster(int a, int b, ENUM_QUEST_ENEMY_TYPE type)
{
    // 0x0866cb04 主分支（CheckQuestMonster.md）：遍历进行中任务表
    // UserQuest::m_quest[i]（<=0 跳过；0x13 上限），Quest::m_type==3/4 或
    // UserQuest::m_trigger[i] 触发位；CheckKillMonster(a,
    // m_authenData.m_characClass@+0x75da, b, type) 掷掉落：rate = 炼金/背包
    // GetQuestItemDropBouns/队伍倍率/premium 加成；掷 tryCount 次 rand(100)；
    // 入包 0xe（新槽 insert / 已有槽累加封顶；needCount<=已有则删掉落项重掷）。
    extern Inven_Item g_emptySlot;   // ORIG 全局（0x943ddc0）
    // ORIG 以 getCurCharacQuestRefR()（= &CUser::m_quest，@+0x858d4）为基址：
    // +0x7538 表 = m_quest[i]、+0x7588 = m_trigger[i]、+0x75da = m_characClass。
    const UserQuest* questRef = getCurCharacQuestRefR();
    for (int i = 0; i <= 0x13; ++i)
    {
        // b28d-lane5 复测：ORIG 无 questIdx 槽（P0 右值内联）——三处使用点各重读
        //   `mov -0x60(%ebp),%edx; mov -0x64(%ebp),%eax; add $0x1d4c,%edx;
        //    mov 0x8(%eax,%edx,4),%eax`（0x0866cb29/0x0866cb41/0x0866cb6b）⇒ 删具名缓存。
        if (questRef->m_quest[i] <= 0)
            continue;
        Quest* quest = G_CDataManager()->find_quest(questRef->m_quest[i]);
        if (quest == 0)
        {
            LogManager::logFormat(1, "user.cpp",
                "bool CUser::CheckQuestMonster(int, int, ENUM_QUEST_ENEMY_TYPE)",
                0x54f7, "G_CDataManager()->find_quest(%d) fail",
                questRef->m_quest[i]);
            continue;
        }
        if (quest->m_type != 4 && quest->m_type != 3 &&
            questRef->m_trigger[i] == 0)
            continue;

        std::vector<MonsterKillItem> kills;
        if (quest->CheckKillMonster(a, questRef->m_authenData.m_characClass, b,
                                    type, kills) != 1)
            continue;

        bool again = true;
        while (again)
        {
            again = false;
            if (kills.empty())
                break;
            // P0 声明序（ORIG idx 80-104）：MonsterKillItem 默认构造 @-0xc0 →
            //   4 个 int 清零 @-0x58/-0x54/-0x50/-0x4c → pick @-0x48 → 短路判定。
            //   结构赋值 `drop = kills[pick]`（非引用绑定——ORIG 有 C2Ev call）。
            MonsterKillItem drop;
            int itemIdx = 0;      // 掉落道具 idx
            int tryCount = 0;     // 掷骰次数
            int dropRate = 0;     // 基础掉率
            int needCount = 0;    // 需要数量（-1=无限）
            int pick = get_rand_int((int)kills.size());
            // P338 块布局：ORIG 条件为 De Morgan 正向形 `!(pick >= 0 && size > pick)`
            //   （汇编 js/jle → mov $1 → test/je SKIP），非 `pick < 0 || size <= pick`
            //   （后者 4.4 生成 jg → mov $0 → test/jne BREAK 的反向块）。
            if (!(pick >= 0 && (int)kills.size() > pick))
                break;
            drop = kills[pick];
            itemIdx = drop.m_field10;
            tryCount = drop.m_field14;
            dropRate = drop.m_field18;
            needCount = drop.m_field1c;

            int bonus = 0;
            for (int s = 0; s < 0x1a; ++s)
            {
                // ORIG 0x0866cd51：`getCurCharacInvenR()->GetInvenRef(0,s)` 走
                //   CInventory::GetInvenRef(int,int) **const**（_ZNK…Eii），返回
                //   const Inven_Item*——无 const_cast（P167 重载选型）。
                const Inven_Item* it =
                    getCurCharacInvenR()->GetInvenRef((INVEN_TYPE)0, s);
                if (it != 0 && it->m_addInfo != 0)
                    bonus += dropRate *
                             G_CDataManager()->m_itemList
                                 ->GetQuestItemDropBouns(
                                     reinterpret_cast<unsigned long&>(
                                         const_cast<Inven_Item*>(it)->m_addInfo)) /
                             100;
            }
            int rate = expert_job::CAlchemist::GetQuestItemDropRate(
                           this, dropRate) +
                       bonus;
            if (G_CEnvironment()->m_serverEnvir.m_serverType == 2)
            {
                CParty* party = (CParty*)GetParty();
                if (party != 0)
                {
                    int count = party->get_member_count();
                    float mult;
                    if (count < 1)
                        mult = 1.0f;
                    else
                        mult = *(float*)((char*)G_CDataManager() + 0x10 +
                                         (count + 0xf7) * 4);
                    rate = (int)((float)rate * mult);
                }
            }
            if (isAffectedPremium((ENUM_PREMIUM_TYPE)0x4f))
            {
                unsigned int p =
                    getAddQuestItemDropRateFromPremium(
                        (ENUM_PREMIUM_TYPE)0x4f);
                rate = dropRate * (int)(p / 100) + rate;
            }

            int gained = 0;
            for (int t = 0; t < tryCount; ++t)
                if (get_rand_int(100) < rate)
                    ++gained;
            if (gained == 0)
                break;

            Inven_Item item;
            CItem* ci = G_CDataManager()->find_item(itemIdx);
            ci->make_item(item);
            item.set_add_info(gained);
            if (gained > 999)
            {
                cMyTrace("bool CUser::CheckQuestMonster(int, int, "
                         "ENUM_QUEST_ENEMY_TYPE)", 0x5579, 5)
                    ("CUser::CheckQuestMonster, mon_item.item.set_add_info , "
                     "ITEM #%d, %d , QUEST#%d",
                     itemIdx, gained, questRef->m_quest[i]);
                // 判据㊿：ORIG 0x0866cfdc `movl $0x3e8,-0x40(%ebp)` 写的是 **gained
                //   局部槽**（-0x40，即上方 set_add_info(gained)/cmpl $0x3e7 的同一槽），
                //   非 item 字段；Ghidra C 亦作 `local_44 = 1000;`。原写
                //   item.m_addInfo2 会污染发包内容（put_int(item.get_add_info())）。
                gained = 1000;
            }

            PacketGuard packet;
            InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
            pkt->put_header(0, 0xe);
            pkt->put_byte(0);
            pkt->put_short(1);

            Inven_Item existItem;
            int slot = getCurCharacInvenR()->GetInvenData(itemIdx, existItem);
            if (slot < 0)
            {
                int newSlot = getCurCharacInvenW()->insertItemIntoInventory(
                    item, (eItemAddReason)8, true, true);
                if (newSlot > 0)
                {
                    pkt->put_short(newSlot);
                    pkt->put_int(itemIdx);
                    pkt->put_int(item.get_add_info());
                    pkt->put_byte(item.GetItemAttr());
                    pkt->put_short(item.m_fieldb);
                    pkt->put_byte(item.m_field0);
                    pkt->put_int(item.m_fieldd);
                    pkt->put_byte(item.m_amp.getAbilityType());
                    pkt->put_short(item.m_amp.getAbilityValue());
                    if (item.isAvatarItemType())
                    {
                        pkt->put_int(0x1e);
                        // ORIG 0x0866d1??：socketData / expansion 各自重调
                        //   getCurCharacInvenR()+GetAvatarItemMgrR()（无 mgr 缓存，P183）。
                        const char* socketData =
                            getCurCharacInvenR()->GetAvatarItemMgrR()
                                ->getJewelSocketData(item.get_add_info());
                        pkt->put_binary(socketData, 0x1e);
                        const char* expansion = static_cast<const char*>(
                            getCurCharacInvenR()->GetAvatarItemMgrR()
                                ->GetExpansionInfo(item.get_add_info()));
                        pkt->put_int(4);
                        if (expansion == 0)
                        {
                            stAvatarExpansionInfo_t init4;
                            init4.init();
                            pkt->put_binary(
                                reinterpret_cast<const char*>(&init4), 4);
                        }
                        else
                        {
                            pkt->put_binary(expansion, 4);
                        }
                    }
                    pkt->put_byte(0);
                    pkt->put_packet(g_emptySlot);
                    pkt->finalize(true);
                    Send(packet);
                }
            }
            else
            {
                if (needCount == -1)
                {
                    existItem.set_add_info(existItem.get_add_info() +
                                           item.get_add_info());
                }
                else if (needCount <= existItem.get_add_info())
                {
                    for (std::vector<MonsterKillItem>::iterator it =
                             kills.begin();
                         it != kills.end(); ++it)
                    {
                        if (memcmp(&drop, &*it, 0x20) == 0)
                        {
                            kills.erase(it);
                            break;
                        }
                    }
                    again = true;
                }
                else
                {
                    int cur = existItem.get_add_info();
                    if (cur + item.get_add_info() < needCount)
                        existItem.set_add_info(cur + item.get_add_info());
                    else
                        existItem.m_addInfo2 = needCount;   // ORIG 字段直写（无 set_add_info call）
                }
                getCurCharacInvenW()->update_item((INVEN_TYPE)1, slot,
                                                  existItem);
                pkt->put_short(slot);
                pkt->put_int(itemIdx);
                pkt->put_int(existItem.get_add_info());
                pkt->put_byte(0);
                pkt->put_short(0);
                pkt->put_byte(0);
                pkt->put_int(0);
                pkt->put_byte(existItem.m_amp.getAbilityType());
                pkt->put_short(existItem.m_amp.getAbilityValue());
                if (existItem.isAvatarItemType())
                {
                    pkt->put_int(0x1e);
                    // ORIG 0x0866d3??：同 slot<0 分支，两次独立重调（无 mgr 缓存）。
                    const char* socketData =
                        getCurCharacInvenR()->GetAvatarItemMgrR()
                            ->getJewelSocketData(existItem.get_add_info());
                    pkt->put_binary(socketData, 0x1e);
                    const char* expansion = static_cast<const char*>(
                        getCurCharacInvenR()->GetAvatarItemMgrR()
                            ->GetExpansionInfo(existItem.get_add_info()));
                    pkt->put_int(4);
                    if (expansion == 0)
                    {
                        stAvatarExpansionInfo_t init4;
                        init4.init();
                        pkt->put_binary(
                            reinterpret_cast<const char*>(&init4), 4);
                    }
                    else
                    {
                        pkt->put_binary(expansion, 4);
                    }
                }
                pkt->put_byte(0);
                pkt->put_packet(existItem);
                pkt->finalize(true);
                Send(packet);
            }
        }
    }
