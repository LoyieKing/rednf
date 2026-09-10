void CMissionList_Charac::Update_RecvPacket_event(
    CUser& user, std::vector<short>& kinds, std::vector<int>& values)
{
    if (G_GameWorld()->GetChannelType() != 0xf)
        return;
    if (getIndex_byKind(0x1b) != 0)
        return;
    alter();
    PvP_Room* room = (PvP_Room*)user.GetPVPRoom();
    if (room == 0)
        return;
    {
        MissionClearCondition_Parameter param(
            (PVP_BATTLE_MODE)room->get_pvp_battle_mode());
        // [b28d-solo-CMissionList_Charac] ORIG 0x085e5d2a `movl $0x0,-0x10(%ebp)`
        // 位于 for 初始化区（循环外）⇒ 原码 mission 声明于循环外并零预清（P198①），
        // 循环体内仅赋值。
        // 判词 N82（116/116，指令序列全同）：残差=槽号 room(-0x14/-0x18)、
        // mission(-0x10/-0x14)、i(-0xc/-0x10)、param(-0x33/-0x27)（4.4 首次使用序）。
        CMission* mission = 0;
        for (unsigned int i = 0; i < kinds.size(); ++i)
        {
            mission = (CMission*)G_CDataManager()->find_mission(kinds[i]);
            if (mission == 0)
                goto L_next;
            {
                    // ORIG 0x85e5caa：kind 6/0x15 执行 check_mission_kind，
                    // 其它 kind 记 HackUser 日志；ORIG 短路二次 GetMissionKind
                    // + 两分支内各重取（P167 定 4 调用位点，复现需逐位点对齐）。
                    // [b28d-solo-CMissionList_Charac] ORIG 无 kind 局部槽（首次调用
                    // 结果直接 `cmp $6,%eax`，P0 右值内联）⇒ 两处内联调用。
                    // [b28d-solo-CMissionList_Charac] 本轮修形：①ORIG 0x085e5d93
                    // 的 bool 物化 `mov $1,%eax;jmp;mov $0,%eax;test %al;je` 无栈槽
                    // ⇒ 删具名 hackUser，改短路条件直判（P214①/P0）；②trace 改
                    // P227 全表达式临时链，令 GetMissionKind 在 ctor 前求值并跨
                    // ctor 以 %ebx 保活（ORIG 0x085e5dae `mov %eax,%ebx`）。
                    if ((mission->GetMissionKind() != 6) &&
                        (mission->GetMissionKind() != 0x15))
                    {
                        cMyTrace(__PRETTY_FUNCTION__, 0x704, 5)(
                            "[@missionSystem] HackUser! send [%d kind]\n",
                            mission->GetMissionKind());
                        goto L_join;
                    }
                    else
                    {
                        check_mission_kind(mission->GetMissionKind(), user,
                                           param, values);
                        goto L_join;
                    }
                }
        L_next:;
        L_join:;
        }
    }
