void PvpUserTable::CheckDan()
{
    memset(m_dan, 0, sizeof(m_dan));
    for (int team = 0; team < 8; ++team)
    {
        for (int idx = 0; idx < 8; ++idx)
        {
            CUser* u = m_users[team][idx].user;
            // b28d-54 判词：ORIG 0x085d57c2（CheckDan）。
            // ORIG 形 `je 0x085d583d(nop)` + `je 0x085d583e`（两守卫落点相差 1 字节
            // nop 岛）⇒ 空 then 占位岛，4.4 对 if/else-if 恒 jump-thread 合并两出口。
            // P223：否定 goto + 尾空标签串复现（标签序=守卫序）。
            if (u == 0)
            {
                goto check_dan_then_end;
            }
            if (((CUserCharacInfo*)u)->get_pvp_grade() > 9)
            {
                m_dan[team] = 1;
                break;
            }
        check_dan_then_end:
            ;
        }
    }
}