void CBattle_Field::MakeNextRoundInfo(int memberCount,
    std::map<short, TournamentInfo>& roundMap)
{
    std::map<short, TournamentInfo> nextMap;
    for (int i = 0; i < 0x10 / memberCount - 1; i += 2)
    {
        int a = i;
        int b = i + 1;
        int groupA = roundMap[(short)a].m_group;
        int groupB = roundMap[(short)b].m_group;

        if (roundMap[(short)a].m_monster[0].m_mobId == 0)
        {
            roundMap[(short)a].m_roundOrder = (short)(a / 2);
            nextMap.insert(std::make_pair(
                roundMap[(short)a].m_roundOrder, roundMap[(short)a]));
        }
        else if (roundMap[(short)b].m_monster[0].m_mobId == 0)
        {
            roundMap[(short)b].m_roundOrder = (short)(b / 2);
            nextMap.insert(std::make_pair(
                roundMap[(short)b].m_roundOrder, roundMap[(short)b]));
        }
        else
        {
            int rndA = 0;
            int rndB = 0;
            rndA = get_rand_int(groupA);
            rndB = get_rand_int(groupB);
            if (rndA > rndB)
            {
                roundMap[(short)a].m_roundOrder = (short)(a / 2);
                nextMap.insert(std::make_pair(
                    roundMap[(short)a].m_roundOrder, roundMap[(short)a]));
            }
            else
            {
                roundMap[(short)b].m_roundOrder = (short)(b / 2);
                nextMap.insert(std::make_pair(
                    roundMap[(short)b].m_roundOrder, roundMap[(short)b]));
            }
        }
    }
    roundMap = nextMap;
