    // [b28 判词 N96+P168a④] X2 寄存器 xor/钉值物化形需 register 绑定；绑定 P0 全域禁用后无源形，N 封顶。
CUser* COnlinePreliminary::GetUser(unsigned int idx)
{
    if (idx >= 4)
        return 0;
    /* ORIG @08560e6：无独立 bool 槽（无 sub 帧），field==1 判据。4.4 对
     * `if(m_flag==1)` 产 `cmp $1,%al; jne`，ORIG 为 `xor $1,%eax; test %al,%al; je`——
     * [b28b 判词 N96+P168a④] xor $1 物化（非 cmp）需 register 绑定，P0 禁用后
     * 4 形裸测（==1 / !=1 / ^1!=0 / (uchar)v^=1）恒折 cmp；帧/槽已对齐（无 sub），仅余 3 条。 */
    if ((m_teams[0].m_seats[idx].m_flag ^ 1) != 0)
        return 0;
    return m_teams[0].m_seats[idx].m_user;
}