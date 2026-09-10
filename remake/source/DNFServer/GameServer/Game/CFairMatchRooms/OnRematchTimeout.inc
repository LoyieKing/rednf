void CFairMatch::OnRematchTimeout(PvP_Room* room)
{
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链。
    // P0：ORIG 无 rematchable 槽（`call CheckRematch; movzbl %al,%edi` 直接留
    // callee-saved 供 vararg）⇒ 表达式内联为最右实参。
    CSwitchLog(
        "virtual void fair_pvp::CFairMatch::OnRematchTimeout(PvP_Room*)",
        0xfd7, 0, 0)("pvp@log room(%d),%d,%d",
                     room->get_index(), room->get_pvp_battle_mode(),
                     static_cast<unsigned char>(CheckRematch(room)));

    // [b28f-SweepCFairMatch 真语义 bug + P292] ORIG 0x0856602b `call CheckRematch;
    //   xor $0x1,%eax; test %al,%al; je 085662bb`，而 0x085662bb = `add $0x1c,%eax;
    //   call TeamInfoThird::refresh`（refresh 主体）⇒ ORIG 原码是**负极性写法 +
    //   分支交换**：`if (!CheckRematch(room)) { 队处理 } else { refresh 主体 }`。
    //   原实现把 refresh 主体放进 `!CheckRematch` 分支 ⇒ 重赛超时的真/假两路处理
    //   **整体颠倒**（CheckRematch==1 该整场续战，却去逐队拉出；==0 该拉出，却续战）。
    //   语义 + 形态同时按 ORIG 修正（bool callee 直测产 `xor;je`，无 cmp/sete）。
    if (!CheckRematch(room))
    {
        // 不能整场续战：两支队伍按各自去留意图分别拉出
        // ORIG: pull 队1 条件 = size(team1)!=0 且 *(int*)(this+0x68)==0
        //      （this+0x68 = m_teams[1].m_field04，重赛决定位）
        if (m_teams[1].size() != 0 && m_teams[1].m_field04 == 0) {
            _PullOutRoomTeam(room, static_cast<ENUM_SCHOOL_MATCH_TEAM>(1));
        }
        // ORIG 0x856607a 实证读 *(int*)(this+0x20) = m_teams[0].m_field04（4 字节）
        // （队决定位，OnAskRematch 写入；FM_TeamByte0 队头字节仅 CheckRematch 用）。
        // ORIG 0x085661f5 段 `test %eax,%eax; je; test; js; cmp $2; jg` = 无槽
        // switch 决策树（P251/P261；具名 team0Decision 恒产 cmpl mem 槽形）。
        switch (m_teams[0].m_field04) {
        case 0:
            _PullOutRoomTeam(room, static_cast<ENUM_SCHOOL_MATCH_TEAM>(0));
            if (m_teams[1].size() != 0) {
                PacketGuard pkt;
                pkt.put_header(0, 0x13b);
                pkt.finalize(true);
                m_teams[1].SendToTeam(pkt);
                // b28d-solo-CFairMatchRooms：P83/N84——ORIG 0x08566131 为结构整体
                // 赋值 rep movsl（0x12 dwords）。
                m_teams[0] = m_teams[1];
                m_teams[0].refresh();
                m_teams[1].reset();
                m_fieldB0 = 0;
            }
            break;
        case 1:
        case 2: {
            // [b28f-SweepCFairMatch] P211/P274：`_ReturnTeam(…, int teamNo, …)` 形参
            //   int 与局部 unsigned 异型 ⇒ 4.4 先求值第 3 参（mgrTeam）再写第 4 参
            //   常量 `movl $0x0,0xc(%esp)`，复现 ORIG 0x085661dd 段装载序（同
            //   OnConnectP2PTimeout 的手法，探针 /tmp/fm_p3.cpp w31）。
            const unsigned int mgrTeam = room->get_manager_team();
            PvP_Room* kept =
                _ReturnTeam(room, mgrTeam, 0);
            PacketGuard pkt;
            pkt.put_header(0, 0x13b);
            pkt.finalize(true);
            if (kept != 0)
                kept->send_to_pvp(pkt);
            m_teams[0].SendToTeam(pkt);
            m_teams[1].SendToTeam(pkt);
            if (m_teams[0].size() == 0) {
                // b28d-solo-CFairMatchRooms：P83/N84——ORIG 0x08566265 为结构整体
                // 赋值 rep movsl（0x12 dwords）。
                m_teams[0] = m_teams[1];
            }
            m_teams[0].refresh();
            m_teams[1].reset();
            m_fieldB0 = 0;
            break;
        }
        }
    }
    else
    {
        m_teams[0].refresh();
        m_teams[1].refresh();
        PacketGuard pkt;
        room->battle_reset();
        for (int seat = 0; seat <= 7; ++seat) {
            if (room->get_user_seat(seat) != 0) {
                // P0：ORIG 无 relay 引用槽（`lea 0x620(%eax),%edx` 直传 this）
                // ⇒ 表达式内联。
                room->m_relayMgr.OnReady(seat, true);
            }
        }
        room->start_pvp();
        room->make_state_info(reinterpret_cast<char*>(&pkt));
        G_GameWorld()->send_all(pkt);
    }
