void CAssaultPlace::ChangeHp(CUser* user, int hp)
{
    int idx = _GetUserIndex(user, true);
    // P198-②：ORIG 对 CAssaulter::ChangeHp 返回 `test %al,%al; je` 寄存器直测（无槽）；
    // `bool changed` 具名会额外 `mov %al,-off; cmpb $0,-off`，内联直测对齐。
    if (m_assaulters[idx].ChangeHp(hp)) {
        PacketGuard packet;
        _MakeChangeHp(&packet, idx, hp);
        // [b28d-solo-W6_AssaultPlaceDepsB] P0：ORIG 0x82ed7b6 短路汇聚值直测
        //   `test %al,%al; je`（无槽）——具名 bool 局部会多 store+cmpb 两条。
        if (GlobalData::s_power_manager->IsPowerWarEventOn()
            && user->getCurCharacVill() == 7) {
            CParty* party = static_cast<CParty*>(user->GetParty());
            // P59 正极性：ORIG `cmpl $0; je <Send 块>` ⇒ party!=0 走 fall-through。
            if (party != 0)
                party->send_to_party(packet);
            else
                user->Send(packet);
        } else {
            _SendPacketToArea(packet);
        }
    }
    if (m_state == 5 || m_state == 6) {
        m_assaulters[idx].SetState(static_cast<ENUM_ASSAULT_STATE>(8));
        bool allDead = true;
        for (int i = 0; i <= 7; ++i) {
            // P223：ORIG `test %al,%al; jne <nop 岛>` 正极性 guard-continue。
            if (m_assaulters[i].Empty())
                continue;
            if (m_assaulters[i].GetState() != 8)
                allDead = false;
        }
        if (allDead) {
            // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82ed92a 直读 m_field00
            // （`mov 0x12c(%eax),%eax`，无 Get() call）。
            if ((reinterpret_cast<CParty*>(m_parties[0].m_field00)) != 0)
                (reinterpret_cast<CParty*>(m_parties[0].m_field00))->SetAssaultState('\a');
            if ((reinterpret_cast<CParty*>(m_parties[1].m_field00)) != 0)
                (reinterpret_cast<CParty*>(m_parties[1].m_field00))->SetAssaultState('\a');
            m_state = (ENUM_ASSAULT_STATE)7;
            CloseAssault();
        }
    }
