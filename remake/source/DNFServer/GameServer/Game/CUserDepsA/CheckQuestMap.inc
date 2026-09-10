int CUser::CheckQuestMap(int dungeonIdx)
{
    if (getCurCharacR() == 0)
        return 0;
    return getCurCharacQuestR()->get_rescue_questmap(dungeonIdx);
