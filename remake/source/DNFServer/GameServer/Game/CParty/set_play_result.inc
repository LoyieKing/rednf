int CParty::set_play_result(CUser* user, GameResultSet& result)
{
    int ret = 0;                                        // -0x10
    // ORIG 0x085b2068：两段 guard 早退（各 `mov -0x10,%eax; jmp epilogue`），
    // 非嵌套 if+单一尾 return——写 guard-return 形复现双读回块。
    if (m_host != user)
        return ret;
    if (m_recvResultFlag)                               // +0x118
        return ret;
    for (int i = 0; i <= 3; ++i)                        // -0xc
    {
        if (!_checkValidUser(i))
            continue;
        m_member[i].m_pUser->setPlayingDungeonIndex(-1);
    }
    m_state = 3;                                        // +0x6a（ORIG movb 直写）
    m_recvResultFlag = true;                            // +0x118（ORIG movb 直写）
    return ret;
