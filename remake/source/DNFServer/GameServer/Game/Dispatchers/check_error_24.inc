// ORIG 0x081d6746（报告 Dispatcher_Clear_Used_QP/check_error.md）[随行依赖，
// process 语义必需；不在 55 桩清单]
int Dispatcher_Clear_Used_QP::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user == 0)
    {
        return -1;                       // 0x081d6752
    }
    if (user->get_state() != 3)
    {
        return 99;                       // 0x081d676e
    }
    return 0;
}