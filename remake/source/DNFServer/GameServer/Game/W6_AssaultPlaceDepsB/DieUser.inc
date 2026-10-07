// ORIG T 0x82ebda6 DieUser(int, int, MSG_DIE_ASSAULT_PLAYER&)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=DIFF 528/529（p167 61/61 CLEAR）
//   残差：N82 帧槽（0xdc vs 0xcc，killerName -0x45 vs ORIG -0x61）+ P63 jmp/nop
//   中继；§6.2 三件套（gap=1 + 调用点差集归零 + 槽位集对齐）判语义完成。
//   本轮复核：ORIG 0x82ebda6 函数头有 killer(-0x2c)/victim(-0x28) 双预清 +
//   killerName(-0x61) 清零；三形实测（头部双预清 390 / 仅 killer 391 / 原位
//   347）以原位最优 ⇒ 保持原位，槽序差归 N82。
// §6.2 巨件核销（b25）：P167 修前 61/66 [多4 PartyInfo::Get 多1 memset]→
// 修后 61/61 [0缺0多]；gap 15→-2（orig 531 / our 533）；修法：①killerName
// memset 改 `= {0}` 初始化形（复现 ORIG 字节/字/双字对齐手工清零展开，
// N-78b 回扫）；②m_parties[i].Get() 改 m_parties[i].m_field00 直读（ORIG
// 内联解引用无 call）；③商比较加 (int) 复现 ORIG divl+jle 有符号比较
// （P196 无符号反）；④hack 命名局部删除（ORIG getHackAnalyzer 返回值
// 直传入参无槽）；⑤pw bool 局部删除（ORIG && 汇聚条件内联无槽）；
// ⑥side/winnerType 改 int 槽（ORIG movsbl 物化后 dword 存）。残差归因
// N82 帧槽（0xdc vs 0xcc）+ P63 jmp/nop 中继（详见
// docs/asm_translation_patterns.md 负例表）。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82）→ 试=①prett2/修正判据缺口 0、
//   strall 串差 0、p167 CLEAR(61=61)；②本 TU 已路由 4.4.6 全量（C6446R），本波
//   反向实测 4.4.7 全量变体 **劣化**（6/40/0/5/12 → 6/39/1/5/12，ae−1/near+1）
//   ⇒ 路由已最优、无反向旋钮；③本波复核 531/533 gap=-2：帧 0xcc vs 0xdc（我方大
//   0x10）+ killerName 槽（ORIG -0x61 vs 我方 -0x45）+ `movl $0x0,-0x2c` 预清位置
//   （ORIG 头部双预清 0x10 之外，三形历史实测以原位最优 ndiff 347）⇒ 维持
//   （新=DIFF 531/533 gap=-2, blocks=58）。
void CAssaultPlace::DieUser(int victimIdx, int killerIdx, MSG_DIE_ASSAULT_PLAYER& msg)
{
    char killerName[0x15] = { 0 };               // ORIG 0x15 字节初始化清零展开（非 memset call）

    if (m_state != 4)
        throw (ENUM_ERROR)0x13;

    CUser* victim = m_assaulters[victimIdx].GetUser();
    if (m_assaulters[victimIdx].GetState() != 5)
        throw (ENUM_ERROR)0x12;

    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82ebda6 在函数头预清 killer/victim
    //   两槽（-0x2c/-0x28）；实测三形（头部双预清 ndiff 390 / 仅 killer 391 /
    //   原位 347）以原位最优 ⇒ 保持原位（槽序差异归 N82）。
    CUser* killer = 0;
    if (killerIdx >= 0 && killerIdx <= 7) {
        killer = m_assaulters[killerIdx].GetUser();
        if (m_assaulters[killerIdx].GetState() != 5) {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xd37, 0);
            trace("CAssaultPlace::DieUser => killer is already died!");
        }
        strncpy(killerName, m_assaulters[killerIdx].GetCharacName(), 0x14);
        if (killer != 0 && msg.m_damage != 0) {
            if (msg.m_damage == 1) {
                killer->getHackAnalyzer()->addServerHackCnt(
                    killer, static_cast<WongWork::ENUM_HACKTYPE>(0x7d), 1, 0, 0);
            }
            if ((int)(msg.m_attackCount / msg.m_damage) > 100000) {  // ORIG divl + jle（有符号比较，P196）
                killer->getHackAnalyzer()->addServerHackCnt(
                    killer, static_cast<WongWork::ENUM_HACKTYPE>(0x7e), 1, 0, 0);
            }
        }
    } else {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xd54, 0);
        trace("CAssaultPlace::DieUser invalid killer index income(%d,%d)", victimIdx, killerIdx);
    }

    if (victim != 0) {
        cUserHistoryLog* hist =
            &victim->m_historyLog;                             // +0x79700
        if (victim->getCurCharacVill() == 7
            && GlobalData::s_power_manager->IsPowerWarEventOn()) {
            hist->DiePvP((ENUM_PVP_HISTORY_REASON)2,
                         msg.m_damage, msg.m_attackCount,
                         (int)(short)msg.m_field08, (int)(short)msg.m_field0a,
                         (int)(short)msg.m_field0c, (int)(short)msg.m_field0e,
                         killerName);
        } else {
            hist->DiePvP((ENUM_PVP_HISTORY_REASON)6,
                         msg.m_damage, msg.m_attackCount,
                         (int)(short)msg.m_field08, (int)(short)msg.m_field0a,
                         (int)(short)msg.m_field0c, (int)(short)msg.m_field0e,
                         killerName);
        }
    }

    if (_IsChangeDieState())
        _ChangeDieState(victimIdx);

    if (victim != 0
        && !(victim->getCurCharacVill() == 7
             && GlobalData::s_power_manager->IsPowerWarEventOn())) {
        PacketGuard packet;
        _MakeUserState(&packet, victim);
        G_GameWorld()->send_all(packet);
    }

    if (killer != 0
        && !(killer->getCurCharacVill() == 7
             && GlobalData::s_power_manager->IsPowerWarEventOn())) {
        if (_IsProcsssChaos())
            _ProcessChaos(victimIdx, killerIdx);
    }

    // [Q8 2026-09-11 试形·回滚] ORIG 0x082ec2c6 `xor $0x1,%eax; test %al,%al;
    //   je <+0x7a6>`（bool 取反物化 + 互补跳）vs 我方 `test %al,%al; jne <+0x7a4>`
    //   —— 两者同条件（_CheckAssaultable()!=0 跳主体），仅 4.1 多一条 `xor $1`
    //   物化。实测 P226 语句表达式 `({ !_CheckAssaultable(); })` **零变化**
    //   （our 533 / diff 174+175 行逐项相同、四数不变）⇒ 回滚，形态差维持 N。
    // [DNF-FIX-AUDIT 2026-10-07] ORIG 0x082ec2c1..0x082ec2cb：`call _CheckAssaultable;
    //   xor $0x1,%eax; test %al,%al; je 0x082ec54c`，0x082ec54c 即函数尾声（add/pop/ret），
    //   即「双方仍有人（可继续乱入）⇒ 直接返回；否则走结算并 EndAssault」。
    //   上方旧注释的「同条件」判词有误：旧写法 `if (!_CheckAssaultable()) return;`
    //   极性相反——双方都还有人时就把乱入战结束，一方全灭反而不结算。
    if (_CheckAssaultable())
        return;

    if (victim != 0 && killer != 0
        && victim->getCurCharacVill() == 7
        && GlobalData::s_power_manager->IsPowerWarEventOn()) {
        int winCnt = 0, loseCnt = 0, aliveCnt = 0;
        int side = 0;          // ORIG local_28 int 槽（movsbl 物化后 dword 存）
        int winnerType = 0;
        if (killerIdx >= 0 && killerIdx <= 7) {
            side = (int)(signed char)killer->getPowerSide();
            winnerType = 1;
        } else {
            side = (int)(signed char)victim->getPowerSide();
            winnerType = 2;
        }
        if (_CheckPowerWarInPlayer((ENUM_POWER_SIDE_TYPE)side,
                                   (ENUM_POWER_WAR_WINNER_OR_LOSER)winnerType,
                                   winCnt, loseCnt, aliveCnt)) {
            if (winnerType == 1)
                _SendPowerWarPointToGuildServer(killer,
                                                (ENUM_POWER_WAR_WINNER_OR_LOSER)winnerType,
                                                winCnt, loseCnt, aliveCnt);
            else
                _SendPowerWarPointToGuildServer(victim,
                                                (ENUM_POWER_WAR_WINNER_OR_LOSER)winnerType,
                                                winCnt, loseCnt, aliveCnt);
            PacketGuard packet;
            _MakeUserState(&packet);
            G_GameWorld()->send_all(packet);
            _SendStraightVictories(killer);
        }
    }

    if (victim != 0
        && victim->getCurCharacVill() == 7
        && GlobalData::s_power_manager->IsPowerWarEventOn()) {
        CParty* party = static_cast<CParty*>(victim->GetParty());
        if (party != 0)
            party->resetStraightVictories();
        else
            victim->resetStraightVictories();
    }

    m_state = (ENUM_ASSAULT_STATE)5;
    // ORIG 直读 m_parties[i].m_field00（+0x12c/+0x130），无 Get() call
    if ((reinterpret_cast<CParty*>(m_parties[0].m_field00)) != 0)
        (reinterpret_cast<CParty*>(m_parties[0].m_field00))->SetAssaultState('\x05');
    if ((reinterpret_cast<CParty*>(m_parties[1].m_field00)) != 0)
        (reinterpret_cast<CParty*>(m_parties[1].m_field00))->SetAssaultState('\x05');
    EndAssault();
}