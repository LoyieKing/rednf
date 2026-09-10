bool CItem::isExpertJobUsable(ENUM_EXPERT_JOB_TYPE type, int level) const
{
    if (m_vec104.empty())
    {
        return true;
    }
    for (std::vector<STItemScript::SEXPERTJOB::SUSABLE>::const_iterator it =
             m_vec104.begin();
         it != m_vec104.end(); it++)
    {
        if (it->m_field0 == type)
        {
            if (it->m_field4 <= level)
            {
                return true;
            }
            return false;
        }
        if (it->m_field0 == ENUM_EXPERT_JOB_TYPE_5 &&
            type != ENUM_EXPERT_JOB_TYPE_0)
        {
            if (it->m_field4 <= level)
            {
                return true;
            }
            return false;
        }
    }
    return false;
