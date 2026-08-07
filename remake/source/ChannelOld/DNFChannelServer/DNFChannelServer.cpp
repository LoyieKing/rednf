#include "DNFChannelServer.h"
#include "SocketSystem.h"
#include "GlobalInstance.h"
#include "Script.h"
#include "ScriptData.h"
#include "Exception.h"
#include <stdio.h>

ChannelServiceApp::ChannelService* g_pService = NULL;

Script* G_Script()
{
    return GlobalInstance<Script>::inst_ptr();
}

void ChannelServiceApp::ChannelService::setMode(Mode in_Mode)
{
    Mode_ = in_Mode;
}

void ChannelServiceApp::PortInfo::setTCPPort(int n)
{
    nTCPPort_ = n;
}

void ChannelServiceApp::PortInfo::setUDPPort(int n)
{
    nUDPPort_ = n;
}

App::App()
{
}

void App::readConfig()
{
    puts("Called readConfig");
    bool ret = load_script();
    if (ret == false)
    {
        puts("ERROR readConfig()");
    }
    else
    {
        puts("OK readConfig()");
    }
}

bool App::load_script()
{
    char filename[256];
    sprintf(filename, "./cfg/%s.cfg", ServiceInfo::getConfigFileName());
    printf("[!] Server environment(%s) script loading : %s\n", ServiceInfo::getConfigFileName(), filename);
    bool ret = G_Script()->load(filename);
    if (ret == false)
    {
        printf("Can't open script file : %s", ServiceInfo::getConfigFileName(), filename);
    }
    else
    {
        ret = G_Script()->parse_channel_script();
    }
    return ret;
}

void App::prepareRun(char* service_identify)
{
    puts("Called prepareRun");
    TSocketSystem<SocketSystemLinux>::startup();
    g_pService = new ChannelServiceApp::ChannelService(service_identify);
    g_pService->setMode(ChannelServiceApp::ChannelService::UDP2TCP);
    g_pService->setUDPPort(G_ScriptData()->udp_port);
    g_pService->setTCPPort(G_ScriptData()->tcp_port);
}

void App::run()
{
    puts("Called run");
    g_pService->startup();
    while (isTerminated_ != true)
    {
        TSystem<LinuxSystem>::sleep(10000);
    }
}

void App::finishRun()
{
    puts("Called finishRun");
}

void App::stop()
{
    puts("Called stop");
    g_pService->shutdown();
    delete g_pService;
}

void App::onStop()
{
    puts("Called onStop");
}

void App::onPause()
{
    puts("Called onPause");
}

void App::onContinue()
{
    puts("Called onContinue");
}

int Exception::getErrorCode() const
{
    return iErrorCode_;
}

TCHAR* Exception::getErrorMsg() const
{
    return (TCHAR*)szErrorMsg_;
}

const char* Exception::getFunctionName() const
{
    return szFunctionNameA_;
}

int Exception::getLine() const
{
    return nLine_;
}

template class GlobalInstance<GLOBAL>;
template class GlobalInstance<Script>;

int main(int argc, char** argv)
{
    App* tt;
    try
    {
        tt = new App;
        tt->processCommandLine(argc, argv);
        tt->main(argv[1]);
    }
    catch (Exception& e)
    {
        if (e.getFunctionName() != NULL)
        {
            printf("# %d %s at %d in %s\n", e.getErrorCode(), e.getErrorMsg(), e.getLine(), e.getFunctionName());
        }
        else
        {
            printf("# %d %s\n", e.getErrorCode(), e.getErrorMsg());
        }
    }
    return 0;
}
