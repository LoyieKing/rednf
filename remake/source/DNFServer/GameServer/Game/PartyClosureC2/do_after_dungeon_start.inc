// ===========================================================================
// CParty::do_after_dungeon_start —— T 0x0859fcfc
// 报告：do_after_dungeon_start.md（活动路径；死代码块见文件头注记）。
// 错误码：0xad 虚检/维度券缺失、0x11 必需道具缺失或复仇票不足、0x15 非开放
// 定时副本；成功 0。血腥/维度/终极/锦标赛入场扣减与事件邀请删除均在主路径。
// ===========================================================================
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
    // [M2S4 2026-09-11] 687（ORIG 686）。本波：具名 `const int level` 内联回
    //   GetPriceAverage 实参位（P0/P329：ORIG 0x085a0115 `mov %eax,%ebx` 跨
    //   G_CDataManager 保活、无槽）⇒ 687→686；trace 两处改单语句 `cMyTrace(...)(...)`
    //   （P329，ORIG 0x085a01xx/8xx 段 ctor 在 getCurCharacNo 之后；改后该两段与 ORIG
    //   逐条同序，代价 +1 条，净 687）。
    //   残余差异族（封 N，逐块记录）：
    //   ① ORIG `cmpl $-1; sete %al; test %al,%al; je`（0x085a0655/6bb/6ff 三处
    //      deadMissing）——4.1 独有 4 条形，4.4 无对应写法（探针 /tmp/m2s4_sete.cpp）；
    //   ② P337 恒 0 volatile 守卫（0x085a042c 段）：ORIG 入口为单条 `jmp 085a04c3`，
    //      我方 4 条（movb/movzbl/test/je）——4.4 会 DCE `if(0)`/goto，守卫是必要代价；
    //   ③ EH 布局：ORIG 正常路径 dtor（0x085a0194..19a）在 handler（0x085a01a1..1b7）
    //      之前，4.4 恒把正常 dtor 排在 handler 之后（条数同，顺序反）；
    //   另有已修两处块布局（P222，本波实证）：
    //   ④ 村庄复仇/必需道具两段物理序：ORIG 0x085a0585..91 `test %al,%al; je 085a06e1`
    //      ⇒ then（村庄复仇）后置在 0x085a06e1..0742、else（必需道具段）前置；
    //      原「`if (flag==0) {…}` + 后续必需道具」让 4.4 产 then 前置（`jne` 跳过），
    //      两段 61 条整体颠倒 ⇒ 改写 `if (flag != 0) {必需道具段} else {村庄复仇}` 后
    //      与 ORIG 逐条同序（该段尾部 idx 646..686 已逐条对齐）。
    //   ⑤ `revengeCount == 0 || m_maxRevengeCount < revengeCount` 短路落点：
    //      ORIG 0x085a0701 `je 085a072a` / 0x085a0710 `jb 085a072a` 共落**后置**失败块
    //      ⇒ 取反写成 `if (revengeCount != 0 && max >= revengeCount) {成功} else {失败}`
    //      后同构。两处 ④⑤ 使非槽差异块 19→9（其中 3 块为 .rodata 重定位文本假差异）。
    //   净 +1 条来自 ①（每处 deadMissing 比 ORIG 多 1 条，×2）+ ②（守卫比 ORIG 多 3）
    //   被 EH 布局族抵消后的残差；已按 §6.4 取证确认无法在不引入语义/调用差集退化的
    //   前提下精确配平（试过并回滚：`if (slot == -1)` 全函数 687→681，离 ORIG 更远）。
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
        // [P0/P329] ORIG 0x085a0115 `mov %eax,%ebx`（get_charac_level 结果经 %ebx
        // 跨 G_CDataManager 调用保活，无具名槽）⇒ 原码把实参内联在调用位；
        // 具名 level 让 4.4 多 store/load 各一条（gap +1 的一个来源）。
        const int avgPrice =
            G_CDataManager()->m_serverParameter.GetPriceAverage(
                user->get_charac_level());
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
        // [M2S4/P0] ORIG 0x085a0216/0x085a0218 `test %al,%al; jg`（无槽）⇒ 原码把
        // 该元素读内联在条件位；具名 ticketLeft 让 4.4 多 emit
        // `mov %al,-0x21; cmpb $0x0,-0x21`（+1 条，gap +1 的最后来源）。
        if ((signed char)user->getCurCharacR()->m_demensionInout[dimIdx] < 1 &&
            m_battleField.m_bloodState != 1 && !user->isGMUser())
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
                // [M2S4 负结论] ORIG +0x683 `cmpl $-1; sete %al; test %al,%al; je`
                // 为 4.1 独有形（bool 值留 %al 不落槽）。试过 `if (slot == -1)`：
                // 4.4 恒产 `cmpl;jne`（2 条，比 ORIG 少 2）且整函数 687→681（−5，
                // 离 ORIG 686 更远、非槽差异块数 9→9 不变）⇒ 回滚保留具名 bool
                // （5 条形，多 1 条）。判据见 §6.4：our>ORIG 才走消指令方向。
                const bool deadMissing = (slot == -1);            // ORIG +0x683
                if (deadMissing)
                    return 0x11;
            }
            else
            {
                slot = user->getCurCharacInvenR()->check_item_exist(
                    dungeon->m_entryRequiredItem.m_itemIdx);      // ORIG +0x698..+0x6b3
                const bool deadMissing = (slot == -1);            // ORIG +0x6bb（同上）
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
                // [P329] 同 checkndelete_item：ORIG +0x762 段是单语句形（ctor 在
                //   getCurCharacNo 之后，见 0x085a01xx `mov %eax,%ebx` 中转）。
                cMyTrace(__PRETTY_FUNCTION__, 0xd66, 5)(
                    "CParty::UseAncientDungeonItems, delete_item failed , User ch=%d , %d %d",
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

    // ---- 村庄复仇 / 定时事件副本必需道具 ----
    // [M2S4/P222] ORIG 0x085a0585..0x8591 `movzbl 0x85c(%eax),%eax; test %al,%al;
    //   je 085a06e1` —— then（村庄复仇）块**物理后置**在 0x085a06e1..0742，else
    //   （必需道具段 0x085a0597..06dc）前置，尾 `mov $0x0,%ebx` 共享出口。
    //   等价重写为 `if (flag != 0) {必需道具段} else {村庄复仇}` ⇒ 4.4 的
    //   if/else 布局（A 前置 / B 后置 + `je <B>`）与 ORIG 逐条同构；
    //   原「then 前置」写法（`jne` 跳过）使两段物理序整体颠倒 61 条。
    if (dungeon->m_entryRestrictFlag != 0)
    {
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
    }
    else
    {
        if (dungeon->m_villageAttackEventFlag != 0)
        {
            const unsigned char revengeCount = user->GetCurRevengeDungeonCount();
            // [M2S4/P222] ORIG 0x085a0701 `je 085a072a` 与 0x085a0710
            //   `jb 085a072a` 两条跳转共落**后置**失败块（0x085a072a），
            //   fall-through 为成功块 ⇒ 条件取反写成 if/else（成功前置/失败后置）。
            if (revengeCount != 0 &&
                (unsigned char)dungeon->m_maxRevengeCount >= revengeCount)
            {
                user->DecreaseRevengeDungeonTicket();
                user->VillageAttack_DBUpdate();
            }
            else
            {
                user->SetCurRevengeDungeonCount(0);
                return 0x11;
            }
        }
        return 0;
    }
}