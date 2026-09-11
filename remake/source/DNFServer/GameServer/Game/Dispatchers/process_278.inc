// ORIG 0x081ce356（报告 Dispatcher_CallPartyMemberRealtimeInfo/process.md）：
// 不写 param+4；仅按 check_error 结果归一返回。
int Dispatcher_CallPartyMemberRealtimeInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    int err = check_error(user, msg);
    // ORIG 0x081ce356 分支布局：err>0 → 真分支 fall-through 返回 err；
    // 假侧跳转重测 err<0 → -1；否则 0。
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    return 0;
}