int Dispatcher_MouseRegister::check_error(CUser* user, MSG_BASE& msg)
{
    MSG_BASE* pm = &msg;   // ORIG 0x082632a5：msg→-0x4 死槽（技巧8），恒返 0
    (void)user;
    (void)pm;
    return 0;
}