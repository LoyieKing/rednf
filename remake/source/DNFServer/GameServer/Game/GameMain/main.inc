// ---------------------------------------------------------------------------
// main —— 入口（ORIG 0x829f839）
// ---------------------------------------------------------------------------
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
    // [N3Free171to190·GameMain] AE 修法（P339 块作用域=槽序）：ORIG 的 cMyTrace 是
    // **函数体 toplevel** 对象（ORIG 后端先分配 toplevel 变量 ⇒ 槽序 trace 先于
    // 块内的 pidFile；4.4.7 相反）——去块后 pidFile 0x1e / trace 0x3c / inited 0x4f
    // 与 ORIG 逐字相同（70=70 AE，帧 0x50）。cMyTrace 析构 trivial（无 dtor call）。
    cMyTrace trace(__PRETTY_FUNCTION__, 0x25e6, 0);
    trace("GAME SERVER SHUTDOWN COMPLETE\n");
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
}