void CParty::ReturnToVillage()
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS put_byte/put_short/GameWorld::send_all(PacketGuard,CUser*)/
    // get_member_count 4 项 + EXTRA new/delete（P90）。@0x85aca60
    reinterpret_cast<CBattle_Field*>(&m_battleField)->setBloodState((ENUM_BLOOD_STATE)0);
    PacketGuard packet;
    packet.put_header(0, 2);
    if (m_autoCreated == 0)
    {
        // ORIG 0x085acaad/0x085acac2：put_byte(0) + put_short(get_member_count())
        // （旧码漏此两句 —— 真缺口）。
        packet.put_byte(0);
        packet.put_short(get_member_count());
        // ORIG 0x085acacd `mov $0x40,%edx; rep stos` = 64 dword = **0x100 字节**
        // （P214③/P116：rep stos 立即数是 dword 次数不是字节长度）⇒ 缓冲 char[0x100]，
        // 且 -0x138+0x100 = -0x38 恰接 PacketGuard 槽。
        char names[0x100] = {0};
        _getMemberNames(names);
        for (int i = 0; i <= 3; ++i)
        {
            if (_checkValidUser(i))
            {
                if (m_battleField.m_pDungeon != 0)
                {
                    // [2026-09-10 真 bug 修正] ORIG 0x085acb5x 的 receiver 是
                    // `mov 0x8(%ebp),%edx; mov 0x74(%edx),%edx; add $0x79700,%edx`
                    // = **this+0x74 = m_manager**（CParty.h:732），不是
                    // m_member[i].m_pUser（0x78+i*0x18）。且三实参按 cdecl 右→左
                    // 求值并用 %edi/%esi/%ebx 跨调用保活（P329/P52 单语句形）。
                    m_manager->m_historyLog.LeaveDungeon(
                        m_battleField.m_pDungeon->GetDungeonName(),
                        reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_diff(),
                        _getMemberNames(names),
                        m_battleField.m_bloodState);
                }
                if (m_battleField.m_pDungeon != 0)
                {
                    bool b;
                    if (!IsSinglePlay() || !m_manager->isDungeonClearHackCheck())
                    {
                        b = false;
                    }
                    else
                    {
                        b = true;
                    }
                    if (b)
                    {
                        CUser* mgr = m_manager;
                        m_manager->getHackAnalyzer()->addServerHackCnt(
                            mgr, (WongWork::ENUM_HACKTYPE)0x19c, 1, 0, 0);
                    }
                }
                CUser* u = m_member[i].m_pUser;
                G_GameWorld()->out_from_dungeon(u);
                m_member[i].m_pUser->make_basic_info((char*)&packet, 0);
            }
        }
        set_state(1);
        battle_data_reset();
        packet.finalize(true);
        if (IsExistInvisible())
        {
            send_to_party(packet);
        }
        else
        {
            // ORIG 0x085acf93 二参版 send_all(PacketGuard&, CUser*)（p167 MISS）。
            G_GameWorld()->send_all(packet, m_manager);
        }
    }
    else
    {
        if (m_battleField.m_pDungeon != 0)
        {
            char names2[0x100] = {0};   // ORIG rep stos 0x40 dword = 0x100 字节（+0x32b）
            // 同首分支：cdecl 右→左求值 + 寄存器保活（P329/P52 单语句形）。
            m_manager->m_historyLog.LeaveDungeon(
                m_battleField.m_pDungeon->GetDungeonName(),
                reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_diff(),
                _getMemberNames(names2),
                m_battleField.m_bloodState);
            bool b;
            if (!IsSinglePlay() || !m_manager->isDungeonClearHackCheck())
            {
                b = false;
            }
            else
            {
                b = true;
            }
            if (b)
            {
                CUser* mgr = m_manager;
                m_manager->getHackAnalyzer()->addServerHackCnt(
                    mgr, (WongWork::ENUM_HACKTYPE)0x19c, 1, 0, 0);
            }
        }
        CUser* mgr2 = m_manager;
        G_GameWorld()->out_from_dungeon(mgr2);
        packet.put_byte(0);
        packet.put_short(1);
        short partyIdx = m_manager->GetPartyIndex();
        int userState = m_manager->get_state();
        unsigned int accId = m_manager->get_acc_id();
        char* accIdStr = NumberToString(accId, 0);
        // ORIG 0x085acc59 具名 cMyTrace（C1，无 placement new/delete/EH landing）。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2714, 0);
        trace(
            "<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)",
            accIdStr, userState, (int)partyIdx);
        m_manager->SetPartyIndex(-1);
        if (m_manager->getMoveSpace() != (UserSpace::T)1)
        {
            m_manager->set_state(ch_state3);
        }
        m_manager->make_basic_info((char*)&packet, 0);
        destroy();
        packet.finalize(true);
        CUser* gm = getManager();
        if (gm->getMoveSpace() == (UserSpace::T)1)
        {
            send_to_party(packet);
        }
        else
        {
            G_GameWorld()->send_all(packet);
        }
    }
    SetEPLPState(0);
