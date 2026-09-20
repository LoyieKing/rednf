void CUserCharacInfo::ClearUselessCooltimeItem(unsigned int expireTime)
{
    std::vector<ContinuousItemInfo>* v = &m_selected->m_continuousItemListA;
    for (std::vector<ContinuousItemInfo>::iterator it = v->begin(); it != v->end();)
    {
        if (it->m_expireTime < expireTime)
            it = v->erase(it);
        else
            it++;
    }
}