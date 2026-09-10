int CUser::CheckDungeonClear(int questId)
{
    if (getCurCharacR() == 0)
        return false;   // = 0；bool→int 透传（CUser.h:1265 item_reward_quest 返 int）
    return getCurCharacQuestR()->item_reward_quest(questId);
