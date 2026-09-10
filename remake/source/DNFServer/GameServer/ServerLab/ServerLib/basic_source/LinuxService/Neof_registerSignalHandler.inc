bool Neof_registerSignalHandler(int sig, void (*func_name)(int))
{
    struct sigaction act;
    struct sigaction oact;
    act.sa_handler = func_name;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (sig == 0xe)
    {
        act.sa_flags |= 0x20000000;
    }
    else
    {
        act.sa_flags |= 0x10000000;
    }
    if (sigaction(sig, &act, &oact) < 0)
    {
        printf("%d\xB9\xF8 signal \xB5\xEE\xB7\xCF \xBD\xC7\xC6\xD0\n", sig);
        return false;
    }
    return true;
