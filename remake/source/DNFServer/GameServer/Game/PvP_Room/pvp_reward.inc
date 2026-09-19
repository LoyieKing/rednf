// §6.2 巨件核销：P167 153/153 [0缺0多]；gap 1045→-55；残差归因 N82（ORIG 帧
// 0xfc/36 槽 vs 4.4 帧 0x140/55 槽全帧平移，edi/esi/ebx 与 esi/ebx 寄存器分配
// 差）+ N88 装载序 + 少量 4.4 布尔物化（jcc+5）；逐块人工比对语义与 Ghidra C
// 一致（详见 docs/asm_translation_patterns.md 负例表）⇒ N 封顶。
// [b28d-7 N82] ORIG 0x085da882：帧 $0xfc + 槽组 -0x68/-0x6c 起，4.4 帧 $0x140 + 槽组 -0xa8 起（N82 槽复用差异反向放大 ndiff）；P167 CLEAR（153=153）⇒ 语义完成、byte 层封顶。
// [L10] gap 复测 2026-09-10：1038 vs 1090 → 1088（P52/P183 删 proxy 具名局部改链式，
// 见函数内 local_a1 段注释）；p167 复验差集仅 C1/C2、D1/D2 同址别名（P93）⇒ CLEAR。
// 残余 = N82 帧/槽整体平移（帧 $0xfc vs $0x140）+ N88 装载序 + 4.4 布尔物化岛，
// 判据① jcc 目标下标序列（ORIG 68 vs 4.4 72，多出 4 条集中于槽/帧平移段）封顶。
// [b28d-solo-PvP_Room] 本轮：P225 拆 `P_WIN!=0 && IsPvpObserver!=1` 嵌套 if、P0 去
// `CEventBase* ev` 具名局部（内联 GetRepeatEvent->IsEventing）⇒ 1096→1088；残余 N82
// 帧 $0xfc/36 槽 vs $0x140/55 槽全平移 + 布尔物化。
// [b28d-t3-PoolBigThree] **752→675**（考古的失传老版本 s408 口径，TU 三数不变）：末段座位循环
// `int i=0; while(true){ if (7<i) {channel6 日志; OnEndPvP; break;} …; i=i+1; }`
// 改 `for (int i=0; i<=7; i=i+1) {…}` + 循环后直落代码 —— ORIG 0x085da9a0
// `movl $0,-0x58; jmp COND` + 函数尾 0x085db76b `addl $1; cmpl $7; setle; test; jne
// BODY`（P226 尾测形）；旧 while 形把出口块放循环头并**复制 epilogue**
// （`add $0x140;pop×4;ret` 岛），与 ORIG 尾部单份 fall-through 错位 ~49 行。
// 残余 675 = N82 帧/槽组整体平移 + 布尔物化岛，判词沿用 b28d-7/solo。
// [L10b 2026-09-10] P225 推广：mode4 段 `bVar15 = (integrated && eventing && !observer)`
// 的 `&&` 形在 4.4 产 bool 常量岛（mov$1/jmp/mov$0/jmp/mov$0/cmpb/je，+4 条）⇒ 拆
// 嵌套 if（bVar15=false + 两级 if）后 1091→1085（ORIG 1045）。残余 = N82 帧 0x140 vs
// 0xfc（+0x44，17 个具名局部）+ 槽组整体平移 + 布尔物化。
void PvP_Room::pvp_reward()   // [b28c-16 SIGFIX] ORIG 串 void
{
    int local_dc[8];
    int local_70 = GetRanking(local_dc);
    int local_6c = GetAce();
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        if (G_GameWorld()->IsFreePvPChannel() != 1)
        {
            CalcuateVictoryPoint(local_dc, local_70);
        }
    }
    unsigned char local_66 = 1;
    // [J8PvPRoom 2026-09-11] P328：ORIG 0x085da8b0 段只有 `movb $0x1,-0x62`（local_66）
    // 与 `movl $0x0,-0x60`（local_64）两条清零，-0x61（local_65）**无初始化指令**
    // （首次写为循环内 `mov %al,-0x61(%ebp)`）⇒ 源码声明无初值，删 `= 0`。
    unsigned char local_65;
    for (int local_64 = 0; local_64 < 8; local_64 = local_64 + 1)
    {
        // [b28d-solo-PvP_Room] P225 推广：ORIG 0x085da882 两条件各为独立
        // `test; je <T>` 直跳；`&&` 链在 4.4 恒物化 bool 岛（mov$1/jmp/mov$0/
        // test/je，+5 条）⇒ 拆嵌套 if。
        if (P_WIN(local_64) != 0)
        {
            if (IsPvpObserver(local_64) != 1)
            {
                local_65 = (unsigned char)local_64;
                local_66 = 0;
                break;
            }
        }
    }
    char local_5d = (char)check_acquire_guild_point();
    ((IMatch*)m_matching)->OnReward(this, get_team((int)local_65), local_66 == 1);
    // [b28d-t3-PoolBigThree] P226 尾测形重试（本件 solo 判词未覆盖块布局）：ORIG
    // 0x085da9a0 `movl $0,-0x58; jmp COND` + 尾块 0x085db76b `i++; cmpl $7; setle;
    // test; jne BODY` ⇒ for(i=0; i<=7; i=i+1) 形（旧 while(true)+头部 7<i 判定把
    // 出口块（channel6/OnEndPvP）置于循环头 + 复制 epilogue（`add $0x140;pop×4;ret`）
    // 与 ORIG 函数尾单份 fall-through 块整体错位）。
    for (int local_5c = 0; local_5c <= 7; local_5c = local_5c + 1)
    {
        CUser* local_58 = P_SEATS(local_5c);
        if (local_58 != 0)
        {
            disconnect_detecter::GetInstanceDisconnectDetecter()->UnregisterUser(
                P_SEATS(local_5c));
            local_58->add_pvp_play_info(
                GlobalData::s_systemTime_.getCurSec() - m_battleStartSec, 1);
            // [J8PvPRoom 2026-09-11 · P231 实测] 原 `bool bVar15;` 在 4.4 恒落 -0x55 槽
            // （`movb $0x0/$0x1,-0x55` ×3 + `cmpb $0x0,-0x55` ×2，ORIG 0x085daa1f 段全程
            // %eax 无槽）⇒ 试裸 register 提示消槽。
            register bool bVar15;
            if (G_GameWorld()->IsIntegratedPvPBaseChannel() != 0)
            {
                // [b28d-solo-PvP_Room] P0：ORIG 0x085da882 的 GetRepeatEvent 返回值
                // 全程在 %eax（无槽往返，`mov %eax,(%esp)` 直传 this）⇒ 原码是内联
                // 表达式，无 `CEventBase* ev` 具名局部（旧形多 6 条槽中转）。
                // [H8-PvP 2026-09-11 复测·N] ORIG 0x85daa1f / 0x85daa4c / 0x85daa65
                // 三个 je 同落 +0x1ec（`mov $0,%eax`），全真 `mov $1,%eax; jmp
                // +0x1f1` ⇒ 原码确为**单条** `bVar15 = A && B && C;`（三条件平等短路、
                // 无槽）。但 4.4 同址产同形后**额外**落槽 store/load
                // （`mov %al,-0x85(%ebp); cmpb $0x0,-0x85(%ebp)` vs ORIG 裸
                // `test %al,%al`）⇒ 本件读数逐值不变（1085 / jcc 99）⇒ 跨版本
                // bool 寄存器化差异（4.1 无槽 / 4.4 必给槽），净 0，保留嵌套 if 原形。
                bVar15 = false;
                if (GlobalData::s_event_manager->GetRepeatEvent(0x38)->IsEventing((CUser*)0) != 0)
                {
                    if (IsPvpObserver(local_5c) == 0)
                    {
                        bVar15 = true;
                    }
                }
            }
            else
            {
                bVar15 = false;
            }
            // ORIG 0x085daa76..（P167 缺 getCurCharacR 同族）：bVar15 由 integrated
            // 频道 eventing 判定独立算出；发包块整体包在 `bVar15 && GetServerGroup()!=0`
            // 条件内（条件臂 + 块内 m_serverGroup + GetServerProxy = 3 次独立调用，P0/P48）。
            if (bVar15 && local_58->GetServerGroup() != 0)
            {
                Packet_Server_Match_data local_a1;
                local_a1.m_serverGroup = (char)(long)local_58->GetServerGroup();
                if (P_WIN(local_5c) == 0)
                {
                    local_a1.m_lose = 1;
                }
                else
                {
                    local_a1.m_win = 1;
                }
                // [L10] P52/P183（GiveOuterPenalty 同族实证 -2 条）：ORIG 段无 proxy
                // 具名槽 ⇒ 保链式，消 -0xNN store/reload。
                GlobalData::s_statistic_proxy_mgr
                    ->GetServerProxy((ENUM_SERVER_GROUP)(long)local_58->GetServerGroup())
                    ->SendPacket((char*)&local_a1, 0xd);
            }
            unsigned int local_84 = 0;
            if (((local_5d == 0) || (P_WIN(local_5c) == 0)) ||
                (IsPvpObserver(local_5c) == 1))
            {
                bVar15 = false;
            }
            else
            {
                bVar15 = true;
            }
            if (bVar15)
            {
                local_58->add_guild_point_item();
            }
            CMissionList_Charac* local_48 =
                (CMissionList_Charac*)local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
            local_48->setIssue(P_WIN(local_5c) != 0, m_pvpBattleMode == 2);
            MissionClearCondition_Parameter local_b0((PVP_BATTLE_MODE)m_pvpBattleMode);
            short local_af = (short)local_48->getIssue_mode(m_pvpBattleMode == 2);
            bool local_b1 = false;
            local_48->Update_Win_event(*local_58, local_b0, local_b1);
            if (local_b1 != false)
            {
                // [H8-PvP 2026-09-11 · P0/P48] ORIG 0x85dac1e..0x85dac45 两处
                // getPVPResultRefR 的返回值均 %eax 直用（`mov 0x8(%eax),%edx` /
                // `mov 0x14(%eax),%eax`，无槽往返）⇒ 原码内联两处调用，无 `r` 具名
                // 指针；写回同样直用 getPVPResultRefW 的 %eax（无 `w` 槽）。
                int local_44 = GlobalData::s_ref_pvp_grade->GetPvpGrade(
                    local_58->getPVPResultRefR()->m_expPoint);
                if (local_58->getPVPResultRefR()->m_pvpGrade < local_44)
                {
                    local_58->getPVPResultRefW()->m_pvpGrade = local_44;
                }
            }
            local_58->send_MissionList();
            if (G_GameWorld()->IsPVPChannel() != 0)
            {
                if (G_GameWorld()->IsWinPointPvPChannel() != 0)
                {
                    local_58->add_pvp_result(P_WIN(local_5c) != 0, &local_84);
                    if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
                    {
                        local_84 = 0;
                    }
                    if (P_WIN(local_5c) != 0)
                    {
                        if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
                        {
                            APSystem::CUserProc::ClearActionAndSendtoUser(
                                local_58, (APSystem::_ActionGroupIndex)0x12, 0,
                                (ENUM_PACKETCLASS)0);
                        }
                        else if ((m_pvpBattleMode == 2) || (m_pvpBattleMode == 5))
                        {
                            APSystem::CUserProc::ClearActionAndSendtoUser(
                                local_58, (APSystem::_ActionGroupIndex)0x13, 0,
                                (ENUM_PACKETCLASS)0);
                        }
                        else if (m_pvpBattleMode == 3)
                        {
                            APSystem::CUserProc::ClearActionAndSendtoUser(
                                local_58, (APSystem::_ActionGroupIndex)0x14, 0,
                                (ENUM_PACKETCLASS)0);
                        }
                    }
                }
                else if (G_GameWorld()->GetChannelType() == 6)
                {
                    int local_40;
                    if (P_WIN(local_5c) == 0)
                    {
                        local_40 = -1;
                    }
                    else if (((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3)) ||
                             (m_pvpBattleMode == 5))
                    {
                        local_40 = 1;
                    }
                    else
                    {
                        int local_3c = count_pvp_user();
                        if (local_3c < 4)
                        {
                            local_40 = 1;
                        }
                        else if (3 < local_3c)
                        {
                            local_40 = 2;
                        }
                    }
                    local_58->add_guild_pvp_result(local_40);
                }
            }
            unsigned int local_54 = 0xffff;
            int local_50 = 0;
            int local_88 = 0;
            if (((m_pvpBattleMode == 3) &&
                 (local_50 = m_relayMgr.GetAllKillUserIndex(&local_88), local_50 >= 0)) &&
                (local_50 < 8) && (P_SEATS(local_50) != 0))
            {
                local_54 = (unsigned int)P_SEATS(local_50)->get_unique_id() & 0xffff;
            }
            PacketGuard local_94;
            local_94.clear();
            local_94.put_header(0, 0x2f);
            local_94.put_byte((int)local_65);
            if (G_GameWorld()->GetChannelType() == 0xf)
            {
                // [H8-PvP 2026-09-11 · P0] ORIG 0x85daf6e 段 `call GetCharacExpandData;
                // mov %eax,(%esp); call CFairPvPScore::GetLastTrainingSealCount` ——
                // 返回值 %eax 直作下一调用 receiver（无槽）⇒ 原码内联，无 `fair` 具名指针。
                local_94.put_int(((fair_pvp::CFairPvPScore*)local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7))->GetLastTrainingSealCount());
            }
            else if (G_GameWorld()->GetChannelType() == 0x10)
            {
                local_94.put_int(0);
            }
            else if (G_GameWorld()->GetChannelType() == 0xe)
            {
                local_94.put_int(0);
            }
            else
            {
                local_94.put_int(local_58->GetWinPoint());
            }
            local_94.put_byte(local_58->get_pvp_grade());
            if (G_GameWorld()->GetChannelType() == 6)
            {
                local_94.put_int(0);
            }
            else
            {
                int iVar14 = local_58->get_pvp_result_exp_point();
                int local_38 = iVar14 - local_58->get_pvp_result_oldexp_point();
                if ((local_54 != 0xffff) && (P_TEAMS(local_5c) == local_88))
                {
                    int iVar9 = GetTeamCount(local_88);
                    local_38 = local_38 + (iVar9 * 5 * local_38) / 100;
                }
                PvpResultType* w = local_58->getPVPResultRefW();
                // ORIG 0x085dc1ce..（P0/P48 去缓存）：w->m_expPoint 与 LogHistory 的
                // 第 3 参各自独立调用 get_pvp_result_oldexp_point()，不缓存到具名局部
                // （缓存则少 1 call 点，P167 差集 -1 根因）。
                w->m_expPoint = local_58->get_pvp_result_oldexp_point() + local_38;
                local_94.put_int(local_38);
                const PvpResultType* r = local_58->getPVPResultRefR();
                local_58->LogHistory("PvPExp,%d,%d,%d", r->m_expPoint,
                                     local_58->get_pvp_result_oldexp_point(),
                                     local_38);
            }
            // [H8-PvP 2026-09-11 · P0] ORIG 0x85db11f `call has_within_Mission; test
            // %ax,%ax; sete %al; test %al,%al; je` —— 16 位返回值直用（**无 sVar4 槽**）
            // ⇒ 原码内联调用（callee 声明 short，见 CUser.h:2694）。
            if (local_58->has_within_Mission() == 0)
            {
                local_58->acceptable_within_mission();
            }
            local_94.put_byte(local_70);
            for (int local_34 = 0; local_34 < local_70; local_34 = local_34 + 1)
            {
                int local_30 = local_dc[local_34];
                if (P_SEATS(local_30) != 0)
                {
                    local_94.put_short((int)P_SEATS(local_30)->get_unique_id() & 0xffff);
                    local_94.put_int(m_field78[local_30]);
                    local_94.put_byte(P_SEATS(local_30)->get_pvp_grade());
                    local_94.put_int(P_SEATS(local_30)->get_pvp_result_exp_point());
                    local_94.put_int(GlobalData::s_ref_pvp_grade->GetPvpCurrRankPoint(
                        P_SEATS(local_30)->get_pvp_grade()));
                    local_94.put_int(GlobalData::s_ref_pvp_grade->GetPvpNextRankPoint(
                        P_SEATS(local_30)->get_pvp_grade()));
                    int local_2c = m_deathMatch.getKillCount(local_30);
                    int local_28 = m_deathMatch.getDeathCount(local_30);
                    local_94.put_int(local_2c);
                    local_94.put_int(local_28);
                    if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
                    {
                        // [L10] P227 实测（已回退）：改全表达式临时链后 push 3→4 与 ORIG
                        // 对齐，但指令 1088→1091、差异行 1008→1049 反劣（vararg 求值
                        // 前置引入额外槽往返）⇒ 维持具名形（N82 帧/槽族，非 P227 适用件）。
                        cMyTrace local_80(__PRETTY_FUNCTION__, 0xdab, 0);
                        local_80("<PVP_REWARD> Room(%d), iRank(%d), iRankMember(%d), Player(%s), mode(%d), KillCnt(%d), DeathCnt(%d)",
                                 get_index(), local_34, local_30,
                                 P_SEATS(local_30)->getCurCharacName(), m_pvpBattleMode,
                                 local_2c, local_28);
                    }
                }
            }
            if (local_70 != 0)
            {
                if ((local_6c == -1) || (P_SEATS(local_6c) == 0))
                {
                    local_94.put_short(0);
                }
                else
                {
                    local_94.put_short((int)P_SEATS(local_6c)->get_unique_id() & 0xffff);
                }
            }
            local_94.put_short((int)local_54);
            local_94.put_int((int)local_84);
            local_94.put_byte((int)local_66);
            if (G_GameWorld()->GetChannelType() == 0xf)
            {
                // [H8-PvP 2026-09-11 · P48/P0] ORIG 0x85db2e0 段两次
                // GetCharacExpandData(7) 均 `mov %eax,(%esp)` 直传（无 `fair` 槽）⇒
                // 原码两处内联重调。
                local_94.put_short(((fair_pvp::CFairPvPScore*)local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7))->GetLastTrainingSealBonusCount());
                local_94.put_short(((fair_pvp::CFairPvPScore*)local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7))->GetRemainBonusCount());
            }
            else if (G_GameWorld()->GetChannelType() == 0x10)
            {
                local_94.put_short(0);
                local_94.put_short(0);
            }
            // [H8-PvP 2026-09-11 · P0/P48] ORIG 0x85db4f4 同形（第二调用点，无槽）。
            local_94.put_byte(local_58->has_within_Mission() != 0);
            unsigned char local_49 = 0xff;
            if (m_pvpBattleMode == 3)
            {
                local_49 = (unsigned char)m_relayMgr.GetKillCount(local_5c);
            }
            local_94.put_byte((int)local_49);
            local_94.finalize(true);
            local_58->Send(local_94);
            // [H8-PvP 2026-09-11 · P48/P0] ORIG 0x85db58c 段 `call get_pvp_battle_mode;
            // cmp $0x4,%eax; je …; call get_pvp_battle_mode; cmp $0x5,%eax; jne …`
            // —— 两次独立调用、返回值 %eax 直用（**无 `mode` 槽**）⇒ 原码两处内联重调。
            if (((get_pvp_battle_mode() == 4) || (get_pvp_battle_mode() == 5)) && (local_84 != 0))
            {
                int local_b8 = 0;
                int local_bc = 0;
                if (local_58->gain_exp_sp((int)local_84, local_b8, local_bc,
                                          (CUser::eExpAddReason)10, 0, 0) != 0)
                {
                    local_58->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
                }
            }
            // [H8-PvP 2026-09-11 · P0] ORIG 0x85db6a0 段 `call getCurSec; mov %eax,%ebx;
            // … call getHackAnalyzer; mov %ebx,0x4(%esp)` —— 结果以 callee-saved %ebx
            // 跨调用保活（**无槽**）⇒ 原码内联，无 `lVar13` 具名量。
            local_58->getHackAnalyzer()->setLastMonsterDeadTime(
                GlobalData::s_systemTime_.getCurSec());
            // [H8-PvP 2026-09-11 · P0/P52] ORIG 0x85db6a8 段 GetRepeatEvent(0x37) 的
            // 返回值全程 %eax（`mov (%eax),%edx; add $0x34,%edx; …; mov %eax,(%esp);
            // call *%edx`，无槽往返）⇒ 原码内联，无 `CEventBase* ev` 具名局部；
            // `live` 同（GetCharacExpandData 返回值 %eax 直用）。
            if (GlobalData::s_event_manager->GetRepeatEvent(0x37)->IsEventing((CUser*)0) != 0)
            {
                ((CPvPLiveEventData*)local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)3))
                    ->IncreasePlayCount(local_58);
            }
            // [H8-PvP 2026-09-11] ORIG 0x85db6cd `mov 0x8(%eax),%edx` 直接消费
            // getPVPResultRefR 的返回值（%eax 无槽往返）⇒ 原码内联，无 `r2` 具名指针。
            int local_24 = GlobalData::s_ref_pvp_grade->GetPvpGrade(
                local_58->getPVPResultRefR()->m_expPoint);
            // [H8-PvP 2026-09-11] ORIG 0x85db6c8/0x85db6db 双条件（`jle`=local_24<1、
            // `je`=grade==local_24）同落 +0xe62（`mov $0,%eax`），全假落 +0xe5b
            // （`mov $1,%eax`）后 `test %al,%al; je` —— 即**无 bVar15 具名槽**（旧形
            // `bVar15=false/true + if(bVar15)` 在 4.4 恒落 -0x85 槽 store/load）。
            if ((local_24 >= 1) &&
                (local_58->getPVPResultRefR()->m_pvpGrade != local_24))
            {
                // [H8-PvP 2026-09-11] ORIG 0x85db6f3 `call getPVPResultRefW; mov
                // -0x20(%ebp),%edx; mov %edx,0x14(%eax)` —— 返回值 %eax 直用 ⇒ 无
                // `w2` 具名指针槽（旧形多一次 store/reload）。
                local_58->getPVPResultRefW()->m_pvpGrade = local_24;
                CMissionList_Charac* mission = (CMissionList_Charac*)local_58->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
                mission->MakeMissionList_forOldUser(*local_58);
                G_GameWorld()->send_AllBasicInfo(local_58);
            }
        }
    }
    if (G_GameWorld()->GetChannelType() == 6)
    {
        m_guildWarLog.WriteGuildWarPvPLog(m_index, (char)m_pvpBattleMode, m_users);
    }
    ((IMatch*)m_matching)->OnEndPvP(this);
}