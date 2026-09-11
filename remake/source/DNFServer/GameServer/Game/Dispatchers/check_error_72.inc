// ORIG 0x081de2e6（nm 实测，sz 0x43）：state>3 且 getCurCharacR()!=0 → 0，否则 -1。
int Dispatcher_Direct_Entrance_Quick_Party::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    if (user->get_state() <= 3 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    return 0;
}