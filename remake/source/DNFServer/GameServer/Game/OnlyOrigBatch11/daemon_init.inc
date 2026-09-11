// ORIG T 0x0829e8b9：argv[2]（argv+8 处指针）为 "start"/"start_all" 时
// fork 守护化（父退出、setsid、chdir("./")、umask(0)）；最后 save_pid()，
// 成功返 0、fork 失败返 -1。
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
}