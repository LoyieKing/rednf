bool UserQuest::check_quest_type_condition(Quest* quest)
{
    switch (quest->m_triggerType)
    {
    case 0:
    case 1:
    case 2:
    case 4:
    case 6:
    case 0xf:
    case 0x19:
        return 1;
    case 5:
        if (quest->m_triggerData.at(0) <=
            m_user->getPVPResultR()->m_pvpGrade)
        {
            return 1;
        }
        return 0;
    case 3:
        if (quest->m_triggerData.at(0) == (int)(short)m_authenData.m_field4)
        {
            return 1;
        }
        return 0;
    case 8:
        if (quest->m_triggerData.at(0) <= (int)(short)m_user->GetUserPowerWarPoint())
        {
            return 1;
        }
        return 0;
    case 7:
        if (GlobalData::s_power_manager->GetWinnerSide() ==
            m_user->getPowerSide())
        {
            return 1;
        }
        return 0;
    case 0x10:
    case 0x13:
        return 1;
    default:
        return check_quest_subtype_condition(quest);
    }
