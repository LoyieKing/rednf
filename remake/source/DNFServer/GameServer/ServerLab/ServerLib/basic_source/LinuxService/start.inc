void nsl::LinuxService::start()
{
    puts("In LinuxService::start");
    if (m_bRunAsService)
    {
        _evlog("Run As Service\n");
        make_dir("./pid");
        pid_t pid = fork();
        if (pid < 0)
        {
            puts("fork Fail");
            return;
        }
        if (pid != 0)
        {
            exit(0);
        }
        setsid();
        chdir("./");
        umask(0);
        readConfig(NULL);
        save_pid(getPIDFileName());
    }
    else
    {
        _evlog("Run As Non Service\n");
    }
    puts("Out LinuxService::start");
