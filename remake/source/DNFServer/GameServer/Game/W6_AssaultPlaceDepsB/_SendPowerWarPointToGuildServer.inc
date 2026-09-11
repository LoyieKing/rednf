// ORIG T 0x82eceb2 _SendPowerWarPointToGuildServer(CUser*, ENUM_POWER_WAR_WINNER_OR_LOSER, int, int, int)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=DIFF 333/329（p167 36/36 CLEAR）
//   修法：①删独立 side 局部（P0：ORIG 直写 pkt.m_powerSide@-0x5f=pkt+0xa，
//   后续一律读该成员）；②删 playTime 具名局部（内联为 EndPvP 第三实参，
//   ORIG 经 %ebx 直传）；③winPoint/losePoint 改 int 乘法取低字节（消 4 条
//   movsbl 截断链，ORIG 0x82ed0xx 为 `imul; mov %al`）；④winAliveCnt 形参写回、
//   直写 pkt.m_characNo[idx]、P227 trace 临时链、u 函数头预清（ndiff 260→202）。
//   残差：N95/N82 槽序（4.4 按首次使用序给 u 最深 -0x34 vs ORIG -0x2c，且
//   winPoint/losePoint 与 u 的相对序相反）+ N86b（`losePoint += bonus` 4.4 恒
//   折内存 add，ORIG 为 movzbl/lea/mov 四条形）。
// [E02·R5 fresh 实测] verdict=DIFF 335/329（our 331→329）；p167 调用点差集
//   36=36 CLEAR（MISSING/EXTRA 仅 Packet_Guild_Change_Power_War_Point 与
//   cMyTrace 的 C1/C2 同址别名 = P93 良性）⇒ 语义缺口 0，残差纯形态（槽序）。
//   本轮修法⑤（P0/P52/P134）：删具名 `CGuildServerProxy* proxy` 局部，改链式
//   `GlobalData::s_guild_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE)
//    ->SendPowerWarPoint(pkt);` —— ORIG 0x82ed2xx 为 `call GetServerProxy;
//   lea -0x69(%ebp),%edx; mov %edx,0x4(%esp); mov %eax,(%esp); call
//   SendPowerWarPoint`（结果 %eax 直通 receiver 实参，无栈往返）；我方旧形多
//   `mov %eax,-0x20(%ebp)` + 回读一对（−2 insn）。
//   反劣实验记录（勿重试）：删 cMyTrace 实参位 `static_cast<int>(winPoint)` /
//   `static_cast<int>(losePoint)` 显式提升 ⇒ our 329→331（+2）；此处 cast 是
//   uchar→int 提升必需形（ORIG `movzbl -0x3d,%eax; movzbl %al,%edi`），与
//   C28L0「vararg 冗余 cast 反劣」方向相反，非冗余。
// [E11·语义修复 + codegen] verdict=DIFF 335/335（改前 335/329，gap −6→0）；
//   语义：ORIG 的配对是 m_losePoint(+0x2c) ← multiple×(winCnt−winAliveCnt)
//   （0x082ed036 段，日志行 0xf62；败者分支 0x082ed25f 读取）、m_winPoint(+0x2b)
//   ← multiple×loseCnt（0x082ed0b4 段，日志行 0xf6a；胜者分支 0x082ed1c4 读取，
//   且 _VictoryBonus 结果加于其上 0x082ed0e1）。改前重建把两式互换 ⇒ 发给胜者/
//   败者的积分值与包内 +0x2b/+0x2c 内容均与 ORIG 相反（真 bug，由整函数逐条对齐
//   反证）。字段锁定：ORIG ctor 0x082f070c = PacketHeader(this,0x6a4,0x2d) 且
//   movb $0 写 +0xa/+0x2b/+0x2c ⇒ 对象 0x2d 字节、栈 [-0x69,-0x3d] ⇒ -0x3e/-0x3d
//   即成员（无独立局部）。同刻删两个具名积分局部（P0/P183）⇒ our 329→335。
//   残差：仅败者分支 `pkt.m_characNo[loserIdx + 4]` 的 lea 时机（ORIG 先提升
//   loserIdx 入 %ebx、call 后 `lea 0x4(%ebx),%edx`；4.4 提前 lea 入 %ebx）⇒
//   P226/P331 书写序族，全函数唯一 1 处。
void CAssaultPlace::_SendPowerWarPointToGuildServer(CUser* user,
                                                    ENUM_POWER_WAR_WINNER_OR_LOSER winner,
                                                    int winCnt, int loseCnt,
                                                    int winAliveCnt)
{
    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82ecebe 函数最头 `movl $0x0,-0x2c`
    //   预清 u 槽（在 user==0 检查之前）。
    CUser* u = 0;
    if (user == 0) {
        LogManager::logFormat(
            1, "Assault.cpp",
            __PRETTY_FUNCTION__,
            0xf4c, "Invalid User pointer income (%d, %d, %d, %d)",
            winner, winCnt, loseCnt, winAliveCnt);
        return;
    }

    Packet_Guild_Change_Power_War_Point pkt;     // ORIG ctor W 0x082f070c
    // [b28d-solo-W6_AssaultPlaceDepsB] P0：ORIG 无独立 side 局部——直接写
    //   pkt.m_powerSide（-0x5f = pkt 基址 -0x69 + 0xa），后续一律读该成员。
    if (winner == 1) {
        pkt.m_powerSide = static_cast<char>(user->getPowerSide());
    } else {
        pkt.m_powerSide = (user->getPowerSide() == 1) ? 2 : 1;
    }

    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82ecfa1 直接把 winCnt 写回形参
    //   winAliveCnt 槽（`mov 0x14(%ebp),%eax; mov %eax,0x1c(%ebp)`）——无 aliveCnt
    //   具名局部（我们多一个 -0x28 槽使帧 +0x24、ndiff 24）。
    if (winCnt - winAliveCnt < 0) {
        LogManager::logFormat(
            1, "Assault.cpp",
            __PRETTY_FUNCTION__,
            0xf5c, "Power War Point Computer Error( winCnt : %d, winAliveCnt : %d )",
            winCnt, winAliveCnt);
        winAliveCnt = winCnt;                    // ORIG: param_6 = param_4
    }

    // [E11·语义修正] ORIG 的「公式 ↔ 字段/用途」配对与此前重建相反（真 bug）：
    //   m_losePoint(+0x2c) ← multiple × (winCnt - winAliveCnt)（0x082ed036 段：
    //     `mov 0xa67c(%eax),%eax; mov 0x14(%ebp),%ecx; sub 0x1c(%ebp),%ecx;
    //      imul; mov %al,-0x3d`，日志行 0xf62），败者分支
    //     （0x082ed25f `movzbl -0x3d(%ebp),%eax`）读取；
    //   m_winPoint(+0x2b) ← multiple × loseCnt（0x082ed0b4 段：`imul 0x18(%ebp),%eax;
    //     mov %al,-0x3e`，日志行 0xf6a），胜者分支（0x082ed1c4 `movzbl -0x3e`）读取，
    //     且 _VictoryBonus 结果经 `movzbl -0x3e,%edx; lea (%edx,%eax,1); mov %al,-0x3e`
    //     （0x082ed0d7..0x082ed0e1）加在其上。
    //   字段锁定：ORIG ctor 0x082f070c = PacketHeader(this,0x6a4,0x2d) + movb $0 写
    //     +0xa/+0x2b/+0x2c ⇒ 对象 0x2d 字节，栈 [-0x69,-0x3d] ⇒ pkt+0x2b=-0x3e、
    //     pkt+0x2c=-0x3d；无独立积分局部，故直接写成员。
    //   P48/P183：ORIG 每次读取 multiplicand 均重调 G_CDataManager 并读
    //     m_powerParameter(+0xa67c)（六点），禁具名缓存。
    if (G_CDataManager()->m_powerParameter.m_powerWarPointMultiple *
            (winCnt - winAliveCnt) > 0xff) {              // imul; cmp $0xff; setg
        LogManager::logFormat(
            1, "Assault.cpp",
            __PRETTY_FUNCTION__,
            0xf62,
            "Power War Point Computer Error( winCnt : %d, winAliveCnt : %d , multiple n : %d)",
            winCnt, winAliveCnt,
            G_CDataManager()->m_powerParameter.m_powerWarPointMultiple);
        pkt.m_losePoint = 0xff;                      // ORIG 0x082ed016 movb $0xff,-0x3d
    } else {
        // ORIG 0x082ed036：int 乘法后取低字节（低 8 位等价，无 movsbl 截断链）。
        pkt.m_losePoint = static_cast<unsigned char>(
            G_CDataManager()->m_powerParameter.m_powerWarPointMultiple *
            (winCnt - winAliveCnt));
    }
    if (G_CDataManager()->m_powerParameter.m_powerWarPointMultiple * loseCnt > 0xff) {
        LogManager::logFormat(
            1, "Assault.cpp",
            __PRETTY_FUNCTION__,
            0xf6a,
            "Power War Point Computer Error( winCnt : %d, winAliveCnt : %d , multiple n : %d )",
            winCnt, winAliveCnt,
            G_CDataManager()->m_powerParameter.m_powerWarPointMultiple);
        pkt.m_winPoint = 0xff;                   // ORIG 0x082ed09d movb $0xff,-0x3e
    } else {
        pkt.m_winPoint = static_cast<unsigned char>(
            G_CDataManager()->m_powerParameter.m_powerWarPointMultiple * loseCnt);
    }

    int bonus = _VictoryBonus(user, pkt.m_powerSide);
    // [b28d-L28 round2 · P322② 实测负例] ORIG 0x82ed0d7 段为读-加-写四条形
    // （`movzbl -0x3e,%edx; mov -0x28,%eax; lea (%edx,%eax,1),%eax; mov %al,-0x3e`），
    // 但加 `volatile` 后全部使用点（5 处读 + pkt 写入）都强制重载 ⇒ ourN 331→339
    // （gap +4→−4、块 5→11）反劣，已回退。P322② 仅适用于「该累加器无其它使用点」场景。
    pkt.m_winPoint = pkt.m_winPoint + bonus;

    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82ed235 直接写 pkt.m_characNo[idx]
    //   （`mov %eax,-0x5e(%ebp,%ebx,4)` = pkt 基址 -0x69 + 0xb）——无 winnerNo/
    //   loserNo 局部数组、无末尾回填循环（我们此前多 8 清零 + 拷贝段）。
    int winnerIdx = 0;
    int loserIdx = 0;
    for (int i = 0; i <= 7; ++i) {
        // P223：ORIG `test %al,%al; jne <skip>` 正极性 guard-continue。
        if (m_assaulters[i].Empty())
            continue;
        u = m_assaulters[i].GetUser();
        if (u == 0)
            continue;
        // P0/P227：ORIG 无 now/hist/winFlag/playTime 具名局部（值经 %ebx/%eax
        //   直传）；playTime 内联为 EndPvP 第三实参（右起求值先算）。
        u->m_historyLog.EndPvP(
            (ENUM_PVP_HISTORY_REASON)1 /*ORIG 字面 1*/,
            static_cast<int>(static_cast<unsigned char>(pkt.m_powerSide))
                == static_cast<int>(static_cast<signed char>(u->getPowerSide())),
            static_cast<unsigned int>(
                GlobalData::s_systemTime_.getCurSec() - m_abuseSec));  // +0x13c
        u->IncPowerWarAssaultCount();
        if (static_cast<int>(static_cast<unsigned char>(pkt.m_powerSide))
            == static_cast<int>(static_cast<signed char>(u->getPowerSide()))) {
            u->IncPowerWarAssaultVictoryCount();
            u->IncreasePowerWarPoint(static_cast<short>(pkt.m_winPoint));
            if (winnerIdx > 3) {
                LogManager::logFormat(
                    1, "Assault.cpp",
                    __PRETTY_FUNCTION__,
                    0xf94,
                    "Power War Point Computer Error( winner index : %d, loser index : %d )",
                    winnerIdx, loserIdx);
            } else {
                pkt.m_characNo[winnerIdx] =
                    static_cast<unsigned int>(u->get_charac_no(-1));
                winnerIdx++;
                _SendPowerWarAssaultResultToUser(u, pkt.m_winPoint);
            }
        } else {
            u->IncreasePowerWarPoint(static_cast<short>(pkt.m_losePoint));
            if (loserIdx > 3) {
                LogManager::logFormat(
                    1, "Assault.cpp",
                    __PRETTY_FUNCTION__,
                    0xfa4,
                    "Power War Point Computer Error( winner index : %d, loser index : %d )",
                    winnerIdx, loserIdx);
            } else {
                pkt.m_characNo[loserIdx + 4] =
                    static_cast<unsigned int>(u->get_charac_no(-1));
                loserIdx++;
                _SendPowerWarAssaultResultToUser(u, pkt.m_losePoint);
            }
        }
    }

    // P227 全表达式临时链：ORIG 0x82ed30e 在 ctor 前把 6 个 vararg 求值入
    //   %edi/%esi/%ebx 跨 ctor 保活。
    cMyTrace(__PRETTY_FUNCTION__, 0xfad, 0)(
        "win side(%d), win pp(%d, %d), lose pp(%d, %d), winAliveCnt(%d)",
        static_cast<int>(static_cast<unsigned char>(pkt.m_powerSide)), winCnt,
        // [E02·R5 实验已回退] 删 (int) cast 实测反劣（our 329→331，+2 条）：
        //   ORIG `movzbl -0x3d,%eax; movzbl %al,%edi` 的提升直入形由**显式**
        //   窄化 cast 链复现（与 C28L0 的「vararg 冗余 cast 反劣」方向相反，
        //   因本处 cast 是 uchar→int 提升必需形，非冗余）。
        static_cast<int>(pkt.m_winPoint), loseCnt,
        static_cast<int>(pkt.m_losePoint), winAliveCnt);

    // [E02·R5 修·P0/P52/P134] 删具名 proxy 局部：ORIG 0x82ed2xx 段为
    //   `call GetServerProxy; lea -0x69(%ebp),%edx; mov %edx,0x4(%esp);
    //    mov %eax,(%esp); call SendPowerWarPoint`（结果 %eax 直通 receiver，
    //   无槽往返）。我方具名局部多 `mov %eax,-0x20(%ebp)` + 回读一对。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE)
        ->SendPowerWarPoint(pkt);

    GlobalData::s_power_manager->IncreaePowerWarPointInChannel(
        pkt.m_powerSide, static_cast<int>(pkt.m_winPoint));
    GlobalData::s_power_manager->IncreaePowerWarPointInChannel(
        (pkt.m_powerSide == 1) ? 2 : 1, static_cast<int>(pkt.m_losePoint));
}