// ---------------------------------------------------------------------------
// 37) CUser::CheckDungeonClear(int questId)
//     报告：docs/class_func_reports/CUser/CheckDungeonClear.md。ORIG 0x0866d934。
// ---------------------------------------------------------------------------
// CUser.h:3620 已改 int（Main 落，ORIG 0x0866d96a 尾 call item_reward_quest; leave 直返 int）
int CUser::CheckDungeonClear(int questId)
{
    if (getCurCharacR() == 0)
        return false;   // = 0；bool→int 透传（CUser.h:1265 item_reward_quest 返 int）
    return getCurCharacQuestR()->item_reward_quest(questId);
}