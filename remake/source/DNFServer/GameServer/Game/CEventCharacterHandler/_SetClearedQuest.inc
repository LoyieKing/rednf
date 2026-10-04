void CEventCharacterHandler::_SetClearedQuest(CUser* user)
{
    if (user->get_charac_job() == 0)
    {
        user->getCurCharacQuestW()->m_cleared.setClearedQuest(0x37f);
    }
    else if (user->get_charac_job() == 1)
    {
        user->getCurCharacQuestW()->m_cleared.setClearedQuest(0x382);
    }
    else if (user->get_charac_job() == 2)
    {
        user->getCurCharacQuestW()->m_cleared.setClearedQuest(0x380);
    }
    else if (user->get_charac_job() == 3)
    {
        user->getCurCharacQuestW()->m_cleared.setClearedQuest(0x383);
    }
    else if (user->get_charac_job() == 4)
    {
        user->getCurCharacQuestW()->m_cleared.setClearedQuest(0x381);
    }
    else if (user->get_charac_job() == 5)
    {
        user->getCurCharacQuestW()->m_cleared.setClearedQuest(0x13f9);
    }
    else if (user->get_charac_job() == 6)
    {
        user->getCurCharacQuestW()->m_cleared.setClearedQuest(0x384);
    }
}