bool CBossTower::InsertRewardExp()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        if ((member == 0) || (m_party->checkValidUser(i) != 1))
            continue;

        int bonusExp = 0;                        // ebp-0x18
        int extraFlag = 0;                       // ebp-0x1c
        bool gained = false;
        gained = member->gain_exp_sp(
            m_rewardExp[i], bonusExp, extraFlag,
            (CUser::eExpAddReason)2, 0, false);
        if (gained)
        {
            member->SendNotiPacket((CUser::eSendTarget)2,
                                   (ENUM_NOTIPACKET)2, 1);
        }
    }
    return 1;
