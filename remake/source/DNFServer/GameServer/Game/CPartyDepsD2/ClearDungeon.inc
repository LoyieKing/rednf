// ---------------------------------------------------------------------------
// CParty::ClearDungeon —— ORIG T 0x085a9330（报告 ClearDungeon.md 逐段转写）：
//   清图流程：普通/绝望塔/巡回分支 → 动作点/统计/奖励 → 时间与地狱值结算。
// ---------------------------------------------------------------------------
// [b28d-SweepCPartyDepsD2-2] 真 bug 修复（虚调用槽取错，Main 全库排查判据命中）：
//   ORIG 0x085a9b6e `add $0x8,%edx` = vptr+0x08 = _ZTVN8WongWork14CItemGeneratorE
//   word[4] @0x08c9a798 = 0x0853a400 = CItemGenerator::generateRandomItem
//   （Ghidra C `(**(code **)(*piVar14 + 8))(piVar14, ref, result)`；同 vtable
//   word[5]=vptr+0x0c=0x0853a406=generateSpecificItem）。旧码调
//   generateSpecificItem ⇒ 运行时选错虚函数。改后本 TU 唯一 `call *` 槽号
//   与 ORIG 逐条一致（本 TU 全量 grep 仅此 1 处间接调用）。
// [b28d-solo-CPartyDepsD2] 判词 N82（1058 vs 1060，§6.2 三件套闭合）：已修
// P167 缺口 ①getManager() 由具名缓存改 4 处逐点重调（P183）②m_partyIndex 改
// GetPartyIndex() 3 处（0x2c/0x22/0x15 timer）③itemList[i].second 改
// item.m_addInfo2 字段直读（消 get_add_info call）④push_back 改传 m_todPos
// 引用（消 pair<int&,int&> 临时 + push_back(&&) 重载）。p167 149/149 CLEAR
// （0 缺 0 多）；残余：帧 0x170 vs 0x1d0 全帧槽平移 + 多分支槽复用方向差
// （N82 巨件族）。语义/调用集齐，byte 层封顶。
// [b28d-round1-L24] 本轮真修（1058→1055→1037，ORIG 1054；gap +4→+1→-17）：
// ①**块序反转**（P222/P272）：`if (m_bloodState == 1) { A; return; } B` —— ORIG
//   0x085a9356 `cmp $1; jne <B(+0x1b0)>` 且 A 尾 `jmp <epilogue>`（0x85a94bd）
//   ⇒ then=A 直落、B 在后；旧码写 `if (!= 1) { B }` 使 4.4 反向。
// ②**64 位 union 直算**（P180/P246）：ORIG 0x085a94e0..0x085a9525 无具名局部，
//   `sub/sbb` 直对 +0x20/+0x24 与 +0x2c/+0x30、双字直存 +0x32c/+0x330 ⇒ 改用
//   BattleData.h 新增 `m_clearPlayedTime`/`m_clearEnterTime` 视角（旧手写
//   `<<32|` 拼接产 40 条 sar/or 展开，净 -39）。
// ③**P183 去 CUser* 缓存**：ORIG 0x085a9589/0x85a95e0/... 六个调用点各自
//   重算 `m_member[slot].m_pUser`（9 条 stride 0x18 寻址）⇒ 净 +38。块序交换
//   （blood 分支 `>= 1` 前置）实测零 codegen 影响（4.4 if/else 两臂同形）。
// 残余（-17）：tower 大段（ORIG[318:782]）与我方发射位置差 ⇒ difflib 大段错位；
//   p167 149/149 CLEAR（0 缺 0 多）；帧 0x170 vs 0x1bc（我方多 push %edi + 槽集大）
//   ⇒ N82/N95 巨件族。语义/调用集齐。
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
        // [Q2P342ScanB] 原手写串 → prett（ORIG `_ZZN6CParty12ClearDungeonEvE19__PRETTY_FUNCTION__` size 0x1c）
        m_battleField.m_dungeonClearTracer.Trace(__PRETTY_FUNCTION__);
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
        // [R13a18 形态复刻] ORIG 0x085a9545 `movl $0x64,-0x3c(%ebp)`：该具名局部
        // （初值 100）在全函数内**从不被读**（ORIG 反汇编 -0x3c 唯一引用点即该写），
        // 属原码遗留死写；按 P171/P348「死写入/未初始化读照抄」口径复刻（ORIG 该
        // 指令位于本 if 体内、for 之前，故声明位置同）。
        int deadLocal100 = 100;
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
                    // [R5a18 ④ #27 有符号比较] ORIG 0x085a9675/0x085a967c 段 `call
                    // CDungeon::get_dimension_possible; test %al,%al; setg %al` ⇒
                    // 比较是**有符号**视角（头声明 unsigned char，与 0 的无符号比较
                    // 在 4.4 恒折 setne ⇒ 值 ≥0x80 时分叉）。同 TU 既有正确形见
                    // CPartyDepsD2/CheckEnterDimensionDungeon.inc:15 与
                    // CPartyDepsB2/checkInoutConditionDungeon.inc:76。
                    if ((signed char)m_battleField.m_pDungeon->get_dimension_possible() > 0)
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
    // [R4a08 SEMANTIC-DIFF 修复] ORIG 0x085a973b/0x085a9741/0x085a9748/0x085a974a 段：`movzbl 0x89f(%eax),%eax;
    //   test %al,%al; jle <skip>` = **有符号**比较 `(signed char)field > 0`
    //   （字段值 >=0x80 视为负 ⇒ 跳过 BloodRound）。原写 `>= 1` 在 4.4 产
    //   `je`（==0 语义），对 >=0x80 的值分叉 ⇒ 按 ORIG 有符号语义修正
    //   （P196 同宽字节符号序；同 TU 先例 Dungeon_check_error.inc:34）。
    if ((int)(signed char)m_battleField.m_pDungeon->m_bloodDungeonType > 0)
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
            // [G07 2026-09-11 · 值级缺口修复] ORIG 085a9b1b `movb $0x46,-0x118(%ebp)`
            //   与 085a9b14 `movb $0x0,-0x117(%ebp)`：ref 基址 = -0x14c（085a9aea
            //   `lea -0x14c(%ebp); call stGenerateRefData_t::C1`）⇒ 二者 = ref+0x34 /
            //   ref+0x35。**我方旧码把 +0x34 写成 0，ORIG 写 0x46(70)** ⇒ strict 复核
            //   我方整函数 0x46 出现 0 次（值级缺口，非 codegen）。
            //   语义（同字段其它调用点实证）：+0x34 = 等级基准值 ——
            //   CBattle_Field_KillChain/_MakeDropItems.inc `ref.m_field34 = (char)mob.m_level`；
            //   GmCmds_B/execute_10.inc `ref.m_field34 = (char)standardLevel`；
            //   MonsterIndependentDrop 以 `(u8)ref.m_field3f - (u8)ref.m_field34` 作掉落等级差。
            //   ⇒ 本处为 TOD 奖励生成的**固定等级 70**（原码即字面量：ORIG 为 `movb $imm`
            //     而不是变量装载；本库同区已用字面量 0x4e4/0xcf2，风格一致）。
            ref.m_field34 = (char)70;
            ref.m_field35 = 0;
            // [R12a15 ④ 修复 · §18-A 结构字段写照抄] ORIG 0x085a9aea `lea -0x14c(%ebp);
            //   call stGenerateRefData_t::C1` ⇒ ref 基址 -0x14c；该段对 ref 的字段写
            //   共 5 处（连续引用，无跳条）：
            //     0x085a9b1b `movb $0x46,-0x118(%ebp)` = ref+0x34 = 70
            //     0x085a9b14 `movb $0x0,-0x117(%ebp)`  = ref+0x35 = 0
            //     0x085a9b22 `mov $0x3f800000,%eax; mov %eax,-0x114(%ebp)` = ref+0x38 = 1.0f
            //     0x085a9b3c `movl $0x0,-0x110(%ebp)`  = ref+0x3c = 0
            //     0x085a9b2d/36 `mov 0x8(%ebp),%eax; mov 0xcac(%eax),%eax; mov %eax,
            //                   -0x10c(%ebp)`      = ref+0x40 = m_battleField.m_pDungeon
            //   ORIG 在 ref 内无 +0x00/+0x1c/+0x36 的写（C1 负责 +0x00=100）；
            //   旧码把 1.0f 落 m_field1c(+0x1c) 并多写 m_rateSlot(+0x36)/m_dropRate(+0x00)，
            //   且缺 +0x38/+0x3c/+0x40 ⇒ 送 generateRandomItem 的 ref 与 ORIG 不等（④）。
            ref.m_rankBonusRate = 1.0f;             // +0x38
            ref.m_field3c = 0;                      // +0x3c
            ref.m_dungeon = bf(this).m_pDungeon;    // +0x40
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
                // R17a18 ④ 修复：ORIG 0x085a9c35..0x085a9c42 为「直调 is_stackable（无判空）
                // + 可堆叠侧 fall-through 到 m_addInfo2、je 落 second=1 岛」；我方原多出
                // `card != 0 &&` 合取项 ⇒ 按用户裁定移除（照抄 ORIG 缺陷），并写正向 if 以复现
                // ORIG 的 `test %al,%al; je <second=1>` 形态（无 xor 物化）。
                if (card->is_stackable())
                    // ORIG 1315 段直接读字段 +0x07（m_addInfo2），无 get_add_info call
                    itemList[i].second = item.m_addInfo2;
                else
                    itemList[i].second = 1;
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
        // [R19a08 P183 去具名缓存] ORIG 0x085aa057 / 0x085aa0e6 / 0x085aa101 等每个
        // 使用点各自完整重算 `m_member[slot].m_pUser`（stride 0x18 八条链 +
        // `mov (%eax),%eax`），无 CUser* 具名槽 ⇒ 逐点写表达式（shl $3/add $0x78
        // 计数 O=14 = N=14）。
        m_member[slot].m_pUser->getCurCharacQuestW()->set_authen_data(
            (QUEST_CONDITION)4, memberCount, 0);
        // ORIG 0x85aa027..0x85aa057：64 位 elapsed（m_field0 低/ m_field4 高，
        // 即 this+0x32c/+0x330）经 __divdi3 除以 1000 后取低 32 位入第二个
        // set_authen_data（QUEST_CONDITION)2；ORIG 每个 set 前各调一次
        // getCurCharacQuestW（两次 call，互不复用）。
        // [b28d-solo-CPartyDepsD2] ORIG 0x85aa027 段对 +0x32c/+0x330 直接做
        // 64 位 __divdi3（`mov 0x330(%eax),%edx` 单次高半装载，无 sar/or 拼接）
        // ⇒ 用 BattleData.h 的 64 位 union 视角（P180）。
        m_member[slot].m_pUser->getCurCharacQuestW()->set_authen_data(
            (QUEST_CONDITION)2,
            (int)((long long)bd(this).m_clearPlayedTime / 1000),
            0);
        if (m_battleField.m_pDungeon->m_bloodDungeonType == 0 &&
            !m_battleField.m_pDungeon->isTowerOfDespairDungeon() &&
            !m_battleField.m_pDungeon->isTournamentDungeon())
            m_member[slot].m_pUser->Send(packet);
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
            SendSecretShopItemList(m_member[slot].m_pUser);
            APSystem::CUserProc::ClearActionAndSendtoUser(
                        m_member[slot].m_pUser, (APSystem::_ActionGroupIndex)0xd, 0, (ENUM_PACKETCLASS)0);
        }
        if (m_member[slot].m_pUser->getBreakAwayDungeonClearCnt() > 0)
            m_member[slot].m_pUser->subBreakAwayDungeonClearCnt();
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
    m_battleField.m_dungeonClearTracer.Trace(__PRETTY_FUNCTION__);   // [Q2P342ScanB] 同上
    m_tournamentVictory = 1;
    QuickParty::RewardData_Param rewardParam(this);
    QuickParty::RewardData_Result rewardResult;
    set_random_reward((QuickParty::RandomBuffType)10, rewardParam, rewardResult);
    rewardResult.clear();
    return;
}