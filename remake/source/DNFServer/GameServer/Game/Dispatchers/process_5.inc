int Dispatcher_SummonUnit::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Fix6：ORIG 0x8142356 空壳（mov $0x0,%eax; ret），不转调 process_packet。
    (void)user; (void)msg; (void)param;
    return 0;
