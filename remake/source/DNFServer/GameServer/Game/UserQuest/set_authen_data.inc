void UserQuest::set_authen_data(QUEST_CONDITION cond, int v1, int v2)
{
    switch (cond)
    {
    case QUEST_CONDITION_0:
        m_authenData.m_characNo = (short)v1;
        m_authenData.m_characClass = (short)v2;
        break;
    case QUEST_CONDITION_1:
        m_authenData.m_field4 = (short)v1;
        break;
    case QUEST_CONDITION_2:
        m_authenData.m_field8 = v1;
        break;
    case QUEST_CONDITION_3:
        m_authenData.m_fieldc = (char)(v1 != 0);
        break;
    case QUEST_CONDITION_4:
        m_authenData.m_field10 = v1;
        break;
    case QUEST_CONDITION_5:
        m_authenData.m_field14 = v1;
        break;
    }
