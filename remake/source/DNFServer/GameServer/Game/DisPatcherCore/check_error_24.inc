unsigned int DisPatcher_GetItem::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081c35ac：msg 存入局部（-0x4）未使用，恒返 0。
    MSG_BASE& req = msg;   // ORIG -0x4(%ebp)
    (void)req;
    return 0;
