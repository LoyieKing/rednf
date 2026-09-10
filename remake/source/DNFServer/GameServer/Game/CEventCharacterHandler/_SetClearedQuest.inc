void CEventCharacterHandler::_SetClearedQuest(CUser* user)
{
    if (user->get_charac_job() == 0)
    {
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x37f);
    }
    else if (user->get_charac_job() == 1)
    {
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x382);
    }
    else if (user->get_charac_job() == 2)
    {
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x380);
    }
    else if (user->get_charac_job() == 3)
    {
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x383);
    }
    else if (user->get_charac_job() == 4)
    {
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x381);
    }
    else if (user->get_charac_job() == 5)
    {
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x13f9);
    }
    else if (user->get_charac_job() == 6)
    {
        ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
            ->setClearedQuest(0x384);
    }
