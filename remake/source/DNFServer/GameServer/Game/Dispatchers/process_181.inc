// ORIG 0x082642b8（报告 Dispatcher_ValidateScriptHash/process.md）：
// check_error 恒 0；get_hack_type(msg+0xd) → addServerHackCnt(type,1,0xe,0xf)。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082642b8 orig/ours=51/54 ndiff-blocks=11
//   帧差 sub $30 vs $3c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    %eax,-0x14(%ebp) | cmpl   $0x0,-0x14(%ebp)
int Dispatcher_ValidateScriptHash::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    // ORIG +0x21：err 具名槽 -0x14（mov %eax,-0x14; cmpl $0,-0x14; je 继续）
    const int err = check_error(user, msg);
    if (err != 0)
    {
        return 0;
    }
    MSG_SCRIPT_HASH& req = reinterpret_cast<MSG_SCRIPT_HASH&>(msg);
    const int hackType = get_hack_type((char)req.m_field0d);
    user->getHackAnalyzer()->addServerHackCnt(
        user, (WongWork::ENUM_HACKTYPE)hackType, 1u,
        (unsigned int)req.m_field0e, (unsigned int)req.m_field0f);
    return 0;
}