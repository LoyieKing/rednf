bool regist_signal(int sig, void (*handler)(int))
{
    struct sigaction sa;
    struct sigaction oldact;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sig == 14)
        sa.sa_flags |= 0x20000000;
    else
        sa.sa_flags |= 0x10000000;
    if (sigaction(sig, &sa, &oldact) < 0)
    {
        std::printf("%d\xb9\xf8 signal \xb5\xee\xb7\xcf \xbd\xc7\xc6\xd0\n", sig);  // ORIG 文案（CP949 韩文）
        return 0;
    }
    return 1;
