void CParty::MakeQuestRewardPacket(PacketGuard* packet, CUser* user)
{
    int count = 0;
    int needCount = 0;   // ORIG 0x085bd7c0 头部预清（-0x24）
    int countIndex = packet->get_index();
    packet->put_byte(0);
    for (int q = 0; q < 0x14; ++q)
    {
        // [b28d-solo-CPartyDepsD2] P183：ORIG 0x085bd7f4 每轮重调
        // user->getCurCharacQuestR()（-0x1c 槽），无循环外缓存。
        // ORIG 0x085bd8xx 用 getCurCharacQuestRefR（21 字 mangled，与
        // checkMoveMapHackCnt 的 18 字 getCurCharacQuestR 不同重载）。
        UserQuest* questRef = user->getCurCharacQuestRefR();
        if (questRef->m_quest[q] == 0 || questRef->m_trigger[q] == 0)
            continue;
        Quest* quest = G_CDataManager()->find_quest(questRef->m_quest[q]);
        if (quest == 0)
            continue;
        // +0x75da = UserQuest.m_authenData(+0x75d8) + 2 = m_characClass（CUser.h:1220）
        // P183：ORIG 0x085bd878/0x085bd8e3 两次独立 call get_dungeon_index
        //（无缓存槽，各自直传实参）。
        // [b28f-CTS P200] ORIG 0x085bd86x 为 `movswl %ax,%ebx`（短整表达式作
        //   普通实参直传、2 操作数扩展到 callee-saved）；原码无具名 short 局部
        //   —— 具名形退化为 `mov %ax,-0x16(%ebp); movswl -0x16(%ebp),%ebx`
        //   并多占 4 字节帧槽（全函数槽号整体深 4）。内联直传复现 ORIG。
        if (!quest->check_clear_item(bf(this).get_dungeon_index(),
                                     questRef->m_authenData.m_characClass))
            continue;
        needCount = questRef->m_quest[q];
        if (needCount <= 0)
            continue;
        // ORIG 0x085bd8b8/-0x4c 循环外默认构造 + 拷贝赋值（N86 反向）；
        // 0x085bd8ce quest==0 死块照抄（P184）。
        std::vector<std::pair<int, int> > itemList;
        std::pair<int, int> pairValue;
        if (quest == 0)
            continue;
        itemList = quest->get_clear_itemlist(bf(this).get_dungeon_index());
        // [b28f-CTS 轴B] ORIG 循环尾为 `cmp -0x..(%ebp),%eax; setg`（**有符号**序，
        //   见 0x085bd9xx）⇒ 原码循环变量是 int；`size_t i < size()` 让 4.4 产
        //   无符号序 `seta`（符号性分叉）。改 int + 显式 `(int)` 收窄复现 ORIG
        //   （P196 前置已核：ORIG 是 setg 而非 seta）。at() 实参 32 位下零转换开销。
        for (int i = 0; i < (int)itemList.size(); ++i)
        {
            Inven_Item item;
            item.reset();
            // ORIG 0x085bd972 单次 vector::at（P175：ORIG 走 at 非 operator[]）
            // 后 pair 拷贝到 -0x4c/-0x48。
            pairValue = itemList.at(i);
            item.m_addInfo = pairValue.first;
            // ORIG 0x085bd98e `mov 0xc(%eax),%eax` 直接成员访问（无 getItemList call）
            G_CDataManager()->m_itemList->create_item(
                ENUM_ITEM_CREATE_TYPE_1, item, pairValue.second);
            int slot = (int)user->getCurCharacInvenW()->insertItemIntoInventory(
                item, (eItemAddReason)0xb, true, true);
            if (slot < 0)
                continue;
            packet->put_short(slot);
            packet->put_int(item.m_addInfo);
            packet->put_int(item.get_add_info());
            packet->put_short(0);
            packet->put_byte((int)item.m_amp.getAbilityType());
            packet->put_short((int)item.m_amp.getAbilityValue());
            ++count;
            if ((item.m_addInfo >> 16) == 1)
            {
                CItem* card = G_CDataManager()->find_item(item.m_addInfo);
                if (card != 0)
                    CLuckPoint::UseLuckPoint(user, (int)(item.m_addInfo >> 16),
                                             card->get_rarity(),
                                             card->get_grade());
            }
        }
    }
    if (count != 0)
        packet->put_byte(countIndex, count);
