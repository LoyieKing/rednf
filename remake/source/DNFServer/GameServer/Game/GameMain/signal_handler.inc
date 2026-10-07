// [b28d-28] DIFF | ORIG 0x0829f28b | P167 CLEAR（48/48）；N82 槽/寄存器分配差
// （我方多 28 条）。
// [b28d-solo-GameMain] AE | 121/122 | p167 48/48 CLEAR | 修法：CPacketTracerInstance()/
// G_CEnvironment()/G_GameWorld() 全表达式直用（P0，消 tracer/env/world 具名局部）+
// P227 cMyTrace 全表达式临时链（userCount 跨 ctor %ebx 保活）。
void signal_handler(int sig)
{
    switch (sig)
    {
    case 15:
    {
        std::puts("RECV SIGTERM");
        rdar_terminate();
        // P0：ORIG `call CPacketTracerInstance; mov %eax,(%esp); call ...` 无具名局部。
        CPacketTracerInstance()->AbsoluteTcpWriteLog();
        CPacketTracerInstance()->AbsoluteUdpWriteLog();
        break;
    }
    case 10:
    {
        std::printf("RDAR_SUSPEND pointer: %p\n", G_CEnvironment());
        G_CEnvironment()->set_suspend_state(true);
        break;
    }
    case 12:
    {
        std::puts("RECV SIGUSR2");
        std::printf("G_CEnvironment pointer: %p\n", G_CEnvironment());
        G_CEnvironment()->set_suspend_state(true);
        GetInstanceShutdowManager()->Reset();
        GetInstanceShutdowManager()->EnableShutdown(true);
        // P227 全表达式临时链：ORIG 先 `call get_UserCount_InWorld` 入 %ebx，
        // 再 cMyTrace ctor，ctor 后 `mov %ebx,0x8(%esp)`（无具名 userCount 槽）。
        cMyTrace(__PRETTY_FUNCTION__, 0x2459, 0)(
            "current user count (%d)\n", G_GameWorld()->get_UserCount_InWorld());
        if (G_GameWorld()->get_UserCount_InWorld() < 1)
        {
            GetInstanceShutdowManager()->OnAllUserLogout();
        }
        else
        {
            G_GameWorld()->arrange_users();
        }
        break;
    }
    case 6:
    case 11:
    {
        std::puts("Recv SIGSEGV signal --> make Dump Core file.");
        CPacketTracerInstance()->AbsoluteTcpWriteLog();
        CPacketTracerInstance()->AbsoluteUdpWriteLog();
        omll_dump_core_file();
        break;
    }
    case 8:
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__,
                              0x2478, "Recv FPE signal");
        CPacketTracerInstance()->AbsoluteTcpWriteLog();
        CPacketTracerInstance()->AbsoluteUdpWriteLog();
        omll_dump_core_file();
        break;
    }
    case 5:
	case 9:
	case 13:
	case 14:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 26:
	case 27:
	case 28:
	case 29:  // [DNF-FIX 2026-10-08 c23] ORIG 0x0829f28b jump table 0x8c184b8: 29/30 -> default (no-op)
	case 30:
		break;
	case 4:
    case 7:
    case 16:
    case 23:
    case 24:
    case 25:
    case 31:
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__,
                              0x2486,
                              "Recv %d signal --> process is terminated abnormaly.",
                              sig);
        CPacketTracerInstance()->AbsoluteTcpWriteLog();
        CPacketTracerInstance()->AbsoluteUdpWriteLog();
        omll_dump_core_file();
        std::exit(-1);
        break;
    }
}