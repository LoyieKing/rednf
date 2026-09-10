CNPCDynamicInfo* CNPCDynamicInfoManager::getNPCInfo(unsigned int idx)
{
    std::map<unsigned int, CNPCDynamicInfo*>::iterator it = m_map.find(idx);
    return (it == m_map.end()) ? 0 : it->second;
