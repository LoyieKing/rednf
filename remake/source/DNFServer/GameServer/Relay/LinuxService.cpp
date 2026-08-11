// df_relay_r — LinuxService（GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "LinuxService.h"
#include "Helper.h"
#include "PIDHelper.h"
#include "SignalHandler.h"

static void __attribute__((regparm(0))) _evlog(const char* sz)
    __asm__("_ZL6_evlogPKc");
static void __attribute__((regparm(0))) _evlog(const char* sz)
{
    puts(sz);
}

void ServiceInfo::setPIDFileName(const char* fn)
{
    if (fn != 0)
    {
        strncpy(m_pidFileName, fn, 0xff);
        m_pidFileName[0xff] = 0;
    }
    else
    {
        m_pidFileName[0] = 0;
    }
}

void ServiceInfo::setInfo(const char* name, const char* display,
                          const char* binary, const char* config)
{
    if (name != 0)
    {
        strncpy(m_serviceName, name, 0xff);
        m_serviceName[0xff] = 0;
    }
    else
    {
        m_serviceName[0] = 0;
    }
    if (display != 0)
    {
        strncpy(m_displayName, display, 0xff);
        m_displayName[0xff] = 0;
    }
    else
    {
        m_displayName[0] = 0;
    }
    if (binary != 0)
    {
        strncpy(m_binaryName, binary, 0xff);
        m_binaryName[0xff] = 0;
    }
    else
    {
        m_binaryName[0] = 0;
    }
    if (config != 0)
    {
        strncpy(m_configFileName, config, 0xff);
        m_configFileName[0xff] = 0;
        setPIDFileName(m_configFileName);
    }
    else
    {
        m_configFileName[0] = 0;
    }
}

LinuxService* LinuxService::theInstance = 0;

LinuxService::LinuxService()
    : ServiceInfo(), IService()
{
    m_terminated = false;
    m_dwServiceState = 0;
    theInstance = this;
    memset(m_command, 0, 0x1e);
}

LinuxService::~LinuxService()
{
}

LinuxService* LinuxService::getInstance()
{
    return theInstance;
}

bool LinuxService::checkConfigFile()
{
    char path[30] = {0};
    snprintf(path, 0x1e, "./cfg/%s.cfg", getConfigFileName());
    puts(path);
    if (access(path, 0) == 0)
    {
        return true;
    }
    return false;
}

bool LinuxService::checkPIDFile()
{
    char path[30] = {0};
    snprintf(path, 0x1e, "./pid/%s.pid", getPIDFileName());
    puts(path);
    if (access(path, 0) == 0)
    {
        return true;
    }
    return false;
}

bool LinuxService::prepareStart()
{
    if (!Neof_registerSignalHandlers())
    {
        puts("Fail to regist os signals, Server process is terminated.");
        return false;
    }
    Neof_setCoreLimit();
    return true;
}

void LinuxService::install()
{
}

void LinuxService::remove()
{
}

void LinuxService::start()
{
    if (m_bRunAsService)
    {
        _evlog("Run As Service\n");
        make_dir("./pid");
        make_dir("./log");
        pid_t pid = fork();
        if (pid < 0)
        {
            puts("fork Fail");
        }
        else
        {
            if (pid != 0)
            {
                exit(0);
            }
            setsid();
            chdir("./");
            umask(0);
            save_pid();
        }
    }
    else
    {
        _evlog("Run As Non Service\n");
    }
}

void LinuxService::sendBroadCastMessage(const char* msg)
{
}

void LinuxService::controlStop()
{
    m_terminated = true;
}

void LinuxService::controlPause()
{
}

void LinuxService::controlContinue()
{
}

void LinuxService::setServiceState(unsigned long state)
{
}

void LinuxService::stopPending()
{
    stop();
}

void LinuxService::setRunning()
{
}

void LinuxService::setStopped()
{
}

void LinuxService::main()
{
    try
    {
        if (strstr(m_command, "start") != 0)
        {
            readConfig();
            prepareRun();
            run();
        }
        else if (strstr(m_command, "run") != 0)
        {
            readConfig();
            prepareRun();
            run();
        }
        else if (strstr(m_command, "stop") != 0)
        {
            readConfig();
            stopPending();
        }
    }
    catch (...)
    {
    }
}

void LinuxService::processCommandLine(int argc, char** argv)
{
    setInfo("RelayServer", "RelayServer", *argv, argv[1]);
    if (prepareStart() == 0)
    {
        puts("Fail to Preprocessing, Server process is terminated.");
    }
    else
    {
        char* pszCmdLine = argv[2];
        strncpy(m_command, pszCmdLine, 0x1d);
        m_command[0x1d] = 0;
        if (strstr(pszCmdLine, "install") != 0)
        {
            _evlog("ProcessCommandLine2");
            install();
        }
        else if (strstr(pszCmdLine, "remove") != 0)
        {
            _evlog("ProcessCommandLine3");
            remove();
        }
        else if (strstr(pszCmdLine, "run") != 0 || strstr(pszCmdLine, "test") != 0)
        {
            _evlog("ProcessCommandLine4");
            if (checkConfigFile() == 0)
            {
                printf("There is not %s config_file\n",
                       getInstance()->getConfigFileName());
            }
            else
            {
                if (checkPIDFile() != 0)
                {
                    printf("Process(%s) is already operated.\n",
                           getInstance()->getChannelName());
                }
                else
                {
                    setRunAsService(false);
                    start();
                }
            }
        }
        else if (strstr(pszCmdLine, "start") != 0)
        {
            _evlog("ProcessCommandLine5");
            if (checkConfigFile() == 0)
            {
                printf("There is not %s config_file\n",
                       getInstance()->getConfigFileName());
                exit(0);
            }
            if (checkPIDFile() != 0)
            {
                printf("Process(%s) is already operated.\n",
                       getInstance()->getChannelName());
                exit(0);
            }
            setRunAsService(true);
            start();
        }
        else if (strstr(pszCmdLine, "stop") != 0)
        {
            bool r = Neof_sendTerminateSignal();
            if (r)
            {
                puts("Neof_sendTerminateSignal return OK");
            }
            else
            {
                puts("Neof_sendTerminateSignal return FAIL");
            }
            if (r)
            {
                delete_pid();
            }
        }
        else if (strstr(pszCmdLine, "pause") != 0)
        {
            Neof_sendSuspendSignal();
        }
        else if (strstr(pszCmdLine, "mess") != 0)
        {
            sendBroadCastMessage(argv[3]);
        }
    }
}
