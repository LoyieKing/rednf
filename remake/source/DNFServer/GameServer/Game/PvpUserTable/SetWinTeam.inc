// b28d-solo-PvpUserTable：ORIG 0x085d585e 两分支各自 `movb $0x1/$0x0` 直写
//   （then 落 0x085d589c、else 落 0x085d58b5，无 al 物化、无 push %ebx、
//   循环槽 -0x8/-0x4）⇒ 三目写形在 4.4 恒物化 `mov $1/jmp/mov $0` 并缓存 i 到
//   %ebx（P171 反向：ORIG 不物化就必须写成两分支独立赋值）。
void PvpUserTable::SetWinTeam(int team)
{
    m_winTeam = team;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (i == team)
                m_users[i][j].flag = 1;
            else
                m_users[i][j].flag = 0;
        }
    }
}