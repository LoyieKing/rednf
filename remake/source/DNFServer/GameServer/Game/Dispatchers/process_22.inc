int Dispatcher_GM_Command::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)user; (void)msg; (void)param;
    return 0;  // ORIG 类未声明 process（vtable 纯虚由基类骨架覆盖），恒 0
