bool CRewardUserList::delete_mark(unsigned int key)
{
    std::map<unsigned int, RewardUserData>::iterator it = m_map.find(key);
    if (it != m_map.end()) {
        it->second.mark = true;
        return true;
    }
    return false;
