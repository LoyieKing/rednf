void CTournamentDungeonReward::MakeReward(
    const std::map<int, TournamentRewardRate>* rates,
    const TournamentRewardItemRate* itemRate, int round, bool victory)
{
    if (rates == 0)
        return;

    // [b28c-16] P198①：ORIG 先发 `movl $0x0,-0x18`（预清）再按 victory 分支
    // 各自 `mov 0x14(%ebp),%eax; [sub $1]; mov %eax,-0x18` 两段写；
    // 三目合并形 4.4 丢预清行并改装载序。
    int curRound = 0;
    if (victory)
        curRound = round;
    else
        curRound = round - 1;
    if (curRound < 0)
        curRound = 0;
    else if (4 < curRound)
        curRound = 4;

    std::map<int, TournamentRewardRate>::const_iterator it = rates->find(curRound);
    if (it == rates->end())
        return;

    TournamentRewardRate rate = it->second;
    DecisionRewardType(ENUM_TOURNAMENT_REWARD_CARD_TYPE_0, rate, itemRate);
    if (round == 4 && victory)
    {
        std::map<int, TournamentRewardRate>::const_iterator itFinal = rates->find(5);
        if (itFinal == rates->end())
            return;
        {
            TournamentRewardRate rateFinal = itFinal->second;
            DecisionRewardType(ENUM_TOURNAMENT_REWARD_CARD_TYPE_1, rateFinal,
                               itemRate);
        }
    }
