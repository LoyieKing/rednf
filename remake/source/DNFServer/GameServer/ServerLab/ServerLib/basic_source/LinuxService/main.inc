void nsl::LinuxService::main(char* service_identify)
{
    puts("In LinuxService::main");
    try
    {
        if (std::strstr(m_command, "start") != NULL)
        {
            readConfig(service_identify);
            prepareRun(service_identify);
            run();
        }
        else if (std::strstr(m_command, "run") != NULL)
        {
            readConfig(service_identify);
            prepareRun(service_identify);
            run();
        }
        else if (std::strstr(m_command, "stop") != NULL)
        {
            readConfig(service_identify);
            stopPending();
        }
    }
    catch (...)
    {
        return;
    }
    puts("Out LinuxService::main");
