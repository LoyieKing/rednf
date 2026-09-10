STGuildStorageUpgradeInfo* GuildParameterScript::getGuildCargoUpradeInfo(int capacity)
{
    for (std::vector<STGuildStorageUpgradeInfo>::iterator it = m_vec38.begin();
         it != m_vec38.end(); ++it)
    {
        STGuildStorageUpgradeInfo* v = &*it;   // ORIG：de 结果存局部
        if (v->m_field0 == capacity)
        {
            return v;
        }
    }
    return 0;
