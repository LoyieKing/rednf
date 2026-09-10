bool CParty::BloodHandleDieUser(CUser* user)
{
    int local_10 = 0;
    for (local_10 = 0; local_10 <= 3; ++local_10)
    {
        // [b28d-53] ORIG 0x085b8e8c 段为正向包裹（`call; test %al,%al; je join`），
        // `if(!f()) continue;` 会多产 xor $1（P248 反向）。
        if (checkValidUser(local_10))
        {
            if (get_user(local_10) == user)
            {
                if (m_battleData.m_arr54[local_10] != 1)
                {
                    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2b, 0x12);
                    return false;
                }
                set_charac_live(user, (ENUM_USER_DUNGEON_STATE)0);
                break;
            }
        }
    }

    if (check_allmember_die())
    {
        if (m_battleField.m_bloodState != 1)
        {
            // [b28d-53 P24/P52] ORIG 先求第 5 参 gen_timer_key 并入 %esi 跨后续
            // call 保活 ⇒ 实参内联（具名 key 局部落槽 +2 条并致帧 0x30→0x34）。
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                        GetPartyIndex(), (TIMER_MESSAGE)0x22,
                                        10, gen_timer_key((TIMER_MESSAGE)0x22),
                                        (unsigned int)0x22);
        }
    }
    return true;
