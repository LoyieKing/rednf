bool COnlinePreliminary::CheckMatchable(IMatch* other)
{
    /* ORIG 槽序 selfUser@-0x14 / otherUser@-0x10 / 共享 i@-0xc(@08562434
     * 声明+双循环 entry init 三写同 i 槽)；declaration order = far→near */
    CUser* selfUser = 0;
    CUser* otherUser = 0;
    int i;                     /* ORIG entry 仅 for-init 单写 i(@+0x15) */
    for (i = 0; i <= 3; ++i)
    {
        otherUser = other->GetUser(i);
        if (otherUser != 0)
            break;
    }
    if (otherUser == 0)
        return false;
    for (i = 0; i <= 3; ++i)
    {
        selfUser = GetUser(i);
        if (selfUser != 0)
            break;
    }
    if (selfUser == 0)
        return false;

    // ORIG 0x085624c3 有符号 setge 比较（count >= limit → false）；
    // call 结果免 played 槽直 cmp
    if ((int)_GetEachOtherPlayCount(selfUser, otherUser) >=
        G_CDataManager()->m_serverParameter.GetDailyMatchCount())
        return false;
    return true;
