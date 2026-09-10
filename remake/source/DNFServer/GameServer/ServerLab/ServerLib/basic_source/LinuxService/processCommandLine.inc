void nsl::LinuxService::processCommandLine(int argc, char** argv, char* ServiceName, bool isSignalRegist)
{
    puts("In LinuxService::processCommandLine");
    setInfo("ChannelBridge", "ChannelBridge", *argv, argv[1]);
    if (isSignalRegist)
    {
        prepareStart();
    }
    readConfig(ServiceName);
    char* pszCmdLine = argv[2];
    strncpy(m_command, pszCmdLine, 0x1e);
    printf("ServiceName='%s'\n", ServiceName);
    memset(gServiceName, 0, 0x14);
    strcpy(gServiceName, ServiceName);
    if (strstr(pszCmdLine, "install") != NULL)
    {
        _evlog("ProcessCommandLine2");
        install();
    }
    else if (strstr(pszCmdLine, "remove") != NULL)
    {
        _evlog("ProcessCommandLine3");
        remove();
    }
    else if (strstr(pszCmdLine, "run") != NULL || strstr(pszCmdLine, "test") != NULL)
    {
        _evlog("ProcessCommandLine4");
        if (checkConfigFile() != true)
        {
            printf("There is not %s config_file\n", getInstance()->getConfigFileName());
            return;
        }
        if (checkPIDFile() != false)
        {
            printf("Process(%s) is already operated.\n", getInstance()->getChannelName());
            return;
        }
        setRunAsService(false);
        start();
    }
    else if (strstr(pszCmdLine, "start") != NULL)
    {
        puts("\xB8\xAE\xB4\xAA\xBD\xBA \xBC\xAD\xBA\xF1\xBD\xBA \xBD\xC3\xC0\xDB~");
        _evlog("ProcessCommandLine5");
        if (checkConfigFile() != true)
        {
            printf("There is not %s config_file\n", getInstance()->getConfigFileName());
            return;
        }
        setRunAsService(true);
        start();
    }
    else if (strstr(pszCmdLine, "stop") != NULL)
    {
        bool bRet = Neof_sendTerminateSignal(ServiceName);
        if (bRet)
        {
            puts("Neof_sendTerminateSignal return OK");
        }
        else
        {
            puts("Neof_sendTerminateSignal return FAIL");
        }
        if (bRet)
        {
            delete_pid();
        }
    }
    else if (strstr(pszCmdLine, "pause") != NULL)
    {
        Neof_sendSuspendSignal();
    }
    else if (strstr(pszCmdLine, "mess") != NULL)
    {
        sendBroadCastMessage(argv[3]);
    }
    else if (strstr(pszCmdLine, "info") != NULL)
    {
        SendInfoMessage(ENUM_OTP);
    }
    puts("Out LinuxService::processCommandLine");
