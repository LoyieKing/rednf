void CMissionList_Charac::ClearMission(CUser& user, short index)
{
    CMission* mission =
        (CMission*)G_CDataManager()->find_mission((int)index);
    if (!mission)
    {
        // ORIG 0x85e6441：mission 缺失记日志（cMyTrace 5/0x797）后返回。
        // [b28d-solo-CMissionList_Charac] P227：ORIG 0x085e646c 先把 index 载入
        // %ebx 跨 ctor 保活再 call op() ⇒ 全表达式临时链形（具名对象会即时读取）。
        // 判词 N7/N82（121/123）：残差=cMyTrace 变参装载寄存器角色（N7）与
        // sete/setne 极性（setcc-fold），语义等价。
        cMyTrace(__PRETTY_FUNCTION__, 0x797, 5)(
            "[@missionSystem] InvalidIndex: <index : %d>", (int)index);
        return;
    }
    int kind = mission->GetMissionKind();
    if (kind == 6)
    {
        m_field14e = 1;
    }
    // [b28d-solo-CMissionList_Charac] P59/P222 块序：ORIG 0x085e64b9 `je +0xf4`
    // 跳主流程、**日志块 fall-through 在前**；4.4 对正极性 if(==) 产反布局
    // ⇒ 改否定条件 `if (m_index != index) {日志} else {主流程}`。
    if (m_missionList[kind].m_index != index)
    {
        // ORIG 0x85e64e1：m_index 不匹配记日志（cMyTrace 5/0x7a5）后返回。
        cMyTrace(__PRETTY_FUNCTION__, 0x7a5, 5)(
            "[@missionSystem] NotMatchIndex: <valid index:%d invalid index:%d>",
            (int)m_missionList[kind].m_index, (int)index);
    }
    else
    {
        alter();
        m_clearedFlags[(int)index] = true;
        m_missionList[kind].clear();
        mission->Reward(user);
        // [b28d-solo-CMissionList_Charac] ORIG 0x085e65b1 `cmp $2; sete %al;
        // test %al,%al; jne <nop 岛>; call; jmp <epilogue>; nop`（==2 跳过）
        // ⇒ P223 `if(c) goto L; body; L:;` 形。
        if (mission->GetMissionGrade() == 2)
            goto L_grade2;
        MakeMissionList_forOldUser(user);
    L_grade2:;
    }
