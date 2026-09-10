void CParty::die_user(CUser* user)
{
    int slot = -1;                                      // -0x10
    for (int i = 0; i <= 3; ++i)                        // -0xc
    {
        if (_checkValidUser(i))
        {
            if (m_member[i].m_pUser == user)
            {
                ++m_member[i].m_pUser->m_character.m_f04;   // +0x8e038 死亡计数
                slot = i;
                HistoryLog::WriteDie(
                    reinterpret_cast<FILE*>(user->m_field796f8));
                break;
            }
        }
    }

    PacketGuard packet;                                 // -0x1c
    packet.put_header(1, 0x2b);
    packet.put_byte(0);
    if (slot == -1)
    {
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        // P248 档②/P292：ORIG 0x085a794a `movzbl 0x380(%edx,%eax,1); xor $1,%eax;
        // test %al,%al; je big` = **bool 字段取反三件套** ⇒ 裸 `if (!field)` 正形
        // （字段 m_arr54 已是 bool[4]；旧 `unsigned char live=…; live^=1; if(live)`
        // 两语句形落栈化三段 movb/xorb/cmpb 并顶浅整帧 4B）。
        if (!m_battleData.m_arr54[slot])   // +0x380[slot]==0 → 0x12 回执
        {
            packet.put_byte(0x12);
            packet.finalize(true);
            user->Send(packet);
        }
        else                            // +0x380[slot]==1 → 死亡清算
        {
            set_charac_live(user, (ENUM_USER_DUNGEON_STATE)0);
            if (user->GetRevivalTime() >
                GlobalData::s_systemTime_.getCurSec())
            {
                set_charac_live(user, (ENUM_USER_DUNGEON_STATE)2);
                user->SetRevivalTime(0);
            }
            user->SetUseAPCPotionTime(0);

            if (check_allmember_die())
            {
                if (!m_tournamentVictory)
                {
                    if (m_battleField.m_bloodState != 1)
                        send_dungeon_fail_msg();
                }
            }
        }
    }
