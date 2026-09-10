stFavorableRelationShip_t::stLevelRewardInfo CNPCScript::getLevelRewardInfo(
    unsigned short level) const
{
    stFavorableRelationShip_t::stLevelRewardInfo result;
    std::vector<stFavorableRelationShip_t::stLevelRewardInfo>::const_iterator it =
        m_npcScript.m_favorableRelationShip.m_levelRewards.begin();
    for (; it != m_npcScript.m_favorableRelationShip.m_levelRewards.end(); it++)
    {
        const stFavorableRelationShip_t::stLevelRewardInfo* p = &*it;
        if (p != 0)
        {
            if (p->m_level == level)
            {
                result.m_level = p->m_level;
                result.m_flag0 = p->m_flag0;
                result.m_flag1 = p->m_flag1;
                result.m_name = p->m_name;
                result.m_desc = p->m_desc;
                break;
            }
        }
    }
    return result;
