// df_relay_r — DNFRelayServer（App/main，GCC 4.1.2, 无 DWARF）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "DNFRelayServer.h"
#include "RelayService.h"
#include "Helper.h"
#include "ScriptData.h"
#include "Script.h"
#include "SocketSystem.h"
#include "Exception.h"

RelayServiceApp::RelayService* g_pService = 0;

// ---- App（LinuxService 纯虚实现，主体后续补齐）----

App::App()
    : LinuxService()
{
}

App::~App()
{
}

void App::prepareRun()
{
    puts("Called prepareRun");
    TSocketSystem<SocketSystemLinux>::startup();
    g_pService = new RelayServiceApp::RelayService;
    g_pService->setMode(RelayServiceApp::RelayService::MODE_NONE);
    g_pService->m_portInfo.setTCPPort(G_ScriptData()->mPortTcp);
    g_pService->m_portInfo.setUDPPort(G_ScriptData()->mPortUdp);
    g_pService->m_portInfo.setUDPS2SPort(G_ScriptData()->mReservedB);
}

void App::run()
{
    puts("Called run");
    g_pService->startup();
    while (m_terminated != 1)
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

bool App::load_script()
{
    char path[0x100];
    snprintf(path, 0x100, "./cfg/%s.cfg", (char*)this + 0x404);
    printf("[!] Server environment(%s) script loading : %s\n", (char*)this + 0x404, path);
    bool loaded = G_Script()->load(path);
    if (!loaded)
    {
        printf("Can't open script file : %s", (char*)this + 0x404);
        return false;
    }
    loaded = G_Script()->parse_channel_script();
    if (!loaded)
    {
        return false;
    }
    return true;
}

void App::readConfig()
{
    puts("Called readConfig");
    bool loaded = load_script();
    if (!loaded)
    {
        puts("ERROR readConfig()");
    }
    else
    {
        puts("OK readConfig()");
    }
}

int main(int argc, char** argv)
{
    if ((argc > 1) && (strcmp(argv[1], "version") == 0))
    {
        puts("Relay Server v2.5.6.9");
        return 0;
    }
    try
    {
        App* app = new App;
        app->processCommandLine(argc, argv);
        app->main();
    }
    catch (Exception& e)
    {
        if (e.getFunctionName() != 0)
        {
            printf("# %d %s at %d in %s\n", e.getErrorCode(), e.getErrorMsg(), e.getLine(),
                   e.getFunctionName());
        }
        else
        {
            printf("# %d %s\n", e.getErrorCode(), e.getErrorMsg());
        }
    }
    return 0;
}
