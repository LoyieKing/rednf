// ---------------------------------------------------------------------------
// CBattle_Field::MakeNextRoundInfo @ 0x0830410c
// 报告要点：函数顶部构造局部 nextMap（0x0831116e）；循环 i=0 起、步进 +2，
// 条件每轮重算 0x10/memberCount-1（0x08304528 idivl 有符号除法，memberCount
// 为 0 时 ORIG 直接除零 —— 无防护，忠实保留）。每对 a=i/b=i+1：先取
// roundMap[(short)a].m_group(+0x14)、roundMap[(short)b].m_group；若
// roundMap[(short)a].m_monster[0].m_mobId(+0x04)==0 则 a 晋级，否则判 b；
// 都不空时 rndA=0;rndB=0 初始化后 get_rand_int(groupA)/get_rand_int(groupB)，
// rndA>rndB 时 a 晋级否则 b。晋级写 roundMap[key].m_roundOrder=(short)(key/2)
// 后以三次 operator[] 形态 insert 进 nextMap：
// insert(make_pair(roundMap[key].m_roundOrder, roundMap[key]))（返回值丢弃）。
// 循环外 roundMap = nextMap（copy-assign 0x083115be）→ ~nextMap。
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
}