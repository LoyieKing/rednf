void CUserCharacInfo::ClearUselessEffectItem(unsigned int expireTime)
{
    std::vector<ContinuousItemInfo>* v = &CUR->m_effectItemList.m_list;
    for (std::vector<ContinuousItemInfo>::iterator it = v->begin(); it != v->end();)
    {
        if (it->m_expireTime < expireTime)
            it = v->erase(it);
        else
            it++;
    }
