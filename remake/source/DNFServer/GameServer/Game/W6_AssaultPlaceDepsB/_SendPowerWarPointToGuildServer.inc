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

    unsigned char winPoint;
    // P48/P183：ORIG 每次读取 multiplicand 均重调 G_CDataManager 并读
    // m_powerParameter(+0xa67c)（+0xf5/+0x11b/+0x16a/+0x187/+0x1a2/+0x1f1 六点），
    // 禁具名缓存。
    if (G_CDataManager()->m_powerParameter.m_powerWarPointMultiple *
            (winCnt - winAliveCnt) > 0xff) {              // imul; cmp $0xff; setg
        LogManager::logFormat(
            1, "Assault.cpp",
            __PRETTY_FUNCTION__,
            0xf62,
            "Power War Point Computer Error( winCnt : %d, winAliveCnt : %d , multiple n : %d)",
            winCnt, winAliveCnt,
            G_CDataManager()->m_powerParameter.m_powerWarPointMultiple);
        winPoint = 0xff;
    } else {
        // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82ed0xx 为 int 乘法后取低字节
        //   （`mov 0xa67c(%eax),%eax; mov %ecx,%ebx; sub %edx,%ebx; imul %edx,%eax;
        //   mov %al,-0x3d`）——无 movsbl 截断链（低 8 位乘法等价）。
        winPoint = static_cast<unsigned char>(
            G_CDataManager()->m_powerParameter.m_powerWarPointMultiple *
            (winCnt - winAliveCnt));
    }
    // [b28d-L28 round2 · P322② 实测负例] ORIG 0x82ed1cb 段为读-加-写四条形
    // （`movzbl -0x41,%edx; mov -0x41,%eax; lea (%edx,%eax,1),%eax; mov %al,-0x41`），
    // 但加 `volatile` 后**全部使用点**（5 处读 + pkt 写入）都强制重载 ⇒ ourN 331→339
    // （gap +4→−4、块 5→11）反劣，已回退。P322② 仅适用于「该累加器无其它使用点」场景。
    unsigned char losePoint;
    if (G_CDataManager()->m_powerParameter.m_powerWarPointMultiple * loseCnt > 0xff) {
        LogManager::logFormat(
            1, "Assault.cpp",
            __PRETTY_FUNCTION__,
            0xf6a,
            "Power War Point Computer Error( winCnt : %d, winAliveCnt : %d , multiple n : %d )",
            winCnt, winAliveCnt,
            G_CDataManager()->m_powerParameter.m_powerWarPointMultiple);
        losePoint = 0xff;                        // movb $0xff
    } else {
        losePoint = static_cast<unsigned char>(
            G_CDataManager()->m_powerParameter.m_powerWarPointMultiple * loseCnt);
    }

    int bonus = _VictoryBonus(user, pkt.m_powerSide);
    losePoint = losePoint + bonus;               // ORIG: movzbl/lea/mov %al 四条形

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
            u->IncreasePowerWarPoint(static_cast<short>(winPoint));
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
                _SendPowerWarAssaultResultToUser(u, winPoint);
            }
        } else {
            u->IncreasePowerWarPoint(static_cast<short>(losePoint));
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
                _SendPowerWarAssaultResultToUser(u, losePoint);
            }
        }
    }

    pkt.m_winPoint = winPoint;
    pkt.m_losePoint = losePoint;

    // P227 全表达式临时链：ORIG 0x82ed30e 在 ctor 前把 6 个 vararg 求值入
    //   %edi/%esi/%ebx 跨 ctor 保活。
    cMyTrace(__PRETTY_FUNCTION__, 0xfad, 0)(
        "win side(%d), win pp(%d, %d), lose pp(%d, %d), winAliveCnt(%d)",
        static_cast<int>(static_cast<unsigned char>(pkt.m_powerSide)), winCnt,
        // [E02·R5 实验已回退] 删 (int) cast 实测反劣（our 329→331，+2 条）：
        //   ORIG `movzbl -0x3d,%eax; movzbl %al,%edi` 的提升直入形由**显式**
        //   窄化 cast 链复现（与 C28L0 的「vararg 冗余 cast 反劣」方向相反，
        //   因本处 cast 是 uchar→int 提升必需形，非冗余）。
        static_cast<int>(winPoint), loseCnt,
        static_cast<int>(losePoint), winAliveCnt);

    // [E02·R5 修·P0/P52/P134] 删具名 proxy 局部：ORIG 0x82ed2xx 段为
    //   `call GetServerProxy; lea -0x69(%ebp),%edx; mov %edx,0x4(%esp);
    //    mov %eax,(%esp); call SendPowerWarPoint`（结果 %eax 直通 receiver，
    //   无槽往返）。我方具名局部多 `mov %eax,-0x20(%ebp)` + 回读一对。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE)
        ->SendPowerWarPoint(pkt);

    GlobalData::s_power_manager->IncreaePowerWarPointInChannel(
        pkt.m_powerSide, static_cast<int>(winPoint));
    GlobalData::s_power_manager->IncreaePowerWarPointInChannel(
        (pkt.m_powerSide == 1) ? 2 : 1, static_cast<int>(losePoint));
