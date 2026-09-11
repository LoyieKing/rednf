// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08264740 orig/ours=57/59 ndiff-blocks=14
//   帧差 sub $34 vs $48 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x34,%esp | mov    %eax,-0x14(%ebp)
int Dispatcher_ScanBotByDrv::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x08264740
    int err = check_error(user, msg);
    if (err != 0)
    {
        return 0;
    }
    MSG_SCAN_DRV& req = reinterpret_cast<MSG_SCAN_DRV&>(msg);
    // ORIG 0x0826477c `mov 0xd(%eax),%eax; add $0x708,%eax; mov %eax,-0xc(%ebp)`
    // ⇒ 一次计算入具名 int 槽（禁每次使用重算 +0x708）。
    const int hackType = req.m_field0d + 0x708;
    user->getHackAnalyzer()->setBaseHackType((WongWork::ENUM_HACKTYPE)0x708);
    // ORIG 0x082647c3 `mov %eax,%ebx`（SetETC 结果免槽）+ 单语句链式 receiver
    // （第二次 getHackAnalyzer 在 SetETC 之后求值）⇒ 内联实参。
    user->getHackAnalyzer()->addServerHackCnt(
        user, (WongWork::ENUM_HACKTYPE)hackType, 1,
        (unsigned int)user->SetETC((short)hackType, (int)(char)req.m_field11), 0);
    return 0;
}