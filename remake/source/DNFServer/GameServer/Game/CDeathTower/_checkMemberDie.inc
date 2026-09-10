bool CDeathTower::_checkMemberDie()
{
    // ORIG 0x8467188：循环变量 unsigned（0x84671ea setbe 实证）；双次置 0
    // （0x846718e/0x8467195）；&& 物化岛（范式40）：valid&&alive 共享 mov$1/$0
    // （0x84671d2/0x84671d9）；break 后尾置 i==4 两段物化（0x84671f4..0x8467201）。
    unsigned int i = 0;
    i = 0;
    for (; i <= 3; ++i)
    {
        // [b28d-D13] P0 清绑（N96）：ORIG 0x84671d2 mov$1,%eax 无栈槽 ⇒ 表达式直用。
        if (m_party->checkValidUser(i) && m_playData.isMemberAlive(i))
        {
            break;
        }
    }
    if (i == 4)
    {
        return 1;
    }
    return 0;
