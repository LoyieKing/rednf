int* CDataManager::find_hellparty_group(int idx)
{
    const unsigned short key = (unsigned short)idx;
    std::map<unsigned short, stHellPartyMonsterGroupList>::iterator it =
        reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t())
            ->m_groupMap.find(key);
    if (it != reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t())
                  ->m_groupMap.end())
        return reinterpret_cast<int*>(&it->second);
    return NULL;
