#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <fcntl.h>

#include "Service.h"

static void _evlog(const char* sz);

LinuxService* LinuxService::theInstance = NULL;

void ServiceInfo::setPIDFileName(const char* szPIDFileName)
{
    if (szPIDFileName != NULL)
    {
        strncpy(m_szPIDFileName, szPIDFileName, 0x100);
    }
    else
    {
        m_szPIDFileName[0] = '\0';
    }
}

void ServiceInfo::setInfo(const char* szServiceName, const char* szDisplayName, const char* szBinaryName, const char* szConfigFileName)
{
    if (szServiceName != NULL)
    {
        strncpy(m_szServiceName, szServiceName, 0x100);
    }
    else
    {
        m_szServiceName[0] = '\0';
    }
    if (szDisplayName != NULL)
    {
        strncpy(m_szDisplayName, szDisplayName, 0x100);
    }
    else
    {
        m_szDisplayName[0] = '\0';
    }
    if (szBinaryName != NULL)
    {
        strncpy(m_szBinaryName, szBinaryName, 0x100);
    }
    else
    {
        m_szBinaryName[0] = '\0';
    }
    if (szConfigFileName != NULL)
    {
        strncpy(m_szConfigFileName, szConfigFileName, 0x100);
        setPIDFileName(m_szConfigFileName);
    }
    else
    {
        m_szConfigFileName[0] = '\0';
    }
}

void ServiceInfo::setRunAsService(bool bRunService)
{
    m_bRunAsService = bRunService;
}

const char* ServiceInfo::getPIDFileName() const
{
    return m_szPIDFileName;
}

const char* ServiceInfo::getConfigFileName() const
{
    return m_szConfigFileName;
}

const char* ServiceInfo::getChannelName() const
{
    return m_szChannelName;
}

LinuxService::LinuxService()
{
    theInstance = this;
    isTerminated_ = 0;
}

LinuxService::~LinuxService()
{
}

LinuxService* LinuxService::getInstance()
{
    return theInstance;
}

void LinuxService::setServiceState(DWORD p_State)
{
}

void LinuxService::setRunning()
{
}

void LinuxService::stopPending()
{
    stop();
}

void LinuxService::setStopped()
{
}

bool LinuxService::checkConfigFile()
{
    char file_path[30];
    memset(file_path, 0, 0x1e);
    sprintf(file_path, "./cfg/%s.cfg", ServiceInfo::getConfigFileName());
    puts(file_path);
    if (access(file_path, 0) == 0)
    {
        return true;
    }
    return false;
}

bool LinuxService::checkPIDFile()
{
    char file_path[30];
    memset(file_path, 0, 0x1e);
    sprintf(file_path, "./pid/%s.pid", ServiceInfo::getPIDFileName());
    puts(file_path);
    if (access(file_path, 0) == 0)
    {
        return true;
    }
    return false;
}

void LinuxService::prepareStart()
{
    Neof_setCoreLimit();
    Neof_registerSignalHandlers();
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
        make_dir((char*)"./pid");
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

void LinuxService::main(char* service_identify)
{
    try
    {
        if (strstr(m_command, "start") != NULL)
        {
            readConfig();
            prepareRun(service_identify);
            run();
        }
        else if (strstr(m_command, "run") != NULL)
        {
            readConfig();
            prepareRun(service_identify);
            run();
        }
        else if (strstr(m_command, "stop") != NULL)
        {
            readConfig();
            stopPending();
        }
    }
    catch (...)
    {
    }
}

void LinuxService::sendBroadCastMessage(const char* szMsg)
{
}

void LinuxService::controlStop()
{
    isTerminated_ = 1;
}

void LinuxService::controlPause()
{
}

void LinuxService::controlContinue()
{
}

void LinuxService::processCommandLine(int argc, char** argv)
{
    ServiceInfo::setInfo("ChannelBridge", "ChannelBridge", *argv, argv[1]);
    prepareStart();
    char* pszCmdLine = argv[2];
    strncpy(m_command, pszCmdLine, 0x1e);

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
        if ((char)checkConfigFile() != 1)
        {
            printf("There is not %s config_file\n", getInstance()->getConfigFileName());
        }
        else
        {
            if ((char)checkPIDFile() != 0)
            {
                printf("Process(%s) is already operated.\n", getInstance()->getChannelName());
            }
            else
            {
                ServiceInfo::setRunAsService(false);
                start();
            }
        }
    }
    else if (strstr(pszCmdLine, "start") != NULL)
    {
        _evlog("ProcessCommandLine5");
        if ((char)checkConfigFile() != 1)
        {
            printf("There is not %s config_file\n", getInstance()->getConfigFileName());
        }
        else
        {
            if ((char)checkPIDFile() != 0)
            {
                printf("Process(%s) is already operated.\n", getInstance()->getChannelName());
            }
            else
            {
                ServiceInfo::setRunAsService(true);
                start();
            }
        }
    }
    else if (strstr(pszCmdLine, "stop") != NULL)
    {
        bool ret = Neof_sendTerminateSignal();
        if (ret)
        {
            puts("Neof_sendTerminateSignal return OK");
        }
        else
        {
            puts("Neof_sendTerminateSignal return FAIL");
        }
        if (ret)
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
}

bool Neof_registerSignalHandler(int sig, handler func_name)
{
    struct sigaction act;
    struct sigaction oact;

    act.sa_handler = func_name;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (sig == 0xe)
    {
        act.sa_flags |= 0x20000000;
    }
    else
    {
        act.sa_flags |= 0x10000000;
    }
    if (sigaction(sig, &act, &oact) < 0)
    {
        printf("%d\xb9\xf8 signal \xb5\xee\xb7\xcf \xbd\xc7\xc6\xd0\n", sig);
        return false;
    }
    return true;
}

void Neof_SignalHandler(int sig)
{
    switch (sig)
    {
    case 0xf:
        LinuxService::getInstance()->controlStop();
        break;
    case 10:
        LinuxService::getInstance()->controlPause();
        break;
    case 6:
    case 0xb:
        Neof_dumpCoreFile();
        break;
    case 8:
        Neof_dumpCoreFile();
        break;
    case 4:
    case 7:
    case 0x10:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1f:
        Neof_dumpCoreFile();
        exit(-1);
    }
}

bool Neof_registerSignalHandlers()
{
    bool ret = false;
    if (Neof_registerSignalHandler(0xf, Neof_SignalHandler) == false) return false;
    if (Neof_registerSignalHandler(10, Neof_SignalHandler) == false) return false;
    ret = Neof_registerSignalHandler(0xd, (handler)1);
    if (ret == false) return false;
    ret = Neof_registerSignalHandler(0xb, Neof_SignalHandler);
    if (ret == false) return false;
    ret = Neof_registerSignalHandler(8, Neof_SignalHandler);
    if (ret == false) return false;
    ret = Neof_registerSignalHandler(4, Neof_SignalHandler);
    if (ret == false) return false;
    ret = Neof_registerSignalHandler(7, Neof_SignalHandler);
    if (ret == false) return false;
    ret = Neof_registerSignalHandler(0x10, Neof_SignalHandler);
    if (ret == false) return false;
    ret = Neof_registerSignalHandler(0x17, Neof_SignalHandler);
    if (ret == false) return false;
    ret = Neof_registerSignalHandler(0x18, Neof_SignalHandler);
    if (ret == false) return false;
    ret = Neof_registerSignalHandler(0x19, Neof_SignalHandler);
    if (ret == false) return false;
    ret = Neof_registerSignalHandler(0x1f, Neof_SignalHandler);
    if (ret == false) return false;
    return true;
}

void Neof_dumpCoreFile()
{
    rlimit rlp;
    int ret_val = getrlimit(RLIMIT_CORE, &rlp);
    rlp.rlim_cur = 0xffffffff;
    ret_val = setrlimit(RLIMIT_CORE, &rlp);
    abort();
}

void Neof_setCoreLimit()
{
    rlimit rlp;
    int ret_val = getrlimit(RLIMIT_CORE, &rlp);
    rlp.rlim_cur = 0xffffffff;
    ret_val = setrlimit(RLIMIT_CORE, &rlp);
}

bool Neof_sendTerminateSignal()
{
    puts("called Neof_sendTerminateSignal");
    int pid;
    int ret;
    char file_path[30];
    FILE* fp;
    memset(file_path, 0, 0x1e);
    sprintf(file_path, "pid/%s.pid", LinuxService::getInstance()->getPIDFileName());
    fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        printf("%s process id file open \xbd\xc7\xc6\xd0\n", file_path);
        return false;
    }
    fscanf(fp, "%d", &pid);
    if (pid < 1)
    {
        fclose(fp);
        printf("%d\xb9\xf8\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 process id\n", pid);
        return false;
    }
    ret = kill(pid, 0xf);
    printf("kill pid=\'%d\' error=\'%s\'\n", pid, strerror(*__errno_location()));
    if (ret < 0)
    {
        fclose(fp);
        printf("[ret = %d ]called Neof_sendTerminateSignal() : %d\xb9\xf8 process\xb7\xce \xc1\xbe\xb7\xe1 signal \xbc\xdb\xbd\xc5 \xbd\xc7\xc6\xd0", ret, pid);
        return false;
    }
    fclose(fp);
    return true;
}

void Neof_sendSuspendSignal()
{
    int pid;
    int ret;
    char file_path[30];
    memset(file_path, 0, 0x1e);
    sprintf(file_path, "pid/%s", LinuxService::getInstance()->getPIDFileName());
    FILE* fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        printf("%s process id file open \xbd\xc7\xc6\xd0\n", file_path);
    }
    else
    {
        fscanf(fp, "%d", &pid);
        if (pid < 1)
        {
            fclose(fp);
            printf("%d\xb9\xf8\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 process id\n", pid);
        }
        else
        {
            ret = kill(pid, 10);
            if (ret < 0)
            {
                fclose(fp);
                printf("%d\xb9\xf8 process\xb7\xce \xc1\xbe\xb7\xe1 signal \xbc\xdb\xbd\xc5 \xbd\xc7\xc6\xd0", pid);
            }
            else
            {
                printf("SEND SUSPEND SIGNAL TO %d\n", pid);
                fclose(fp);
            }
        }
    }
}

bool save_pid()
{
    char file_path[30];
    memset(file_path, 0, 0x1e);
    sprintf(file_path, "./pid/%s.pid", LinuxService::getInstance()->getPIDFileName());
    ssize_t write_byte;
    int fd = open(file_path, 0x42, 0x1a4);
    if (fd < 0)
    {
        return false;
    }
    char buf[512];
    memset(buf, 0, 0x200);
    sprintf(buf, "%ld\n", getpid());
    write_byte = write(fd, buf, strlen(buf));
    if (write_byte < 0)
    {
        close(fd);
        return false;
    }
    return true;
}

bool delete_pid()
{
    char file_path[30];
    memset(file_path, 0, 0x1e);
    sprintf(file_path, "./pid/%s.pid", LinuxService::getInstance()->getPIDFileName());
    printf("delete_pid() : file[%s]\n", file_path);
    if (unlink(file_path) == 0)
    {
        printf("delete %s\n", file_path);
        return true;
    }
    return false;
}

void make_dir(char* szdir_name)
{
    DIR* p_dir = opendir(szdir_name);
    if (p_dir == NULL)
    {
        mkdir(szdir_name, 0777);
    }
}

static void _evlog(const char* sz)
{
    printf(sz);
}
