int main(int argc, char** argv)
{
    setCodePageByLocal();
    ShowLogo();
    Neof_startupGlobalInstances();

    bool inited = Init(argc, argv);
    if (inited)
        MainLoop();

    if (!inited)
        return 1;

    // 关服线程收尾：等待 Shutdown 线程退出。
    GetInstanceShutdowManager()->WaitUntilExitThread();

    // 删除 pid 文件：sprintf("./pid/%s.pid", G_CEnvironment()->get_file_name())
    // 后 remove；失败打印 "FAIL TO DELETE PID FILE ERROR: %s\n" + strerror(errno)。
    // [A18 本波实测] 把 pidFile 声明提前到 cMyTrace 块之前无效（ndiff 18→18、
    // 槽位 0x3f/0x10/0x21 不变）⇒ 4.4 按块/使用序分配聚合槽，声明序不可翻
    // （N93/N82附 族：ORIG [pidFile 0x1e, trace 0x3c, inited 0x4f] vs 我方
    // [trace 0x10, pidFile 0x21, inited 0x3f]）。
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x25e6, 0);
        trace("GAME SERVER SHUTDOWN COMPLETE\n");
    }
    char pidFile[0x1e];
    std::memset(pidFile, 0, sizeof(pidFile));
    std::sprintf(pidFile, "./pid/%s.pid",
                 G_CEnvironment()->get_file_name());
    if (std::remove(pidFile) == -1)
        std::printf("FAIL TO DELETE PID FILE ERROR: %s\n",
                    std::strerror(errno));

    Neof_shutdownGlobalInstances();
    GlobalData::Fini();
    CLog::destroyInstance();
    std::puts("main() exited OK");
    return 0;
