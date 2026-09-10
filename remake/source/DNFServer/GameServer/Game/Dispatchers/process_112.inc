int Dispatcher_TutorialLevelUp::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    if (user == 0 || user->get_state() != 5)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    if (user->get_charac_level() != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    CBattle_Field& bf =
        *&party->m_battleField;
    if (bf.get_cur_map() != 0xee49 && bf.get_cur_map() != 0xee51 &&
        bf.get_cur_map() != 0xee58)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    char itemState = 0;                          // -0x0d
    int needExp = 0;                             // -0x14
    if (!user->CalLevelUpItemState(itemState, needExp, 1, 0x3b))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    int spBefore = 0;                            // -0x18（ORIG -0x18/-0x1c 置 0）
    int spAfter = 0;                             // -0x1c
    int expBonus1;                               // -0x20（ORIG 未出线）
    int expBonus2;                               // -0x24（ORIG 未出线）
    user->rewardExp(needExp, spBefore, spAfter, expBonus1, expBonus2,
                    (eExpAddReason)0xd, true);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x1e4);
    return 0;
