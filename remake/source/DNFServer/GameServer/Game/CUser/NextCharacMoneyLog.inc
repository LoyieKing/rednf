void CUser::NextCharacMoneyLog(int now)
{
    if (m_moneyLogs[m_moneyLogIdx].m_money[2] != 0)
    {
        struct tm currentTm;
        struct tm oldTm;
        // b28d-solo-CUser 试形（P0 槽往返反推）：ORIG 0x868f1xx 两个 localtime_r 实参为
        //   `lea 0xc(%ebp)`（**形参 now 的地址**）与 `lea 0xc(%eax)`（**成员 m_money[2]
        //   的地址**）⇒ 原码无 current/old 具名 time_t 局部（我方原形各多一槽 + 装载序差）。
        localtime_r((const time_t*)&now, &currentTm);
        // b28d-solo-CUser 复测：`m_money + 2` 指针算术形与 `&m_money[2]` 取址形
        // 产物逐字相同（4.4 恒 add $0x79678 + lea 0x4；ORIG add $0x79670 + lea 0xc）
        // ⇒ 常量分解差维持 N30/N4 封顶。
        localtime_r(
            (const time_t*)&m_moneyLogs[m_moneyLogIdx].m_money[2], &oldTm);
        if (currentTm.tm_yday != oldTm.tm_yday)
        {
            m_moneyLogIdx = m_moneyLogIdx + 1;
            if (m_moneyLogIdx > 6)
            {
                m_moneyLogIdx = 6;
            }
            else
            {
                return;   // b28d-solo-CUser 试形（P285）：ORIG 0x868f2xx 赋值后保留
                          // `jmp <leave>` 体尾岛；4.4 对纯正向 if 恒 jump-thread。
            }
        }
    }
