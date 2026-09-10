unsigned int DisPatcher_RemoveFriend::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->get_state() <= 2)
    {
        return 0x7fffffff;
    }
    // ORIG 0x081cb5d8：msg 指针先落 -0xc 再 reload（帧 0x28；m_name 数组地址判 0，恒 false）。
    STFriendReqView* pMsg = reinterpret_cast<STFriendReqView*>(&msg);   // ORIG -0xc(%ebp)
    if ((*(&pMsg))->m_name == 0)   // +0x11
    {
        return 1;
    }
    return 0;
