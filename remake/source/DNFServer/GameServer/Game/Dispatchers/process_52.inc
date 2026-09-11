// ORIG 0x08264488（报告 Dispatcher_ScanBotByDll/process.md）：
// check_error==0 后 SetETC(type+0x352, (char)msg+0x11)，再 addServerHackCnt。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08264488 orig/ours=56/59 ndiff-blocks=12
//   帧差 sub $30 vs $3c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    %eax,-0x14(%ebp) | cmpl   $0x0,-0x14(%ebp)
int Dispatcher_ScanBotByDll::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    // [Lane1Dispatchers 2026-09-10] P0 槽往返：ORIG 0x0821e? 为 `call check_error;
    //   mov %eax,-0x14(%ebp); cmpl $0x0,-0x14(%ebp); je`（具名 int 局部 + 内存比较）；
    //   直接写 `if (check_error(...) != 0)` 在 4.4 产 `test %eax,%eax; setne %al;
    //   test %al,%al; je`（+2 条且多 push %edi/帧 +0xc）。与同族 ScanBotByDrv 对齐。
    int err = check_error(user, msg);
    if (err != 0)
    {
        return 0;
    }
    MSG_SCAN_DLL& req = reinterpret_cast<MSG_SCAN_DLL&>(msg);
    const int hackType = req.m_field0d + 0x352;      // ORIG 0x082644c5 一次 add → -0xc 槽
    // ORIG 0x082644f8 `mov %eax,%ebx`（SetETC 结果免槽）+ 0x082644d0 `movzwl 0x12,%esi`
    // 先于 SetETC ⇒ 两值均为调用点内联实参（P52 右→左求值），禁具名结果局部。
    user->getHackAnalyzer()->addServerHackCnt(
        user, (WongWork::ENUM_HACKTYPE)hackType, 1u,
        (unsigned int)user->SetETC((short)hackType, (int)req.m_field11),
        (unsigned int)req.m_field12);
    return 0;
}