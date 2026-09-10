int CParty::do_after_dungeon_start(const CDungeon* dungeon, CUser* user)
{
    // [b28d-solo-PartyClosureC2] ORIG 0x0859fcfc：
    //   P168a bool callee 裸形（ORIG `call *%edx; xor $0x1,%eax; test; je`）；
    //   P214③/P312 `char msg[0x100] = {0}` 复现 4.1 内联 rep stos 慢路（N78）；
    //   P0 fmt/strlen 内联表达式（ORIG 无具名 fmt 槽）；
    //   P167 血腥段补真实调用族 GetCharacExpandData×3 + GetEnterCount/IncEnterCount/
    //   IncPoint/IncUseGold（原 BloodDungeonCountsView 裸字段视图缺 7 个调用点）；
    //   P133/P196 维度券三条件独立物化 + 必需道具段顺序 if + 提前 return（Ghidra C 实证）。
    //   P337 死块 +0x654..+0x7c2（vilmark/check_item/GetInvenSlot/delete_item/
    //   cMyTrace）已用恒 0 volatile 守卫复现：9 个调用点全部发射、运行期不可达。
    //   现读数：686 vs 687（gap −1）、p167 91 vs 91（余差仅 PacketGuard/cMyTrace
    //   C1↔C2/D1↔D2 别名对）、帧 0x2cc 与 ORIG 一致（msg 槽 -0x1b5 vs ORIG
    //   -0x1a9 属 N82 槽阶梯残余）；_do_after_dungeon_start 返回 int→bool 需
    //   CDungeon.h:665/849 + CDungeon.cpp:584/664 跨 TU 同步（已上报 Main）。
    // 虚表槽0：CDungeon::_do_after_dungeon_start(user)
    if (!dungeon->_do_after_dungeon_start(user))
        return 0xad;

    user->setCurCharacLastPlayDungeonIndex(dungeon->get_index());

    // ---- 血腥副本入场（dungeon->m_bloodDungeonType == 1） ----
    if (dungeon->m_bloodDungeonType == 1)
    {
        if (m_battleField.m_bloodState == 1)
        {
            // 提示串：findString(表4,"game_server_msg_110") 作为 snprintf 格式串，
            // 单人模式费用参数为 0
            char msg[0x100] = {0};
            OS_API::snprintf(
                msg, 0xff,
                g_scriptStringManager_.findString(4, "game_server_msg_110", 0), 0);
            PacketGuard packet;
            packet.put_header(0, 0xc);
            packet.put_byte(0);
            packet.put_short(0);
            packet.put_byte(0);
            packet.put_int((int)strlen(msg));
            packet.put_str(msg, (int)strlen(msg));
            packet.finalize(true);
            user->Send(packet);
            return 0;
        }

        const int fee = get_blood_dungeon_admission_fee(user);
        user->getCurCharacInvenW()->use_money(fee, (eMoneySubReason)0x22, true);
        user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 0);

        char msg[0x100] = {0};
        OS_API::snprintf(
            msg, 0xff,
            g_scriptStringManager_.findString(4, "game_server_msg_110", 0), fee);
        PacketGuard packet;
        packet.put_header(0, 0xc);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int((int)strlen(msg));
        packet.put_str(msg, (int)strlen(msg));
        packet.finalize(true);
        user->Send(packet);

        // 血腥扩展数据计数（P167：ORIG 全走真实方法调用，各点独立
        // GetCharacExpandData(1) 重取，Inc* 内部自带 alter()）
        int enterCount = reinterpret_cast<CCharacBloodDungeon*>(
            const_cast<CExpandEquipslot*>(
                user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1)))
            ->GetEnterCount();
        const int lottePoint = G_CDataManager()
                                   ->m_serverParameter.GetLottePoint(enterCount);
        const int level = user->get_charac_level();
        const int avgPrice =
            G_CDataManager()->m_serverParameter.GetPriceAverage(level);
        reinterpret_cast<CCharacBloodDungeon*>(const_cast<CExpandEquipslot*>(
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1)))
            ->IncEnterCount();
        reinterpret_cast<CCharacBloodDungeon*>(const_cast<CExpandEquipslot*>(
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1)))
            ->IncPoint(lottePoint);
        reinterpret_cast<CCharacBloodDungeon*>(const_cast<CExpandEquipslot*>(
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1)))
            ->IncUseGold(fee - avgPrice);
    }

    // ---- 维度副本入场券（dimension possible>0） ----
    if ((signed char)dungeon->get_dimension_possible() > 0)
    {
        if (m_battleField.m_bloodState == 1)
            return 0;
        const int dimIdx = (signed char)dungeon->get_dimension_possible() - 1;
        // P311：具名成员数组 m_demensionInout[dimIdx] 复现单条
        // `movzbl 0xeb9(%eax,%edx,1)`（裸偏移恒 add 链）。
        const signed char ticketLeft =
            (signed char)user->getCurCharacR()->m_demensionInout[dimIdx];
        if (ticketLeft < 1 && m_battleField.m_bloodState != 1 && !user->isGMUser())
            return 0xad;
        if (m_battleField.m_bloodState != 1)
        {
            user->subDemensionInoutValue((char)dimIdx);
            G_GameWorld()->send_party_info_to_all(this, 1);
        }
    }

    // ---- 终极副本（m_bloodDungeonType==2） ----
    if (dungeon->m_bloodDungeonType == 2 && m_battleField.m_bloodState != 1)
        user->subUltimateInoutValue(2);

    // ---- 锦标赛入场包（0xe） ----
    if (dungeon->m_89c != 0 || dungeon->isTournamentDungeon())
    {
        if (m_battleField.m_bloodState == 1)
            return 0;
        if (dungeon->m_entryRequiredItem.m_itemIdx == -1)
            return 0;
        if (dungeon->m_entryRequiredItem.m_enableFlag != 1)
            return 0;

        int slot = 0;                                // ORIG -0x24（+0x642 movl $0x0）
        // [P337 改写] 旧判词「死块不可复现」被 P337 推翻：恒 0 **volatile** 守卫的
        // 读取不可省略 ⇒ 块体与 ORIG 9 个调用点强制发射（`if(0)`/goto 才会 DCE）。
        // ORIG +0x64f 为 e9 无条件 jmp 直达 +0x7c7，死块 +0x654..+0x7c2 无入口；
        // 运行期不可达（与本守卫等价），活动路径语义零变化。
        volatile bool neverEnter = false;
        if (neverEnter)
        {
            // ORIG +0x654 直接比较进入死块时的 eax（= dungeon 指针；32 位恒 ≠ 0x5dc）
            if ((int)reinterpret_cast<size_t>(dungeon) == 0x5dc)
            {
                slot = user->getCurCharacInvenR()->check_vilmark_dungeon_item_exist(
                    dungeon->m_entryRequiredItem.m_itemIdx);      // ORIG +0x660..+0x67b
                const bool deadMissing = (slot == -1);            // ORIG +0x683
                if (deadMissing)
                    return 0x11;
            }
            else
            {
                slot = user->getCurCharacInvenR()->check_item_exist(
                    dungeon->m_entryRequiredItem.m_itemIdx);      // ORIG +0x698..+0x6b3
                const bool deadMissing = (slot == -1);            // ORIG +0x6bb
                if (deadMissing)
                    return 0x11;
            }
            const Inven_Item deadItem =                           // ORIG +0x6d0..+0x6fc
                user->getCurCharacInvenR()->GetInvenSlot(1, slot);
            if (deadItem.m_addInfo2 < dungeon->m_entryRequiredItem.m_itemCount)
                return 0x11;                                      // ORIG +0x6ff..+0x717
            if (!user->getCurCharacInvenW()->delete_item(          // ORIG +0x71c..+0x760
                    (INVEN_TYPE)1, slot, dungeon->m_entryRequiredItem.m_itemCount,
                    (eItemDelReason)0xe, true))
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0xd66, 5);     // ORIG +0x762..+0x796
                trace("CParty::UseAncientDungeonItems, delete_item failed , User ch=%d , %d %d",
                      user->getCurCharacNo(), slot,
                      dungeon->m_entryRequiredItem.m_itemCount);   // ORIG +0x79b..+0x7b8
                return 0x11;
            }
        }
        PacketGuard packet;
        packet.put_header(0, 0xe);
        packet.put_byte(0);
        packet.put_short(1);
        user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, slot, packet);
        packet.finalize(true);
        user->Send(packet);
    }

    // ---- 村庄复仇 / 定时事件副本必需道具（ORIG 否定式早返 + 共享尾） ----
    if (dungeon->m_entryRestrictFlag == 0)
    {
        if (dungeon->m_villageAttackEventFlag != 0)
        {
            const unsigned char revengeCount = user->GetCurRevengeDungeonCount();
            if (revengeCount == 0 ||
                (unsigned char)dungeon->m_maxRevengeCount < revengeCount)
            {
                user->SetCurRevengeDungeonCount(0);
                return 0x11;
            }
            user->DecreaseRevengeDungeonTicket();
            user->VillageAttack_DBUpdate();
        }
        return 0;
    }
    if (!dungeon->IsOpenScheduleDungeon())
        return 0x15;
    if (user == 0)
        return 0;
    if (dungeon->m_entryRequiredItem2.m_itemIdx == -1 &&
        dungeon->m_entryRequiredItem.m_itemIdx == -1)
        return 0;
    if (dungeon->m_entryRequiredItem2.m_itemIdx != -1 &&
        dungeon->m_entryRequiredItem2.m_enableFlag != 1)
        return 0;
    if (dungeon->m_entryRequiredItem.m_itemIdx != -1 &&
        dungeon->m_entryRequiredItem.m_enableFlag != 1)
        return 0;

    // ORIG +0x93f..+0x9e3：req2 命中且 check 返回 0 时立即 SetInvitation 并
    // 提前返回；否则顺序进入 req1 检查（非 if/else 互斥）。
    int resultCode = 0x11;
    if (dungeon->m_entryRequiredItem2.m_itemIdx != -1)
    {
        resultCode = do_after_dungeon_start_checkndelete_item(
            dungeon->m_entryRequiredItem2, user);
        if (resultCode == 0)
        {
            user->SetDeleteEventDungeonInvitation(true);
            return resultCode;
        }
    }
    if (dungeon->m_entryRequiredItem.m_itemIdx != -1)
    {
        resultCode = do_after_dungeon_start_checkndelete_item(
            dungeon->m_entryRequiredItem, user);
        if (resultCode == 0)
            user->SetDeleteEventDungeonInvitation(true);
    }
    return resultCode;
