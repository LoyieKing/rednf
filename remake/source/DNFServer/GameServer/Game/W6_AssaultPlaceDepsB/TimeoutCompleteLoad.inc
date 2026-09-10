bool CAssaultPlace::TimeoutCompleteLoad()
{
    if (m_state != 2)
        throw (ENUM_ERROR)0x13;
    for (int i = 0; i <= 7; ++i) {
        // [b28d-solo-W6_AssaultPlaceDepsB] P223/P292：ORIG 0x82ea3xx 对 Empty 用
        //   `test %al,%al; jne <continue>`（正极性），`!Empty()` 在 4.4 恒多 xor $1。
        if (m_assaulters[i].Empty())
            continue;
        if (m_assaulters[i].GetState() != 2) {
            if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
                GlobalData::s_power_manager->GetPowerWarLog()->IncLoadingTimeOutCountSecede();
                // [b28d-solo-W6_AssaultPlaceDepsB] P227 全表达式临时链：ORIG 在
                //   cMyTrace ctor 前求 vararg 并以 %ebx 跨 ctor 保活。
                cMyTrace(__PRETTY_FUNCTION__, 0xa18, 0)(
                    "POWER WAR LOADING SECEDE");
                for (int j = 0; j <= 7; ++j) {
                    if (m_assaulters[j].Empty())
                        continue;
                    // ORIG 蓝/红两分支各调一次 GetUser（P48 静态调用点各一）
                    if (m_assaulters[j].GetTeam() == 0) {
                        cMyTrace(__PRETTY_FUNCTION__, 0xa20, 0)(
                            "BLUE TEAM : %s",
                            m_assaulters[j].GetUser()->getCurCharacName());
                    } else {
                        cMyTrace(__PRETTY_FUNCTION__, 0xa22, 0)(
                            "RED TEAM : %s",
                            m_assaulters[j].GetUser()->getCurCharacName());
                    }
                }
                // ORIG 直接把 GetUser(i) 指针作为 %s 实参传给 trace（保留原样）。
                cMyTrace(__PRETTY_FUNCTION__, 0xa25, 0)(
                    "SECEDE : %s",
                    reinterpret_cast<const char*>(m_assaulters[i].GetUser()));
            }
            CUser* u = m_assaulters[i].GetUser();
            // P0：ORIG 0x82ea56x 短路汇聚值直测（无 pwVill 槽）；ORIG 无 u!=0
            // 判空（P171 过度防御禁止——直接解引用 u）。
            if (GlobalData::s_power_manager->IsPowerWarEventOn()
                && u->getCurCharacVill() == 7) {
                PacketGuard packet;
                packet.put_header(0, 0xe6);
                packet.put_byte(1);
                packet.finalize(true);
                u->Send(packet);
            }
        }
    }
    // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82ea63b：底部条件循环
    //   （`i=0; jmp cond; body; ++i; cmpl $7; setle; jne body`），i>7 分支在
    //   循环之后——写 `for(;;){ if(i>7){...return true;} }` 4.4 恒条件置顶。
    for (int i = 0; i <= 7; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        if (m_assaulters[i].GetState() != 2) {
            LeaveAssaultPlace(i, true, false);
            if (m_state == 6)
                return false;                    // ORIG 提前返回 0
        }
    }
    DoConnectP2P();
    // ORIG 直读 m_parties[i].m_field00（空判定 +0x12c/0x130 指针），不走 Get()
    if ((reinterpret_cast<CParty*>(m_parties[0].m_field00)) != 0)
        (reinterpret_cast<CParty*>(m_parties[0].m_field00))->SetAssaultState('\x03');
    if ((reinterpret_cast<CParty*>(m_parties[1].m_field00)) != 0)
        (reinterpret_cast<CParty*>(m_parties[1].m_field00))->SetAssaultState('\x03');
    return true;                                 // ORIG mov $0x1,%eax
