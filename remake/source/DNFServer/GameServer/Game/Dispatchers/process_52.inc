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
    const int hackType = req.m_field0d + 0x352;
    const int setEtcResult = user->SetETC((short)hackType, (int)req.m_field11);
    user->getHackAnalyzer()->addServerHackCnt(
        user, (WongWork::ENUM_HACKTYPE)hackType, 1u,
        (unsigned int)setEtcResult, (unsigned int)req.m_field12);
    return 0;
