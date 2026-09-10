bool COnlinePreliminary::_CheckObserver(PvP_Room* room, CUser* /*user*/)
{
    int observers = 0;
    for (int i = 0; i <= 7; ++i)
    {
        if (room->get_team(i) == 3)
            ++observers;
    }
    /* ORIG(@0856078a 起 size→%ebx / sub mem,%ebx / cmp %eax,%ebx; setge 物化)
     * 属 N22/N18a 类：4.4 恒将跨 call 差值 spill 或经 %eax 往返，probe(
     * register asm("ebx")+setge 技巧22/操作数对调) 均不可翻，保留直译 */
    /* ORIG @08560797 cmp %eax,%ebx; setge %al; test; je → mov $1 岛(@+0x71)、
     * ge 真走 fall mov $0(@+0x6a)：register bool 物化+正向 if（同
     * _CheckMatchable 尾法） */
    if (m_teams[0].size() - observers >= (int)(unsigned char)GetPlayCountPerOneTeam())
        return 0;
    return 1;
