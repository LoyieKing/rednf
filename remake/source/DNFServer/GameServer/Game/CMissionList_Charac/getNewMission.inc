int CMissionList_Charac::getNewMission(CUser& user, int kind)
{
    CMission* mission =
        (CMission*)G_CDataManager()->find_mission(kind);
    if (mission)
    {
        // ORIG kind 落槽 -0xc（P0 具名局部；槽序 mission=-0x10 先、kind=-0xc 后）。
        int missionKind = mission->GetMissionKind();
        ClearMissionKind(user, (short)missionKind);
    }
    // ORIG 尾无 mov $0x0（call ClearMissionKind 后直接 leave;ret）：源码无 return，
    // 返回值自然沿 ClearMissionKind 的 eax 掉出（4.1 实证）。
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}