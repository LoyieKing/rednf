// ORIG T 0x0855eb22（结算奖励全流程）
// §6.2 巨件核销：P167 45/45 [0缺0多]（修前 42/45）；gap 79→35；残差归因 N86/N82
// （t*0x70/j*3 寻址代数分解跨版本差：ORIG shl$4+lea+sub 折乘链 vs 4.4
//   shl$3+add，纯分解形态、语义同；全帧槽号平移）。
// 真修三处：
//   1) 结果包改栈对象 Packet_PvPResult res 走真 ctor（W 0x08568dc6≡
//      PacketHeader(0xfa5,0x80)，重建侧符号已在）；原手写展开的
//      reversed1/m_connNo 清零属发明语义（ORIG ctor 不清），删。
//   2) 实时徽章回填条件修正：ORIG 0x0855f539 固定寻址 this+i*0x1c+0x98/0x94
//      = teamB 的 m_flag/m_user（与 t 无关），原实现错挂 t 分支。
//   3) GetDailyBadge 由循环改 3 条独立调用（ORIG $0x0/$0x1/$0x2 立即数直传
//      三处 call，P0），读取 user 按数组化寻址 (&m_teamA)[t].m_players[i].m_user
//      复现 ORIG 乘法寻址链（无具名缓存槽）。
// [b28d-solo-CLeagueMatch] DIFF 992/960·ndiff 1048·N82（S217 新鲜 .o 口径）：
// 帧 0x1fc vs 0x24c，
// 4.4 槽复用策略差（§6.2 三件套口径：p167 调用点差集 45/45 CLEAR [0 缺 0 多]、
// gap 34 归因槽复用与 t*0x70/j*3 寻址代数分解；S180 试 P235 union 视角
// m_teams[t] 该件反劣（1390→1398）⇒ 两形取优保留 (&m_teamA)[t]；剩余全为
// 槽号平移与 shl-vs-lea；语义完成、byte 层 N 封顶）。
// [b28d-solo-CLeagueMatch·S408 定稿] **1048→567**（−481）：帧 0x1fc/0x1fc 全等、
// 22 槽集合逐槽全等、指令数 992/990、p167 45/45 CLEAR。修法=六处 P0/P183
// 内联（eloK2/winType/playCount/TeamInfo&/PlayerInfo&/schools/mySchool/fmt/st/
// winFlagLocal）+ P191 t/i 双初始化复用槽 + `int type;` 去初始化式 + 绿章三守卫
// 负向 continue（P224/P296）+ **真 bug** 删 UpdateRating 前多余的 m_user!=0 守卫
// （P140/P171，ORIG 0x0855f7d3..f819 只查 m_flag）+ ratings 等宽 union int 视角
// 复现 ORIG 单段零初始化。剩余 567 = N-STRIDE
// （*4/*8/*16 的 shl-vs-lea，探针 10 形全败）+ 少量寄存器名/中转（N7/N18a）
// + difflib 错位，语义完成。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局] 本轮两旋钮：①三处 `(&m_teamA)[t]`
// →`m_teams[t]`（读取 GetDailyBadge 的 user 寻址）⇒ our 989→983、行差 394/387→
// 346/333（改善）**但 gap 990-989=1→990-983=7 劣**（指令数硬指标）⇒ 按「四数不劣
// + 不加距」回滚，保持 S180 取优形；②`int type;` → `int type = 0;`（P328⑤ 反向）
// ⇒ our 990、行差 388（各 +1）劣，回滚。gap=+7 缺口的 ORIG-only 主体已逐形归因 =
// 0x0855f0xx 段 synth_mult copy 链（`mov %ecx,%eax`×14 / `mov %eax,%ecx`×13 /
// `lea (%edx,%eax,1),%eax`×17）+ N125（`lea (,%eax,4)`×8），同 OnCompleteLoad 探针
// 网络结论【不可翻】。p167：45/45 调用点全等，MISSING/EXTRA 仅 C1/C2·D1/D2 同址别名对。
void CLeagueMatch::OnReward(PvP_Room* /*room*/, int winTeam, bool draw)
{
    // [G9WorldTimerLeague 2026-09-10 复核] 成员 disp / 立即数多重集扫描
    // （ORIG 切片 vs fresh .o）：唯一 OURS-only {0x10:3, 0x80:3, 0x1c:3}
    // 与 ORIG-only {0xffffff80:3} 全部落在 `add $0x10` / `sub $0xffffff80`
    // （= add 0x80，ORIG 0x0855ed34 / 0x0855ee4e / 0x0855ef48 三处）与
    // stride-0x1c 强度归约链上 ⇒ N125/N4（4.1 lea+立即数拆分 vs 4.4 shl/add）
    // + N107 族，无语义缺口；p167 CLEAR（仅 C1/C2·D1/D2 同址别名对）。
    // ---- 结果包（ORIG 0x0855eb37 call W Packet_PvPResult::C1Ev 0x08568dc6，
    //       重建侧已有该 W 符号（PacketHeader(0xfa5,0x80)），改栈对象真 ctor；
    //       原手写字段展开含 reversed1/m_connNo 额外清零，属发明语义已删——
    //       ORIG ctor 仅设包头，不清该 2 字节） ----
    Packet_PvPResult res;

    // ORIG asm 统一寻址公式：team 块基 = pkt + 0x10 + 56*t，块内
    //   徽章值 @ 0x00 + i + 3*j（`movzwl 0x10(%ebx,%eax,2)` 读 PlayerInfo
    //   m_badges[i] → `mov %bl,(%eax)` 单字节写）
    //   新获旗标 @ 0x0c + i + 3*j（`movb $0x0,0xc(%eax)`）
    // 即 Packet_PvPResult::Team::m_dailyBadge[j][i] / m_newFlag[j][i]。
    // 1) 初始：两支队伍每日徽章快照写 m_dailyBadge[j][i]，同队旗标区清零。
    // [b28d-solo-CLeagueMatch·S408] P191：ORIG 循环变量 t 的 init 段有**两条**
    // `movl $0x0,-0x3c(%ebp)`（声明初始化 + for 初始化）⇒ 原码为
    // `int t = 0;` 前置声明 + `for (t = 0; …)`；三处 t 循环共用同一槽。
    int t = 0;
    for (t = 0; t <= 1; ++t) {
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            for (int i = 0; i <= 2; ++i) {
                res.m_team[t].m_dailyBadge[j][i] =
                    static_cast<char>(m_teams[t].m_players[j].m_badges[i]);
                res.m_team[t].m_newFlag[j][i] = 0;
            }
        }
    }

    // 2) 各分支记徽章（draw→type1 / A 队胜→type2 / 其余→type0）。
    // [b28d-solo-CLeagueMatch·S408] P0：ORIG 三分支各自 `movl $K,-0x38(%ebp)`、
    // 写包头时 `mov -0x38(%ebp),%eax; mov %al,-0xca(%ebp)` ⇒ 原码是**无初始化
    // 声明的 int type**（三分支全赋值）且 m_winType 直接取 type，无 char winType
    // 三目物化；m_eloK/m_playCount 亦无具名局部（ORIG 直读全局/静态后写包）。
    int type;
    if (draw) {
        type = 1;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            res.m_team[0].m_newFlag[j][1] = 0;   // 自队蓝章清零
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res.m_team[0].m_newFlag[j][2], false);
            res.m_team[1].m_newFlag[j][1] = 0;
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res.m_team[1].m_newFlag[j][2], false);
        }
    } else if (winTeam == 1) {
        type = 2;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(1),
                     &res.m_team[0].m_newFlag[j][1], false);
            res.m_team[0].m_newFlag[j][2] = 0;
            res.m_team[1].m_newFlag[j][1] = 0;
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res.m_team[1].m_newFlag[j][2], false);
        }
    } else {
        type = 0;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            res.m_team[0].m_newFlag[j][1] = 0;
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res.m_team[0].m_newFlag[j][2], false);
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(1),
                     &res.m_team[1].m_newFlag[j][1], false);
            res.m_team[1].m_newFlag[j][2] = 0;
        }
    }

    // 写包头数据字段（asm：elo_k2_@+0x0a 4B、winType@+0x0e 1B、playCount@+0x0f 1B）。
    // [b28d-solo-CLeagueMatch·S408] P0 内联：ORIG 0x0855f0xx 段三条均无局部槽往返。
    res.m_eloK = PvPGlobalEnvironmentVariable::school_match_elo_k2_;
    res.m_winType = static_cast<char>(type);
    res.m_playCount = nPlayerCountPerOneTeam_;

    // 3) 两队 schoolNo 表（intsBase=pkt+8，elem=(t*14+i+8)，per-team stride 56B）与
    //    school 排他检查（同校不算对手）及绿章提示。
    for (t = 0; t <= 1; ++t) {
        // [b28d-solo-CLeagueMatch·S408] P0/P183/P191：ORIG 0x0855efd3/0855efda 双
        // `movl $0x0,-0x20(%ebp)`（i 声明初始化 + 循环 1 初始化）后三个 i 循环复用
        // 同一 -0x20 槽；全程无 TeamInfo&/PlayerInfo&/schools/mySchool/fmt 具名局部
        // （每访问点重算 this+t*0x70+i*0x1c，ORIG 0x0855f057/f131/f2d5 三段实证）。
        int i = 0;
        for (i = 0; i < (int)nPlayerCountPerOneTeam_; ++i) {
            res.m_team[t].m_schoolNo[i] = m_teams[t].m_players[i].m_schoolNo;
        }
        for (i = 0; i < (int)nPlayerCountPerOneTeam_; ++i) {
            if (_getSameSchool(res.m_team[t].m_schoolNo, i,
                                (int)nPlayerCountPerOneTeam_,
                                res.m_team[t].m_schoolNo[i])) {
                // [SweepLeagueMatch2·P59/P222] ORIG 物理块序（855f1ac extra=1 →
                // 855f232 extra=0 → 855f23c 绿章）⇒ 外层正极性 if(sameSchool)
                // 使 addBadge 两臂落前、绿章落尾；内层 `!=0||!=0` 正极性短路。
                if (res.m_team[t].m_newFlag[i][1] != 0 ||
                    res.m_team[t].m_newFlag[i][2] != 0) {
                    addBadge(&m_teams[t].m_players[i], ENUM_BADGE_TYPE_0,
                             &res.m_team[t].m_newFlag[i][0],
                             true);
                } else {
                    addBadge(&m_teams[t].m_players[i], ENUM_BADGE_TYPE_0,
                             &res.m_team[t].m_newFlag[i][0],
                             false);
                }
            } else {
                res.m_team[t].m_newFlag[i][0] = 0;
                if (res.m_team[t].m_newFlag[i][1] != 0 ||
                    res.m_team[t].m_newFlag[i][2] != 0) {
                    // [b28d-solo-CLeagueMatch·S408] P224/P296/P168a②：ORIG
                    // 0x0855f303（`movzbl; xor $1; test; jne 0855f50a`）、
                    // 0x0855f312（`mov (%eax),%eax; test; je 0855f50d`）、
                    // 0x0855f34a（`call getCurCharacR; test; sete; test; jne 0855f510`）
                    // 是**三条独立负向守卫**，各自落**独立 nop+jmp 中继岛**
                    // （0855f50a/50d/510 三个岛）⇒ 原码为 `if(!flag) continue;` /
                    // `if(user==0) continue;` / `if(getCurCharacR()==0) continue;`
                    // （正向 && 链反产 je 直落、无岛、无 xor）。
                    if (!m_teams[t].m_players[i].m_flag)
                        continue;
                    if (m_teams[t].m_players[i].m_user == 0)
                        continue;
                    if (m_teams[t].m_players[i].m_user->getCurCharacR() == 0)
                        continue;
                    char textBuf[0x100] = { '\0' };   // P4：ORIG rep stos 内联清零
                    // [S408] P0：ORIG 0x0855f3c9..0x0855f3e6 findString 返回
                    // %eax 直压 0x8(%esp)（无 fmt 具名局部槽）。
                    OS_API::snprintf(
                        textBuf, 0xff,
                        g_scriptStringManager_.findString(
                            4, "school_green_badge_msg", (bool*)0),
                        1);
                    PacketGuard pg;
                    pg.put_header(0, 0xc);
                    pg.put_byte(0);
                    pg.put_short(0);
                    pg.put_byte(0);
                    pg.put_int((int)strlen(textBuf));
                    pg.put_str(textBuf, (int)strlen(textBuf));
                    pg.finalize(true);
                    m_teams[t].m_players[i].m_user->Send(pg);
                }
            }
        }
        // 回填实时徽章计数。ORIG 0x0855f539..0x0855f6da：条件寻址固定
        // this+i*0x1c+0x98(m_flag)/0x94(m_user) = **teamB** 槽，与 t 无关；
        // 读取 user 为 this+i*0x1c+t*0x70+0x24（含 t）；GetDailyBadge 为 3 条
        // 独立调用（$0x0/$0x1/$0x2 立即数直传，非循环，P0）。
        for (i = 0; i < (int)nPlayerCountPerOneTeam_; ++i) {
            // [SweepLeagueMatch2·P59] ORIG 0x0855f539(m_flag) / 0x0855f561(m_user)
            // 两守卫同落 855f6dc 岛（正向 `test; je` 短路）⇒ 正向包裹。
            if (m_teamB.m_players[i].m_flag != 0 &&
                m_teamB.m_players[i].m_user != 0) {
                res.m_team[t].m_dailyBadge[i][0] =
                    static_cast<char>((&m_teamA)[t].m_players[i].m_user->GetDailyBadge(
                        static_cast<ENUM_BADGE_TYPE>(0)));
                res.m_team[t].m_dailyBadge[i][1] =
                    static_cast<char>((&m_teamA)[t].m_players[i].m_user->GetDailyBadge(
                        static_cast<ENUM_BADGE_TYPE>(1)));
                res.m_team[t].m_dailyBadge[i][2] =
                    static_cast<char>((&m_teamA)[t].m_players[i].m_user->GetDailyBadge(
                        static_cast<ENUM_BADGE_TYPE>(2)));
            }
        }
    }

    // 4) Elo 结算并回写个人分 + accId 表（elem=(t*14+i+12)）。
    // [b28d-solo-CLeagueMatch·S408] ORIG 0x0855f70b/f712 是单段 `movl $0x0,-0x44/-0x40`
    // （float 数组的 4.1 零初始化形）；4.4 对 `float ratings[2]={0.0f,0.0f}` 恒发
    // 二段 `mov $0,%eax; mov %eax,-N`（P232 扩展）⇒ 用等宽 union int 视角写零。
    union { float f[2]; unsigned int u[2]; } ratings;
    ratings.u[0] = 0;
    ratings.u[1] = 0;
    for (unsigned int i = 0; i < (unsigned int)nPlayerCountPerOneTeam_; ++i) {
        for (t = 0; t <= 1; ++t) {
            ratings.f[t] = m_teams[t].m_players[i].m_rating;
        }
        CEloRating::calc(ratings.f[0], ratings.f[1],
                         static_cast<ENUM_ELO_RATING_SYSTEM>(type),
                         PvPGlobalEnvironmentVariable::school_match_elo_k_);
        for (t = 0; t <= 1; ++t) {
            // [b28d-solo-CLeagueMatch·S408] 真语义 bug 修正（多余守卫，P140/P171）：
            // ORIG 0x0855f7d3..0x0855f819 只 `movzbl +0x18(m_flag); test; je`
            // 后**直接**取 +0x24(m_user) 调 UpdateRating，**无 m_user!=0 检查**
            // ⇒ 删多余守卫照抄 ORIG 行为。
            if (m_teams[t].m_players[i].m_flag != 0)
                m_teams[t].m_players[i].m_user->UpdateRating(ratings.f[t]);
            res.m_team[t].m_accId[i] = m_teams[t].m_players[i].m_accId;
        }
    }

    // 5) 上报：单人走消息队列流（0x136/-1 + 二进制），多人直发学校服务器代理。
    // [SweepLeagueMatch2·P59] ORIG 物理块序 SendPacket(855f8c2) 先于
    // StreamPool::Acquire(855f8e4) ⇒ 外层负极性 if(count!=1) 使直发臂落前。
    if (nPlayerCountPerOneTeam_ != 1) {
        GlobalData::s_school_proxy->SendPacket(reinterpret_cast<char*>(&res), 0x80);
    } else {
        // [b28d-solo-CLeagueMatch·S408] P0：ORIG 0x0855f8e4..0x0855f8fb 的
        // Acquire 返回值 %eax 直压 0x4(%esp)（无 st 具名局部槽）。
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("MatchingSystem.cpp", 0x4a8), true);
        // ORIG 每次写经显式 operator*() 调用（同 TimerDispatcher 代理链形）；
        // 直 deref/shim 会丢 2 处 operator*() call（P167 缺失）。
        guard.operator*()->operator<<((int)0x136);
        guard.operator*()->operator<<((int)-1);
        guard.operator->()->put_binary(&res, 0x80);
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    }
}