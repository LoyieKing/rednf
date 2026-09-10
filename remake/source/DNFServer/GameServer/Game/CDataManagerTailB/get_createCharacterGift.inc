void CDataManager::get_createCharacterGift(
    int idx, createChracScript::rewardInfoList& out) const
{
    std::map<int, createChracScript::rewardInfoList>::const_iterator it =
        m_createChrac.m_map0.find(idx);
    if (it != m_createChrac.m_map0.end())
    {
        out.m_list.insert(out.m_list.end(),
                          it->second.m_list.begin(),
                          it->second.m_list.end());
    }

    it = m_createChrac.m_map0.find(0xb);
    if (it != m_createChrac.m_map0.end())
    {
        out.m_list.insert(out.m_list.end(),
                          it->second.m_list.begin(),
                          it->second.m_list.end());
    }
