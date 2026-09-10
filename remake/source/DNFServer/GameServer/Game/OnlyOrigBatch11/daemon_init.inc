int daemon_init(char** argv)
{
    const char* arg = argv[2];
    if (strcmp(arg, "start") == 0 || strcmp(arg, "start_all") == 0)
    {
        const int pid = fork();
        if (pid < 0)
            return -1;
        if (pid != 0)
            exit(0);
        setsid();
        chdir("./");
        umask(0);
    }
    save_pid();
    return 0;
