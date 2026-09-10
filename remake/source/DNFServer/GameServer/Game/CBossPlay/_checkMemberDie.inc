bool CBossPlay::_checkMemberDie()
{
    // ORIG 0x08149396 双 init（P191 配套）；条件链 test/je 直落 mov$1/mov$0
    // 岛（无 xor 物化 → 正向 && 短路）；尾块 i==4 双岛 return（P56）。
    unsigned int i = 0;
    for (i = 0; i < 4; ++i)
    {
        if (m_party->checkValidUser(i) && isMemberAlive(i))
            break;
    }
    if (i == 4)
        return true;
    return false;
