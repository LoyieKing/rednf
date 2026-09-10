void Neof_SignalHandler(int sig)
{
    G_TraceLog()->sysLog(2, "Handling Signal %d", sig);
    switch (sig)
    {
    case SIGTERM:
        LinuxService::getInstance()->controlPause();
        break;
    case SIGUSR2:
        LinuxService::getInstance()->SetStopReceived();
        break;
    case SIGABRT:
    case SIGSEGV:
        Neof_dumpCoreFile();
        break;
    case SIGFPE:
        Neof_dumpCoreFile();
        break;
    case SIGILL:
    case SIGBUS:
    case SIGSTKFLT:
    case SIGURG:
    case SIGXCPU:
    case SIGXFSZ:
    case SIGSYS:
        Neof_dumpCoreFile();
        exit(-1);
        break;
    }
