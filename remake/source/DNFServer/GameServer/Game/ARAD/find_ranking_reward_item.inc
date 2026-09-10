bool Arad_VillageAttackRewardItem::find_ranking_reward_item(short rank, RankData& out)
{
    // ORIG 0x81a3ef0：cmpw rank + js（rank<0）+ cmp 16 位 size + jle（rank>=size）
    // —— 有符号 16 位比较；合并 || 以匹配 js/jle 前置 false 布局。
    if (rank >= 0 && rank < (short)m_rankList.size())
    {
        out = m_rankList[rank];
        return true;
    }
    return false;
