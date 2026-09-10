bool CRewardUserList::is_exist(unsigned int key)
{
    std::map<unsigned int, RewardUserData>::const_iterator it =
        m_map.find(key);
    return it == m_map.end() ? false : true;
