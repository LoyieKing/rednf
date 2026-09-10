void CFairMatch::UpdateScore(PvP_Room* room, PlayerInfoThird& info,
                             ENUM_SCHOOL_MATCH_TEAM teamIdx,
                             int win, int lose, int result)
{
    (void)teamIdx;
    // P191 配套：ORIG 双初始化形 `movl $0,-0x28` ×2（声明 + for-init 各一条）。
    int i = 0;
    for (i = 0; i < 4; ++i) {
        // b28d-solo-CFairMatchRooms：ORIG 先读 m_user 存槽（@+0x22），再两条独立
        // 守卫（m_flag 直读字段 / user 读槽；P133/P157 去引用缓存）；P296 嵌套
        // 正向 if 复现 ORIG `movzbl; test %al,%al; je` 直测（取反 continue 形
        // 4.4 恒 xor $1 物化，N122）。
        CUser* user = m_teams[teamIdx].m_players[i].m_user;
        if (m_teams[teamIdx].m_players[i].m_flag) {
            if (user != 0) {
                const int seat = room->get_user_seat(user);
                // b28d-solo-CFairMatchRooms：ORIG（Ghidra C）为两条独立早返
                // （`if (seat < 0) return; if (7 < seat) return;`），合并 || 会产
                // 不同控制流（P133）。
                if (seat < 0)
                    return;
                if (seat > 7)
                    return;
                // b28d-solo-CFairMatchRooms：P227 全表达式临时链。
                CSwitchLog(
                    "void fair_pvp::CFairMatch::UpdateScore(PvP_Room*, fair_pvp::PlayerInfoThird&, ENUM_SCHOOL_MATCH_TEAM, int, int, int)",
                    0xee5, 0, 0)("pvp@log %s,%s,room(%d),%d,%d,%d",
                                 user->get_acc_name(), user->getCurCharacName(),
                                 room->get_index(), room->get_pvp_battle_mode(),
                                 win, lose);
                // b28d-solo-CFairMatchRooms：ORIG（Ghidra C）job=param_2+5、
                // grow=param_2+6 低 nibble —— param_2 即本函数第 2 参 info。
                // 原实现取 m_teams[teamIdx].m_players[0] 属真语义偏差（契约应为形参）。
                reinterpret_cast<CFairPvPScore*>(
                    user->GetCharacExpandData(
                        static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
                    ->UpdateScore(static_cast<int>(info.m_kind.m_job),
                                  info.m_kind.m_grow & 0xf,
                                  static_cast<int>(
                                      static_cast<unsigned char>(
                                          GetPlayCountPerOneTeam())),
                                  win, lose, result,
                                  room->m_relayMgr.GetKillCount(seat));

                // ORIG 0x085658ca `mov %eax,-0x1c(%ebp)`：GetCharacExpandData(8)
                // 结果先存具名局部（P0 槽往返 ⇒ 原码有具名局部），再在
                // Update_GameOver_event 处使用（内联形会把该 call 排到 killCnt 之后）。
                CMissionList_Charac* missionList =
                    reinterpret_cast<CMissionList_Charac*>(
                        user->GetCharacExpandData(
                            static_cast<ENUM_CHARAC_EXPAND_TYPE>(8)));
                // ORIG `cmp $0x1,%al`（单字节比较）⇒ 调用点收窄（P3）。
                {
                    MissionClearCondition_Parameter cond(
                        static_cast<PVP_BATTLE_MODE>(
                            static_cast<unsigned char>(GetPlayCountPerOneTeam())
                                == 1 ? 2 : 3));
                    // ORIG 0x8565904 `mov %eax,-0x44(%ebp)`：二次取 GetKillCount
                    // 结果写入死局部（P171 死写入照抄；P232 未使用数组仍占槽）。
                    int killCnt[1];
                    killCnt[0] = room->m_relayMgr.GetKillCount(seat);
                    (void)killCnt;
                    missionList->Update_GameOver_event(*user, cond);
                }
                GiveTrainingSeal(user, win != 0);
            }
        }
    }
