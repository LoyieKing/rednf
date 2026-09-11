void CMissionList_Charac::check_mission_kind(
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
    unsigned int kind, CUser& user,
    const MissionClearCondition_Parameter& param,
    std::vector<int>& outClearList)
{
    bool cleared = false;
    // [F3b-C05③ 2026-09-10 翻案] ORIG 0x085e6295 `je <add esp 尾 +0x154>` 与
    // 0x085e62e7/0x085e63a8 三条守卫**直达同一尾标签** ⇒ 平铺
    // `if (m_index == 0) { return; }` 在 4.4 产一组 nop+jmp 中继岛（+2 条）；
    // 改正向包裹后 4.4 把 fail 路径 jump-thread 直达共享尾出口。旧判词
    // 「N121（109/111）七形实测均不消」作废（当时未试正向包裹本形）。
    if (m_missionList[kind].m_index != 0)
    {
        CMission* mission =
            (CMission*)G_CDataManager()->find_mission(m_missionList[kind].m_index);
        // [b28d-solo-CMissionList_Charac] ORIG 0x085e62bd..0x085e62e7：mission==0 与
        // canUpdate==0 共享 `je <mov $0>` 落点 + `mov $1;jmp;test %al;je <return>` 物化
        // ⇒ P214① 正极性包裹：`if (a!=0 && b!=0) {主流程}` 复现 je P;je P;mov$1;jmp;
        // P:mov$0;Q:test;je 形（负极性早返被 4.4 反演为 xor $1 直跳）。
        if (mission != 0 && mission->canUpdate(param))
        {
            mission->Update(m_missionList[kind], param);
            if (mission->isClearable(m_missionList[kind], cleared))
            {
                int index = (int)m_missionList[kind].m_index;
                outClearList.push_back(index);
                if (kind == 0x1b)
                {
                    m_missionList[7].clear();
                    m_missionList[0x1e].clear();
                }
                ClearMission(user, (short)index);
            }
        }
        if (cleared)
        {
            Send_Fail_Mission(user, m_missionList[kind].m_index);
            cleared = false;
        }
    }
}