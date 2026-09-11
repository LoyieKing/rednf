// ============================================================================
// CParty::_putItemIntoUser —— ORIG 0x85aab84：双槽奖励发放（items[0] 金币/
// 主物、items[1] 装备），含背包失败邮件退还与幸运点扣减。
// ============================================================================
// §6.2 巨件核销：P167 126/126 [0缺0多]；gap 739→+21（our 760）；残差归因 N82
//（槽基错位）+ N80/N83（cMyTrace 日志段跨版本求值/构造形）。
// 真修 7 处：①put_binary 重载选错 ×3——(char*) 强转选中非 const 版
// put_binary(char*,int)，ORIG 全为 put_binary(const char*,int)（PKc），改 (const char*)
// /删强转走完整链；②金币日志段合并回单 if（误拆双 if 多发一次 get_add_info 条件）；
// ③删 placement new×2（_ZnwjPv/_ZdlPvS_/_Unwind_Resume EXTRA），改 cMyTrace 临时
// 链式（P37）；④删 oldMoney 死局部（EXTRA get_money+1）；⑤金币尾包 put_int(get_money)
// 改 getCurCharacInvenR() 完整链（MISSING R，ORIG 85aae9b）；⑥avatar 段 avMgrR 缓存
// 展开（P183）：getJewelSocketData/GetExpansionInfo 各走 R→GetAvatarItemMgrR 完整链
//（MISSING R/GetAvatarItemMgrR 各 1）；⑦金币尾包补第 4 个 put_byte(0)
//（MISSING put_byte，ORIG 85aaf37，put_short(0) 之后 put_packet 之前）。
// b28d-solo-CParty: N 封顶 —— N82 帧 0x1fc/0x20c；761 vs 739
int CParty::_putItemIntoUser(CUser* user, Inven_Item* items, PacketGuard& packet)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85aab84
    int count = 0;                                      // -0x48
    int pktType = 0;                                    // -0x40
    packet.clear();
    packet.put_header(0, 0xe);
    packet.put_byte(pktType);
    int countIdx = packet.get_index();                  // -0x68
    packet.put_short(count);

    if (items[0].get_add_info() > 0)                    // 金币槽
    {
        if (499999 < items[0].get_add_info())
        {
            // P37 临时链式：ORIG 无 placement new（N83）/无 D1 残留
            cMyTrace(__PRETTY_FUNCTION__, 0x232d, 4)(
                "Dungeon(%d) reward money(%d) on %d' party member with difficulty %d",
                // ORIG _putItemIntoUser.md 085aac58 `mov 0xcb8(%eax)` =
                // CBattle_Field+0x194 → m_dungeonIndex（int）
                m_battleField.m_dungeonIndex, items[0].get_add_info(),
                get_member_count(), m_battleField.get_dungeon_diff());
            // ORIG 085aacc3~085aad47：第二条 cMyTrace(0x2337) 日志
            // "User(%s) Charac(%s) gain money(%d), old money is %d."。
            cMyTrace(__PRETTY_FUNCTION__, 0x2337, 4)(
                "User(%s) Charac(%s) gain money(%d), old money is %d.",
                NumberToString(user->get_acc_id(), 0),
                user->getCurCharacName(), items[0].get_add_info(),
                reinterpret_cast<CUserCharacInfo*>(user)
                    ->getCurCharacInvenR()
                    ->get_money());
        }
        // [G7-sub] ORIG 0x085aad4c..0x085aadc9：`call getCurCharacInvenW; mov %eax,%ebx`
        // + `call get_add_info`（内联实参）+ `mov %ebx,0x4(%esp)` ⇒ receiver 结果全
        // 程在 %ebx、**无 inven 槽**（P52 链式 receiver callee-saved 独占 / P0）。
        // 旧码 `CInventory* inven = …` 落槽 -0x3c（+1 store、每次 +1 load）。
        int gained = reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()
                         ->gain_money(items[0].get_add_info(),
                                      (eMoneyAddReason)5, true, false);
        if (gained < items[0].get_add_info())
            user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0,
                                      (unsigned long)items[0].get_add_info(),
                                      (unsigned long)gained);
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)3, user, (unsigned int)gained);
        if (m_battleField.m_pDungeon != 0)
        {
            // ORIG _putItemIntoUser.md 085aae02 `movzbl 0x89e(%eax)` =
            // CDungeon::m_89e（unsigned char，金币流水分类标志）
            if (m_battleField.m_pDungeon->m_89e == 0)
                GetInstanceCirculationStatistic()->AddCirculationStatistic(
                    (CIRCULATION_STATISTIC_FIELD)1, user,
                    (unsigned int)gained);
            else
                GetInstanceCirculationStatistic()->AddCirculationStatistic(
                    (CIRCULATION_STATISTIC_FIELD)0xf, user,
                    (unsigned int)gained);
        }
        user->saveTaxMoneyForUpperMember(items[0].get_add_info());

        packet.put_short(0);
        packet.put_int(0);
        packet.put_int(reinterpret_cast<CUserCharacInfo*>(user)
                           ->getCurCharacInvenR()
                           ->get_money());
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_packet(g_emptySlot);
        ++count;
    }

    // P0：ORIG 085aaf5f/085aafeb `GetItemSpace(param_2+0x3d)` 两处独立调用，
    // 不缓存 space 局部（条件比较一次 + 重初始化 put_byte 一次）。
    if (pktType != items[1].GetItemSpace())
    {
        if (0 < count)
        {
            packet.put_short(countIdx, count);
            packet.finalize(true);
            user->Send(packet);
        }
        count = 0;
        packet.clear();
        packet.put_header(0, 0xe);
        packet.put_byte(items[1].GetItemSpace());
        countIdx = packet.get_index();
        packet.put_short(count);
    }

    CUserCharacInfo* ci = reinterpret_cast<CUserCharacInfo*>(user);
    int slot = ci->getCurCharacInvenW()->insertItemIntoInventory(items[1],
                                                                (eItemAddReason)0xb,
                                                                true, true);
    bool isTournament = m_battleField.m_pDungeon != 0 && m_battleField.m_pDungeon->isTournamentDungeon() != 0;
    if (slot < 1)
    {
        // P167：ORIG 两分支各一次完整邮件块（findString-155 + find_item +
        // ReqDBSendNewSystemMail；以及 findString-250 + find_item + ReqDBSend），
        // 计数 2/2/2 —— 拆开写而非 senderKey 合并。
        // [P329/P183/P0 去具名局部] ORIG 0x085aae5x 段为 `…; call *%edx;
        // test %al,%al; je <T>`（IsEventing 虚调结果**直测**、无 ev41/ev41ing 槽）
        // + 短路链物化 `mov $0x0,%eax; test %al,%al`（**无 bMail155 byte 槽**）
        // ⇒ 原码是单条全表达式 if，中间量不具名。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x41)->IsEventing(0) &&
            items[1].m_addInfo != 0 && items[1].m_addInfo != -1 &&
            !isTournament &&
            G_CDataManager()->find_item(items[1].m_addInfo) != 0)
        {
            char title[21];                         // -0x89（msg_155 活动线）
            strncpy(title, g_scriptStringManager_.findString(
                               4, "game_server_msg_155", 0),
                    0x14);
            char bodyText[256];                     // -0x19e
            strncpy(bodyText,
                    g_scriptStringManager_.findString(4, "game_server_msg_104", 0),
                    0xff);
            unsigned int serverGroup =
                (unsigned int)user->GetServerGroup();
            unsigned int characNo =
                (unsigned int)ci->getCurCharacNo();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, items[1], 0, characNo, bodyText,
                (int)strlen(bodyText), 8,
                (ENUM_SERVER_GROUP)serverGroup, false, false);
        }
        else if (items[1].m_addInfo != 0 && items[1].m_addInfo != -1 &&
                 isTournament &&
                 G_CDataManager()->find_item(items[1].m_addInfo) != 0)
        {
            char title[21];                         // -0x9a（msg_250 锦标赛线）
            strncpy(title, g_scriptStringManager_.findString(
                               4, "game_server_msg_250", 0),
                    0x14);
            char bodyText[256];
            strncpy(bodyText,
                    g_scriptStringManager_.findString(4, "game_server_msg_104", 0),
                    0xff);
            unsigned int serverGroup =
                (unsigned int)user->GetServerGroup();
            unsigned int characNo =
                (unsigned int)ci->getCurCharacNo();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, items[1], 0, characNo, bodyText,
                (int)strlen(bodyText), 8,
                (ENUM_SERVER_GROUP)serverGroup, false, false);
        }
    }
    else
    {
        packet.put_short((short)slot);
        const Inven_Item* ref =
            ci->getCurCharacInvenR()->GetInvenRef(
                items[1].GetInventoryType(), slot);
        packet.put_int(*reinterpret_cast<const int*>(reinterpret_cast<const char*>(ref) + 2));
        packet.put_int(ref->get_add_info());
        packet.put_byte((unsigned int)ref->GetItemAttr() & 0xff);
        packet.put_short((unsigned int)ref->m_fieldb & 0xffff);
        packet.put_byte((unsigned int)(unsigned char)*reinterpret_cast<const unsigned char*>(ref));
        packet.put_int(*(const int*)&ref->m_fieldd);
        packet.put_byte((unsigned int)ref->m_amp.getAbilityType() & 0xff);
        packet.put_short((unsigned int)ref->m_amp.getAbilityValue() & 0xffff);
        if (ref->isAvatarItemType())
        {
            packet.put_int(0x1e);
            packet.put_binary(
                ci->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                    ref->get_add_info()),
                0x1e);
            void* expInfo = ci->getCurCharacInvenR()
                                ->GetAvatarItemMgrR()
                                ->GetExpansionInfo(ref->get_add_info());
            packet.put_int(4);
            if (expInfo == 0)
            {
// b28d-solo-CParty: DIFF 残帧/槽 —— N82：4.4 对 unsigned 除法多被除数副本槽 -0x2c、bool 形参 spill 槽 -0xc/-0x1c 与 ORIG 反向（已试 int/(unsigned) 两形）；90 vs 88
                stAvatarExpansionInfo_t init0;
                init0.init();
                packet.put_binary((const char*)&init0, 4);
            }
            else
                packet.put_binary((const char*)expInfo, 4);
        }
        packet.put_byte(0);
        packet.put_packet(*ref);
        ++count;

        CItem* card = G_CDataManager()->find_item(items[1].m_addInfo);
        if (card != 0)
        {
            int sellPrice = 0;
            G_Store()->GetSellItemPrice(items[1], card,
                                        (short)items[1].m_addInfo2, false,
                                        sellPrice);
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)4, user, (unsigned int)sellPrice);
        }
        // ORIG 越界读 items[1]+0x3e（Inven_Item 0x3d 后 1 字节；线格式遗留，保留）
        if ((unsigned char) *reinterpret_cast<unsigned char*>(
                reinterpret_cast<char*>(&items[1]) + 0x3e) ==
                1 &&
            card != 0)
        {
            CLuckPoint::UseLuckPoint(user, (int)(unsigned char) * reinterpret_cast<unsigned char*>(
                                         reinterpret_cast<char*>(&items[1]) + 0x3e),
                                     card->get_rarity(), card->get_grade());
        }
    }

    if (0 < count)
    {
        packet.put_short(countIdx, count);
        packet.finalize(true);
        user->Send(packet);
    }
    return count;
}