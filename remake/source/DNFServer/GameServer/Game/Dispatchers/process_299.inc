// ORIG 0x081e85ac（报告 Dispatcher_GrowthCreatureChangeInfinityCreature/process.md）：
// ParamBase 实际为 stGrowthCreature（m_4）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e85ac orig/ours=43/40 ndiff-blocks=10
//   帧差 sub $28 vs $24 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x28,%esp | mov    0x14(%ebp),%eax | mov    %eax,-0xc(%ebp)
int Dispatcher_GrowthCreatureChangeInfinityCreature::process(CUser* user,
                                                             MSG_BASE& msg,
                                                             ParamBase& param)
{
    stGrowthCreature& out = static_cast<stGrowthCreature&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x64e2,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }
    out.m_4 = process_event(user);
    return 0;

}