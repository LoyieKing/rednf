bool CParty::is_limit_minimum_level(int level, int& param)
{
    for (int i = 0; i <= 3; i++)
    {
        // [判据②岛形+拆&&] ORIG O@22-26：`call;cmp;setl%al;test;je`。
        if (_checkValidUser(i))
        {
            // [b28c-L7] P0 清绑：原 `register bool low asm("al")` 与「-O0 局部
            // 恒占栈槽、寄存器仅承载表达式值」相悖（N96 登记口径）；改比较直用
            // 形复测，残差按 P168a④/N18a 判词。
            if (m_member[i].m_pUser->get_charac_level() < level)
            {
                param = m_member[i].m_pUser->get_unique_id() & 0xffff;
                return false;
            }
        }
    }
    return true;
