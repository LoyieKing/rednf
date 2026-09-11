// ORIG 0x85811f8：m_buffRewards 权重随机（get_rand_int(1000)），条件
//   it->m_a < level；命中后拷贝 m_b/m_a/m_c 并 break。
stFavorableRelationShip_t::stBuffRewardInfo CNPCScript::getBuffRewardInfo(int level) const
{
    std::vector<stFavorableRelationShip_t::stBuffRewardInfo>::const_iterator it =
        m_npcScript.m_favorableRelationShip.m_buffRewards.begin();
    std::vector<stFavorableRelationShip_t::stBuffRewardInfo>::const_iterator end =
        m_npcScript.m_favorableRelationShip.m_buffRewards.end();
    stFavorableRelationShip_t::stBuffRewardInfo info;
    int r = get_rand_int(1000);
    int total = 0;
    info.m_b = 0;
    for (; it != end; it++)
    {
        if (it->m_a < level)
        {
            total += it->m_c;
            if (total > r)
            {
                info.m_b = it->m_b;
                info.m_a = it->m_a;
                info.m_c = it->m_c;
                break;
            }
        }
    }
    return info;
}