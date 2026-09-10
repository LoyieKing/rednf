int AradAppSystem::AradAppInit(int argc, char** argv)
{
    std::cerr << "[!] Server Starting...\n";
    srand(time(0));
    if (argc != 2)
    {
        std::cerr << "[!] Check Args Fail!!\n";
        return 0;
    }
    if (!open_log(argv[1]))
        return 0;
    if (!HistoryLog::Init())
        return 0;
    if (!script_load())
        return 0;
    return 1;
