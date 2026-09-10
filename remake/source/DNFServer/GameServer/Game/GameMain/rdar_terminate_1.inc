void rdar_terminate()
{
    char buf[30];
    std::memset(buf, 0, sizeof(buf));
    std::sprintf(buf, "./pid/%s.pid", G_CEnvironment()->get_file_name());
    std::printf("???%s\n", buf);
    if (std::remove(buf) == -1)
    {
        std::printf("FAIL TO DELETE PID FILE ERROR: %s\n",
                    strerror(*__errno_location()));
    }
    else
    {
        std::puts("RDAR PROCESS IS TERMINATED.");
    }
    G_GameWorld()->arrange_users();
    TerminateServers();
