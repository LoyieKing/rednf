int PvpUserTable::GetGhostVsTeamPoint(int team, int)
{
    // ORIG 0x085d4fd1/0x085d4fd8 两条 `movl $0,-0x10/-0xc` 初始化 ⇒ 源码 u 先
    // 声明（槽 -0x10，初始化为 0）再 for-init i（槽 -0xc）；把 u 放循环体内
    // 会让 i 占 -0x10、u 占 -0xc（槽序反，P28/N93 标量可翻）。
    CUser* u = 0;
    for (int i = 0; i < 8; ++i)
    {
        u = m_users[team][i].user;
        // ORIG 0x085d5047 体尾 `jmp +0x80; nop` 空 then 占位岛 ⇒ P223 goto 尾
        // 空标签形（4.4 对 if(cond){body} 恒 jump-thread 掉 jmp+nop）。
        if (u == 0)
            goto next;
        m_teamPoint[team] += GetExpectVal(((CUserCharacInfo*)u)->get_pvp_grade(),
                                          0, 0);
    next:;
    }
    return 1;
}