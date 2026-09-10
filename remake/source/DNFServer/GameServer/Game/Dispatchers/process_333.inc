int Dispatcher_TournamentRewardSelect::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MsgTournamentRewardSelect& req = static_cast<MsgTournamentRewardSelect&>(msg);
    // stTournamentRewardSelect 字段（现 m_pad[0x40] 占位）：+0x04 int errNo
    int err = check_error(user, msg);
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(
            0x5315,
            __PRETTY_FUNCTION__,
            err, 0);
    }

    CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
    if (party == 0)
    {
        return LineFunc(
            0x531c,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    std::vector<int> members;                                // +0x1c7 默认构造
    party->GetValidPartyMember(members);                     // T 0x085bd1dc

    CTournamentDungeonReward& reward =
        reinterpret_cast<CBattle_Field*>(&party->m_battleField)->m_tournamentReward;   // +0x17bc
    // 全员已选卡 → 0x7fffffff（isCardSelectComplete 第二参=卡组类型 2）
    if (reward.isCardSelectComplete(
            (ENUM_TOURNAMENT_REWARD_CARD_TYPE)2, members))
    {
        *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = 0x7fffffff;
        return 0;
    }

    // msg+0xd/msg+0xe 为 signed byte 读入（movsbl）
    const unsigned int slotNo = party->GetMemberSlotNo(user);
    const bool selected = reward.SelectCard(
        (int)slotNo, (char)req.m_0d, (char)req.m_0e);
    if (!selected)
    {
        *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = 0x7fffffff;
    }
    return 0;
