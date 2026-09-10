bool Neof_registerSignalHandlers()
{
    bool ret = false;
    if (!Neof_registerSignalHandler(SIGTERM, Neof_SignalHandler))
    {
        return false;
    }
    if (!Neof_registerSignalHandler(SIGUSR2, Neof_SignalHandler))
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGPIPE, (void (*)(int))SIG_IGN);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGSEGV, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGSTOP, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGFPE, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGTRAP, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGILL, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGBUS, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGSTKFLT, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGURG, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGXCPU, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGXFSZ, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGSYS, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    return true;
