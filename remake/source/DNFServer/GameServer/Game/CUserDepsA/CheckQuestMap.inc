// ----------------------------------------------------------------------------
// 2. ORIG T 0x0866cac6 sz 0x3d。
// getCurCharacR() 为空返回 0；否则委托当前角色 UserQuest 的
// get_rescue_questmap(dungeonIdx)（ORIG 直调 _ZNK9UserQuest19get_rescue_questmapEi）。
// ----------------------------------------------------------------------------
int CUser::CheckQuestMap(int dungeonIdx)
{
    if (getCurCharacR() == 0)
        return 0;
    return getCurCharacQuestR()->get_rescue_questmap(dungeonIdx);
}