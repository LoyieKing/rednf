// ORIG 0x08264b5c（报告 Dispatcher_Float_RDATA_Modulation/check_error.md）
// [随行依赖]：ORIG 逐指令即合法恒 0，忠实保留。
int Dispatcher_Float_RDATA_Modulation::check_error(CUser* user, MSG_BASE& msg)
{
    (void)user;
    (void)msg;
    return 0;                            // 0x08264b5f mov $0x0,%eax
}