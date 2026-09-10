void CMissionList_Charac::Reset_DailyMission()
{
    // [b28d-58] DIFF N104（EH 尾块相对序）：槽位集与 ORIG 全等——daily -0x38、
    //   bitset::reference -0x34、newList -0x44、trace -0x2c（P221 全表达式临时链
    //   `cMyTrace(...)(...)` 翻正 trace 槽方向，具名对象被 4.4 放最深 -0x44）。
    //   残差仅 ORIG 正常 epilogue 在前 / unwind 块在函数最末，4.4 相反（difflib
    //   rotate 放大 ~30 行），gap=1，语义等价。
    // [b28d-solo-CMissionList_Charac] 判词 N104（132/133）：调用点差集归零、槽位集
    //   全等，唯 EH 正常尾块/unwind 尾块相对序互换 ⇒ 封顶。本轮复验：ORIG
    //   0x085e56b9 alter → 0x085e56be ~vector 就地 → 0x085e56c9 epilogue →
    //   0x085e56d3 unwind 最末；4.4 为 alter → jmp → unwind → ~vector → epilogue
    //   （指令集逐条全等，仅块序 rotate，p167 22/22 CLEAR）。
    std::bitset<32> daily = G_CDataManager()->get_daily_mission_kind();
    for (int i = 0; i < 0x20; ++i)
    {
        if (daily[i])
        {
            m_missionList[i].clear();
            m_missionList[i].m_kind = (short)i;
        }
    }

    std::vector<std::pair<unsigned int, unsigned int> > newList;
    G_CDataManager()->get_New_DailyMission(newList);
    if (newList.empty())
    {
        // ORIG 0x85e5607：cMyTrace 5/0x669 记 "[@missionSystem] Daily Mission List is Empty!!"
        // [b28d-58] 试 P221 全表达式临时链：具名 trace 被 4.4 放最深槽 -0x44，
        // ORIG 为 -0x2c（块内对象槽方向相反）。
        cMyTrace(__PRETTY_FUNCTION__, 0x669, 5)(
            "[@missionSystem] Daily Mission List is Empty!!");
    }
    for (unsigned int i = 0; i < newList.size(); ++i)
    {
        unsigned int kind = newList[i].first;
        if (daily[kind])
        {
            MakeMissionList_JustKind(*m_user, kind);
        }
    }
    alter();
