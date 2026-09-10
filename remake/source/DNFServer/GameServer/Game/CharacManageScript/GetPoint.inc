int CharacManageScript::GetPoint(int level)
{
    int total = 0;
    for (std::vector<ManagePointBonus>::iterator it = m_pointBonus.begin();
         it != m_pointBonus.end(); it++)
    {
        ManagePointBonus& bonus = *it;
        for (int lv = it->m_field00; lv <= it->m_field04; ++lv)
        {
            if (level >= lv)
                total += it->m_field08;
        }
    }
    return total;
