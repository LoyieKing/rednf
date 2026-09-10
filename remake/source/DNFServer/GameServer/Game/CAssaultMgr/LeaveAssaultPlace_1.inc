void CAssaultPlace::LeaveAssaultPlace(int index, bool delFlag, bool penaltyFlag)
{
    // b28d-solo-CAssaultMgr: IDENTICAL_AE（fresh .o 复验；ORIG 0x082ead6a 453 条）。
    if (index < 0 || index > 7)
        return;

    CUser* user = m_assaulters[index].GetUser();
    if (user != 0)
    {
        // ---- GiveupPanalty 分支（82eadb0；_IsGiveupPanalty true 时）----
        if (_IsGiveupPanalty(penaltyFlag))
        {
            m_assaulters[index].GiveupPanalty();
            if (m_state > 1 && m_state <= 3)
            {
                user->SetCurCharacLeaveTimeout(true);
            }
            else if (m_state > 3 && m_state <= 6)
            {
                PacketGuard packet;
                _MakeUserState(&packet, user);
                G_GameWorld()->send_all(packet);   // ORIG 82eaee3：%eax 直通实参
            }
            // [SweepCAssaultMgr 真语义修正] ORIG 082eadc5 `je 082eaeb8`（目标 =
            // PacketGuard::C1 @+0x14e，**非** _IsBroadcastRunaway 段 +0x11f）
            // 与 Ghidra C（LeaveAssaultPlace.md:523）同证：_IsBroadcastRunaway
            // 在 `if (_IsGiveupPanalty(penaltyFlag))` **块内**（GiveupPanalty →
            // 状态分支 → 逃跑判定）。旧形置于块外 ⇒ _IsGiveupPanalty 为 false 时
            // 仍执行逃跑广播（真语义反向；AE 口径把分支目标归一为 <T> 故不显形）。
            // ---- _IsBroadcastRunaway（82eae89）----
            if (_IsBroadcastRunaway(user))
                _BroadcastRunaway(user, index);
        }

        // ---- 82eaeb8：leave 包 + PowerWar 计数 ----
        // b28d-solo-CAssaultMgr: ORIG -0x24 的 D1 在 082eb06f（收尾链 +0x310
        // 之前、ghost 块 +0x2e8 之后）⇒ 原码作用域覆盖后续全部语句，非独立块
        //（旧形析构紧跟 SendPacket，多 1 个独立 EH cleanup/_Unwind_Resume）。
        PacketGuard packet;
        _MakeLeaveUser(&packet, index);
        SendPacket(packet);
        if (GlobalData::s_power_manager->IsPowerWarEventOn() &&
            user->getCurCharacVill() == 7)
        {
            GlobalData::s_power_manager->GetPowerWarLog()
                ->IncTotalLeaveAssaultUser();
        }

        // ---- close 包：PowerWar 村庄用 powerWarHP，否则用槽位 HP ----
        if (GlobalData::s_power_manager->IsPowerWarEventOn() &&
            user->getCurCharacVill() == 7)
        {
            _SendCloseAssault(user, user->getPowerWarHP());
        }
        else
        {
            _SendCloseAssault(user, m_assaulters[index].GetHp());
        }

        // ---- 82eafb0：槽位非空 → 删除用户 + 用户状态广播 ----
        if (!m_assaulters[index].Empty())
        {
            _DelUser(index, delFlag);
            if (!user->IsCurCharacGhost())
            {
                PacketGuard statePacket;
                _MakeUserState(&statePacket, user);
                G_GameWorld()->send_all(statePacket);   // ORIG 82eb052 同形
            }
        }
    }

    // ---- 82eb07a：收尾链 ----
    // b28d-solo-CAssaultMgr: ORIG +0x310 段无外层 `if(user!=0)` 包裹（user==0
    // 时 %al=0 仍落到 +0x3d0 的 m_field100 检查；旧形多一层包裹 = 真语义差异，
    // 与 Ghidra C 的 if/else-if/else 链一致）。裸作用域仅维持缩进，零代码生成。
    {
        // 非 PowerWar（或非村庄7）且状态<=3 且索引 0/1：双方队伍置 7 后关闭。
        // b28d-solo: ORIG 82eb1xx 段为短路链直接 test（无 bool 槽）⇒ P0 内联。
        // b28d-solo-CAssaultMgr: ORIG 0x82eb07a 段短路物化形
        //（`cmpl user; je F; IsPowerWarEventOn; xor$1; test; jne T; vill; cmp$7;
        //   je F; T: mov$1; F: mov$0; test; je`）⇒ 源形 `user != 0 &&
        //（!IsPowerWarEventOn() || vill != 7)` 嵌套 `m_state <= 3 &&
        //（index==0||index==1)`（旧形缺 user 项且 m_state 并入外层链）。
        if (user != 0 &&
            (!GlobalData::s_power_manager->IsPowerWarEventOn() ||
             user->getCurCharacVill() != 7))
        {
            if (m_state <= 3 && (index == 0 || index == 1))
            {
                if (m_parties[0].m_field00 != 0)
                    ((CParty*)m_parties[0].m_field00)->SetAssaultState(7);
                if (m_parties[1].m_field00 != 0)
                    ((CParty*)m_parties[1].m_field00)->SetAssaultState(7);
                m_state = (ENUM_ASSAULT_STATE)7;
                CloseAssault();
                return;
            }
        }
        // 82eb13a：m_field100==0 → 同样置 7 后关闭。
        if (m_field100 == 0)
        {
            if (m_parties[0].m_field00 != 0)
                ((CParty*)m_parties[0].m_field00)->SetAssaultState(7);
            if (m_parties[1].m_field00 != 0)
                ((CParty*)m_parties[1].m_field00)->SetAssaultState(7);
            m_state = (ENUM_ASSAULT_STATE)7;
            CloseAssault();
            return;
        }
        // b28d-solo-CAssaultMgr: ORIG 0x82eb1b0 段 `call _CheckPlayable; xor$1;
        // test; je <epilogue>` = 4.1 对 `if (!_CheckPlayable()) { 剩余全部 }` 的形
        //（je 跳过整个 if 体到 epilogue）；单语句 `if (...) return;` 4.4 产 `jne`
        // 反向跳（旧早返形同此）。其后条件链为两段：链①`IsPowerWarEventOn() &&
        // user!=0 && vill==7` 物化后 test，链②`m_state > 3 && m_state <= 4` 为
        // 嵌套 if（非并入链①）。
        if (!_CheckPlayable())
        {
            if (GlobalData::s_power_manager->IsPowerWarEventOn() &&
                user != 0 &&
                user->getCurCharacVill() == 7)
            {
                if (m_state > 3 && m_state <= 4)
                {
                    int winCnt = 0;
                    int loseCnt = 0;
                    int aliveCnt = 0;
                    if (_CheckPowerWarInPlayer(
                            (ENUM_POWER_SIDE_TYPE)user->getPowerSide(),
                            (ENUM_POWER_WAR_WINNER_OR_LOSER)2, winCnt, loseCnt,
                            aliveCnt))
                    {
                        _SendPowerWarPointToGuildServer(
                            user, (ENUM_POWER_WAR_WINNER_OR_LOSER)2, winCnt,
                            loseCnt, aliveCnt);
                    }
                    CParty* party = (CParty*)user->GetParty();
                    if (party != 0)
                        party->resetStraightVictories();
                    else
                        user->resetStraightVictories();
                    for (int i = 0; i <= 7; ++i)
                    {
                        // ORIG 82eb2e6：`call Empty; test %al; jne <loop-inc>` 正向
                        // 判定（P22/P59：写 !Empty() 会 xor$1 物化多 1 条）。
                        if (m_assaulters[i].Empty())
                            continue;
                        CUser* slotUser = m_assaulters[i].GetUser();
                        if (slotUser->getPowerSide() != user->getPowerSide())
                        {
                            _SendStraightVictories(slotUser);
                            break;
                        }
                    }
                }
            }
            // 82eb349：双方队伍置 6 后结束。
            if (m_parties[0].m_field00 != 0)
                ((CParty*)m_parties[0].m_field00)->SetAssaultState(6);
            if (m_parties[1].m_field00 != 0)
                ((CParty*)m_parties[1].m_field00)->SetAssaultState(6);
            m_state = (ENUM_ASSAULT_STATE)6;
            EndAssault();
        }
    }
