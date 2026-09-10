void CMissionList_Charac::addNewMission(const MissionInfo& info)
{
    // ORIG 0x85e4c6c: find_mission 以 MissionInfo.index（+2）判定存在性，slot 以 kind（+0）索引
    // [b28d-64] P0：ORIG 0x085e4c8f 具名指针局部（-0xc 槽往返 + cmpl）⇒ 声明局部；
    // 原码 `if (call())` 4.4 产 setne 物化（+4 条）。
    // [b28d-64] N84/P83：ORIG 0x085e4ca7..b0 为 8 字节结构整体赋值（读 +4 先/+0 后、
    // 写 +0/+4），逐 short 四写会膨胀 20 条 ⇒ 改 `m_missionList[kind] = info;`。
    void* mission = G_CDataManager()->find_mission(info.m_index);
    if (mission != 0)
    {
        m_missionList[info.m_kind] = info;
        if (m_user)
        {
            m_user->m_historyLog.pvpMissionAdd(
                (int)info.m_kind, (int)info.m_index,
                (int)info.m_count, (int)info.m_streak);
        }
    }
