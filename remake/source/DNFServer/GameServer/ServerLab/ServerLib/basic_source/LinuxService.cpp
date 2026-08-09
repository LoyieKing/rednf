// Rebuilt from df_auction_r (DWARF + Ghidra decompile), 2026-08-07
#include "LinuxService.h"

#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>

// The original toolchain's <cstring> provided std::strstr(char*, const char*)
// (weak, out-of-line at -O0).  The c6root glibc defines
// __CORRECT_ISO_CPP_STRING_H_PROTO so this overload is absent; reproduce the
// original _ZSt6strstrPcPKc symbol.
#include "Script.h"
#include "TraceLog.h"

namespace std
{
char* strstr(char* __s1, const char* __s2) throw()
{
    return ::strstr(__s1, __s2);
}
}

namespace nsl {

char gServiceName[0x14];
LinuxService* LinuxService::theInstance = NULL;

static void _evlog(LPCTSTR sz)
{
    puts(sz);
}

LinuxService::LinuxService()
{
    theInstance = this;
    isTerminated_ = false;
    mbStopRecevied = false;
}

LinuxService::~LinuxService()
{
}

bool nsl::LinuxService::checkConfigFile()
{
    puts("In LinuxService::checkConfigFile");
    char file_path[0x100];
    memset(file_path, 0, 0x100);
    puts(file_path);
    if (access(getConfigFileName(), 0) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool nsl::LinuxService::checkPIDFile()
{
    puts("In LinuxService::checkPIDFile");
    char file_path[0x100];
    memset(file_path, 0, 0x100);
    sprintf(file_path, "%s/%s.pid", G_Script()->findCharValue(0, 0), getPIDFileName());
    puts(file_path);
    if (access(file_path, 0) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void nsl::LinuxService::prepareStart()
{
    Neof_setCoreLimit();
    Neof_registerSignalHandlers();
}

void nsl::LinuxService::install()
{
}

void nsl::LinuxService::remove()
{
}

void nsl::LinuxService::start()
{
    puts("In LinuxService::start");
    if (m_bRunAsService)
    {
        _evlog("Run As Service\n");
        make_dir("./pid");
        pid_t pid = fork();
        if (pid < 0)
        {
            puts("fork Fail");
            return;
        }
        if (pid != 0)
        {
            exit(0);
        }
        setsid();
        chdir("./");
        umask(0);
        readConfig(NULL);
        save_pid(getPIDFileName());
    }
    else
    {
        _evlog("Run As Non Service\n");
    }
    puts("Out LinuxService::start");
}

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
}

void nsl::LinuxService::sendBroadCastMessage(const char* szMsg)
{
}

void nsl::LinuxService::SendInfoMessage(ENUM_INFOTYPE infoType)
{
    char buf[10] = {0};
    buf[0] = (char)infoType;
}

void nsl::LinuxService::controlStop()
{
    isTerminated_ = true;
}

void nsl::LinuxService::controlPause()
{
}

void nsl::LinuxService::controlContinue()
{
}

void nsl::LinuxService::setStopped()
{
}

void nsl::LinuxService::stopPending()
{
    stop();
}

void nsl::LinuxService::setRunning()
{
}

void nsl::LinuxService::setServiceState(DWORD p_State)
{
}

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
}

void nsl::ServiceInfo::setPIDFileName(const char* p_szPIDFileName)
{
    puts("In ServiceInfo::setPIDFileName");
    std::string pidFileName(p_szPIDFileName);
    int size = pidFileName.find_last_of(".") - pidFileName.find_last_of("/") - 1;
    pidFileName = pidFileName.substr(pidFileName.find_last_of("/") + 1, size);
    if (p_szPIDFileName != NULL)
    {
        strncpy(m_szPIDFileName, pidFileName.c_str(), 0x100);
    }
    else
    {
        m_szPIDFileName[0] = '\0';
    }
    puts("Out ServiceInfo::setPIDFileName");
}

void nsl::ServiceInfo::setInfo(const char* p_szServiceName, const char* p_szDisplayName,
                               const char* p_szBinaryName, const char* p_szConfigFileName)
{
    puts("In ServiceInfo::setInfo");
    if (p_szServiceName != NULL)
    {
        strncpy(m_szServiceName, p_szServiceName, 0x100);
    }
    else
    {
        m_szServiceName[0] = '\0';
    }
    if (p_szDisplayName != NULL)
    {
        strncpy(m_szDisplayName, p_szDisplayName, 0x100);
    }
    else
    {
        m_szDisplayName[0] = '\0';
    }
    if (p_szBinaryName != NULL)
    {
        strncpy(m_szBinaryName, p_szBinaryName, 0x100);
    }
    else
    {
        m_szBinaryName[0] = '\0';
    }
    if (p_szConfigFileName != NULL)
    {
        strncpy(m_szConfigFileName, p_szConfigFileName, 0x100);
        setPIDFileName(m_szConfigFileName);
    }
    else
    {
        m_szConfigFileName[0] = '\0';
    }
    puts("Out ServiceInfo::setInfo");
}

bool Neof_registerSignalHandler(int sig, void (*func_name)(int))
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
        printf("%d\xB9\xF8 signal \xB5\xEE\xB7\xCF \xBD\xC7\xC6\xD0\n", sig);
        return false;
    }
    return true;
}

void Neof_SignalHandler(int sig)
{
    G_TraceLog()->sysLog(2, "Handling Signal %d", sig);
    switch (sig)
    {
    case SIGTERM:
        LinuxService::getInstance()->controlPause();
        break;
    case SIGUSR2:
        LinuxService::getInstance()->SetStopReceived();
        break;
    case SIGABRT:
    case SIGSEGV:
        Neof_dumpCoreFile();
        break;
    case SIGFPE:
        Neof_dumpCoreFile();
        break;
    case SIGILL:
    case SIGBUS:
    case SIGSTKFLT:
    case SIGURG:
    case SIGXCPU:
    case SIGXFSZ:
    case SIGSYS:
        Neof_dumpCoreFile();
        exit(-1);
        break;
    }
}

bool Neof_registerSignalHandlers()
{
    bool ret = false;
    if (!Neof_registerSignalHandler(SIGTERM, Neof_SignalHandler))
    {
        return false;
    }
    if (!Neof_registerSignalHandler(SIGUSR2, Neof_SignalHandler))
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGPIPE, (void (*)(int))SIG_IGN);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGSEGV, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGSTOP, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGFPE, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGTRAP, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGILL, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGBUS, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGSTKFLT, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGURG, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGXCPU, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGXFSZ, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    ret = Neof_registerSignalHandler(SIGSYS, Neof_SignalHandler);
    if (!ret)
    {
        return false;
    }
    return true;
}

void Neof_dumpCoreFile()
{
    struct rlimit rlp;
    int ret_val;
    ret_val = getrlimit(RLIMIT_CORE, &rlp);
    rlp.rlim_cur = 0xffffffff;
    ret_val = setrlimit(RLIMIT_CORE, &rlp);
    abort();
}

void Neof_setCoreLimit()
{
    struct rlimit rlp;
    int ret_val;
    ret_val = getrlimit(RLIMIT_CORE, &rlp);
    rlp.rlim_cur = 0xffffffff;
    ret_val = setrlimit(RLIMIT_CORE, &rlp);
}

bool Neof_sendTerminateSignal(char* ServiceName)
{
    puts("called Neof_sendTerminateSignal");
    char file_path[0x100];
    memset(file_path, 0, 0x100);
    sprintf(file_path, "%s/%s.pid", G_Script()->findCharValue(0, 0), LinuxService::getInstance()->getPIDFileName());
    printf("killing path [%s]\n", file_path);
    // ORIG DWARF 声明序：ret(238)、pid(239)、fp(240)；声明不初始化，体中赋值。
    int ret;
    int pid;
    FILE* fp;
    fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        printf("%s process id file open \xBD\xC7\xC6\xD0\n", file_path);
        return false;
    }
    fscanf(fp, "%d", &pid);
    printf("killing [%s], PID=[%d]\n", file_path, pid);
    if (pid < 1)
    {
        fclose(fp);
        printf("%d\xB9\xF8\xC0\xC7 \xC0\xDF\xB8\xF8\xB5\xC8 process id\n", pid);
        return false;
    }
    ret = kill(pid, 0xc);
    if (ret != 0)
    {
        fclose(fp);
        printf("error, [ret = %d] called Neof_sendTerminateSignal()\n", ret);
        printf("error, %d\xB9\xF8 process\xB7\xCE \xC1\xBE\xB7\xE1 signal \xBC\xDB\xBD\xC5 \xBD\xC7\xC6\xD0 error = '%s'\n", pid, strerror(errno));
        return false;
    }
    puts("************************************************************");
    printf("******************kill pid=\'%5d\' Success!!!***************\n", pid);
    puts("************************************************************");
    fclose(fp);
    return true;
}

void Neof_sendSuspendSignal()
{
    char file_path[0x100];
    memset(file_path, 0, 0x100);
    sprintf(file_path, "%s/%s.pid", G_Script()->findCharValue(0, 0), LinuxService::getInstance()->getPIDFileName());
    sprintf(file_path, "%s/%s.pid", G_Script()->findCharValue(0, 0), LinuxService::getInstance()->getPIDFileName());
    // ORIG DWARF 声明序：ret(344)、pid(345)、fp(346)；声明不初始化，体中赋值。
    int ret;
    int pid;
    FILE* fp;
    fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        printf("%s process id file open \xBD\xC7\xC6\xD0\n", file_path);
    }
    else
    {
        fscanf(fp, "%d", &pid);
        if (pid < 1)
        {
            fclose(fp);
            printf("%d\xB9\xF8\xC0\xC7 \xC0\xDF\xB8\xF8\xB5\xC8 process id\n", pid);
        }
        else
        {
            ret = kill(pid, 0xc);
            if (ret < 0)
            {
                fclose(fp);
                printf("%d\xB9\xF8 process\xB7\xCE \xC1\xBE\xB7\xE1 signal \xBC\xDB\xBD\xC5 \xBD\xC7\xC6\xD0", pid);
            }
            else
            {
                printf("SEND SUSPEND SIGNAL TO %d\n", pid);
                fclose(fp);
            }
        }
    }
}

bool delete_pid()
{
    char file_path[0x100];
    memset(file_path, 0, 0x100);
    sprintf(file_path, "%s/%s.pid", G_Script()->findCharValue(0, 0), LinuxService::getInstance()->getPIDFileName());
    printf("delete_pid() : file[%s]\n", file_path);
    if (unlink(file_path) == 0)
    {
        printf("delete %s\n", file_path);
        return true;
    }
    else
    {
        return false;
    }
}

bool save_pid(char* pidName)
{
    char file_path[0x100];
    memset(file_path, 0, 0x100);
    sprintf(file_path, "%s/%s.pid", G_Script()->findCharValue(0, 0), pidName);
    printf("pid file_path-%s\n", file_path);
    // ORIG DWARF 声明序：buf(618)、write_byte(619)、fd(620)——fd 在 buf 后声明。
    char buf[0x200];
    ssize_t write_byte;
    int fd = open(file_path, 0x42, 0x1a4);
    if (fd < 0)
    {
        return false;
    }
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

void make_dir(char* szdir_name)
{
    puts("In make_dir");
    DIR* p_dir = opendir(szdir_name);
    if (p_dir == NULL)
    {
        mkdir(szdir_name, 0x1ff);
    }
    puts("Out make_dir");
}

} // namespace nsl
