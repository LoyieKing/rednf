int Dispatcher_SecurityCardAuthCancel::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x0820a580 W：push ebp; mov esp,ebp; mov $0x0,%eax; pop ebp; ret —— 空体。
    // 实际业务逻辑在 static process(CUser*)（DispLinkA3.cpp，ORIG 同址重载）。
    (void)user; (void)msg; (void)param;
    return 0;
