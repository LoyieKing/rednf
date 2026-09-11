// ORIG 0x082649ae（报告 Dispatcher_AbnormalUseStackable/process.md）：
// check_error==0 → addServerHackCnt(user, 0xfa0, 1, msg+0x10, msg+0x14)。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082649ae orig/ours=41/35 ndiff-blocks=6
//   帧差 sub $30 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x30,%esp
int Dispatcher_AbnormalUseStackable::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    int err = check_error(user, msg);
    if (err != 0)
    {
        return 0;
    }
    // ORIG +0x31：err!=0 早退后装载 msg 引用槽 -0xc（P-L11c）
    MSG_ABNORMAL_USE_STACKABLE& req = reinterpret_cast<MSG_ABNORMAL_USE_STACKABLE&>(msg);
    // ORIG +0x37：arg5/arg4 先算入 esi/ebx 跨 receiver call 保活（无 analyzer 具名槽）
    user->getHackAnalyzer()->addServerHackCnt(
        user, (WongWork::ENUM_HACK_TYPE)0xfa0, 1,
        (unsigned int)req.m_field10, (unsigned int)req.m_field14);
    return 0;
}