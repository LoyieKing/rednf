bool CUserCharacInfo::EnablePeriodExpDouble()
{
    std::vector<ContinuousItemInfo>* v = &m_selected->m_continuousItemListB;
    std::vector<ContinuousItemInfo>::iterator it = v->begin();
    unsigned int now = GlobalData::s_systemTime_.getCurSec();
    for (; it != v->end();)
    {
        if (it->m_itemIdx == 0x1cd1)
        {
            unsigned int expire = it->m_expireTime;
            if (now < expire)
                return 1;
            it = v->erase(it);
            SetIsAffectedExpDouble(false);
            break;
        }
        it++;
    }
    return 0;
}