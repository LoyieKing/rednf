bool UserQuest::check_quest_subtype_condition(Quest* quest)
{
    switch (quest->m_triggerSubType)
    {
    case 0:
    {
        int at0 = quest->m_triggerData.at(0);
        int at1 = quest->m_triggerData.at(1);
        int at2 = quest->m_triggerData.at(2);
        if (at1 != -1 && (int)m_authenData.m_characClass != at1)
        {
            return 0;
        }
        if ((int)m_authenData.m_characNo != at0)
        {
            return 0;
        }
        if (at2 * 1000 < m_authenData.m_field8)
        {
            return 0;
        }
        return 1;
    }
    case 1:
    case 2:
    case 3:
    case 6:
        return 1;
    case 4:
    {
        int at0 = quest->m_triggerData.at(0);
        int at1 = quest->m_triggerData.at(1);
        if (at1 != -1 && (int)m_authenData.m_characClass != at1)
        {
            return 0;
        }
        if ((int)m_authenData.m_characNo != at0)
        {
            return 0;
        }
        if (m_authenData.m_fieldc != 0)
        {
            return 0;
        }
        return 1;
    }
    case 5:
    {
        int at0 = quest->m_triggerData.at(0);
        int at1 = quest->m_triggerData.at(1);
        int at2 = quest->m_triggerData.at(2);
        if (at1 != -1 && (int)m_authenData.m_characClass < at1)
        {
            return 0;
        }
        if ((int)m_authenData.m_characNo != at0)
        {
            return 0;
        }
        if (at2 == 1 && m_authenData.m_field10 != 1)
        {
            return 0;
        }
        if (m_authenData.m_field10 < at2)
        {
            return 0;
        }
        return 1;
    }
    case 7:
    {
        int at0 = quest->m_triggerData.at(0);
        int at1 = quest->m_triggerData.at(1);
        int at2 = quest->m_triggerData.at(2);
        if (at1 != -1 && (int)m_authenData.m_characClass != at1)
        {
            return 0;
        }
        if ((int)m_authenData.m_characNo != at0)
        {
            return 0;
        }
        if (m_authenData.m_field14 < at2)
        {
            return 0;
        }
    }
    default:
        return 0;
    }
