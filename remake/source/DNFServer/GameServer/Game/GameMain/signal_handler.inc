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
		break;
	case 4:
    case 7:
    case 16:
    case 23:
    case 24:
    case 25:
    case 29:
    case 30:
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
