void CDeathTower::CPlayData::makeStartMemberInfo(CParty* party)
{
    // [b28d-solo-CDeathTower] verdict=AE（orig 90 / our 90，P167 CLEAR 9/9）：修法 =
    // P214① 短路守卫（P226 语句表达式，消 -0x9 bool 槽）、P197③ info 预清、P30/P311
    // 二维数组视图（m_startMemberInfo[4][0x17]，复现 strcpy 目标 add 链）+ 字段写入
    // 常量拆分按 ORIG 的 0x9d0+0x10 形对齐（N1/N94 已消）。
    // ORIG 0x08461d02：memset(m_startMemberInfo,0,0x5c)；遍历 4 个队伍槽，
    // 有效成员（get_user 非空且 checkValidUser==1）把名字 strcpy 到
    // m_startMemberInfo+count*0x17，level/job/growType 写到条目 +0x14/+0x15/+0x16；
    // 结束时 m_startMemberCnt = get_member_count()。
    memset(m_startMemberInfo, 0, sizeof(m_startMemberInfo));
    int count = 0;
    for (int i = 0; i < 4; ++i)
    {
        // [b28d-solo-CDeathTower] P197③/P198①：ORIG 0x08461d3c 循环体首
        // `movl $0,-0xc` 预清 ⇒ 声明=0 与赋值分两句式。
        CUserCharacInfo* info = 0;
        info = (CUserCharacInfo*)party->get_user(i);
        // [b28d-solo-CDeathTower] P226 条件向：ORIG 0x08461d58..0x08461d85 为
        // 零槽短路物化岛（info==0 与 `xor $1; test` 共享 mov$1 岛，尾 test/jne
        // 跳 SKIP）；具名 bool 局部多 -0x9 槽并令 i/count/info 整体深移 4。
        if (({ info == 0 || !party->checkValidUser(i); }))
            continue;
        {
            strcpy(m_startMemberInfo[count].m_name,
                   info->getCurCharacName());
            m_startMemberInfo[count].m_level =
                (char)info->get_charac_level();
            m_startMemberInfo[count].m_job =
                (char)info->get_charac_job();
            m_startMemberInfo[count].m_growType =
                (char)info->getCurCharacGrowType();
            ++count;
        }
    }
    m_startMemberCnt = party->get_member_count();
}