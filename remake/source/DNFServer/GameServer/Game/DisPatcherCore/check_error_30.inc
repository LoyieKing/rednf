unsigned int DisPatcher_AddFriend::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081... 入口 `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)`：view 指针局部在入口声明，
    // 后续 `mov -0xc(%ebp),%eax/%edx; add $0x11` 均从该槽重定位。
    STFriendReqView* req = reinterpret_cast<STFriendReqView*>(&msg);
    if (user->get_state() <= 2)
    {
        return 0x7fffffff;
    }
    if (req->m_name == 0)                                    // ORIG `add $0x11; test %eax,%eax; jne`（死检查保留）
    {
        return 1;
    }
    if (strcmp(req->m_name, user->getCurCharacName()) == 0)
    {
        return 1;
    }
    return 0;
