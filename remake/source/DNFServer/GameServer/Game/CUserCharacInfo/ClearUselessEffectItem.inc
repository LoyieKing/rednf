void CUserCharacInfo::ClearUselessEffectItem(unsigned int expireTime)
{
    std::vector<ContinuousItemInfo>* v = &m_selected->m_continuousItemListB;
    for (std::vector<ContinuousItemInfo>::iterator it = v->begin(); it != v->end();)
    {
        if (it->m_expireTime < expireTime)
            it = v->erase(it);
        else
            it++;
    }
}