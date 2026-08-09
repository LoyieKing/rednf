#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <algorithm>
#include <functional>

#include "ServerLibrary2.0.h"
#include "ServiceFactory.h"
#include "HandlerFor_GA_.h"
#include "HandlerFor_GP_JPN.h"
#include "HandlerFor_TE_.h"
#include "HandlerFor_DB_.h"
#include "InterHandler.h"
#include "GameDataPool.h"
#include "StatisticsCollector.h"
#include "ServiceError.h"
#include "SocketSystem.h"
#include "Script.h"
#include "TraceLog.h"
#include "DBConnection.h"
#include "CommonDataPool.h"
#include "Zone.h"
#include "WorkThread.h"
#include "Message.h"
#include "InternalMsgDesign.h"
#include "TCPDispatcher.h"
#include "Exception.h"

namespace nsl {
extern char configpath[256];
}

HandlerFor_GA_* handlerFor_GA_;
HandlerFor_GP_* handlerFor_GP_;
HandlerFor_TE_* handlerFor_TE_;
InterHandler* interHandler;
HandlerFor_DB_* handlerFor_DB_;
GameDataPool* gameDataPool_[5];
nsl::DBConnection* pGameDbConnection;
nsl::DBConnection* pAuctionDbConnection;

App::App()
    : nsl::LinuxService()
{
}

App::~App()
{
}

bool App::load_script(char* service_identify)
{
    puts("Called load_script");
    strcpy(nsl::configpath, m_szConfigFileName);
    printf("[!] Server environment(%s) script loading : %s\n",
           m_szConfigFileName, nsl::configpath);
    bool bVar1 = nsl::G_Script()->load(nsl::configpath);
    // fail-first: movzbl/xor/test/je success (match ORIG App::load_script)
    if (!bVar1)
    {
        printf("script load fail : %s\n", m_szConfigFileName);
        return false;
    }
    bVar1 = nsl::G_Script()->get_sections();
    if (!bVar1)
    {
        exit(1);
    }
    // direct ! on return value (no store to local before xor)
    if (!initAuctionString(nsl::G_Script()->findCharValue(1, 0xe)))
    {
        puts("init auction string fail");
        exit(1);
    }
    return true;
}

void App::readConfig(char* service_identify)
{
    puts("Called readConfig");
    bool bVar1 = load_script(service_identify);
    if (!bVar1)
    {
        puts("ERROR readConfig()");
        exit(1);
    }
    puts("OK readConfig()");
}

void App::prepareRun(char* Service_identify)
{
    puts("Called prepareRun");
    try
    {
        nsl::TSocketSystem<nsl::SocketSystemLinux>::startup();
        SetAuctionServiceErrorStr();
        nsl::pApp = new nsl::ServiceFactory(
            Service_identify, m_szConfigFileName);
        handlerFor_GA_ = new HandlerFor_GA_;
        handlerFor_GP_ = new HandlerFor_GP_;
        handlerFor_TE_ = new HandlerFor_TE_;
        handlerFor_DB_ = new HandlerFor_DB_;
        nsl::pApp->super_IHandlers.setNetWorkHandler(
            0, handlerFor_GA_);
        nsl::pApp->super_IHandlers.setNetWorkHandler(
            0x12, handlerFor_GP_);
        nsl::pApp->super_IHandlers.setTimeHandler(
            0, handlerFor_TE_);
        nsl::pApp->super_IHandlers.setTimeHandleNum(1);
        // Evaluate findIntValue before findCharValue (arg order / ORIG esi/ebx)
        int iVar3 = nsl::G_Script()->findIntValue(0, 4);
        char* pcVar4 = nsl::G_Script()->findCharValue(0, 1);
        if (!G_StatisticsCollector()->SetLogFileName(pcVar4, iVar3))
        {
            iVar3 = nsl::G_Script()->findIntValue(0, 4);
            pcVar4 = nsl::G_Script()->findCharValue(0, 1);
            nsl::G_TraceLog()->sysLog(
                7, "G_StatisticsCollector()->SetLogFileName(%s, %d)",
                pcVar4, iVar3);
            exit(1);
        }
        // ORIG: local_24 = limit(1), i at separate local
        int local_24 = 1;
        for (int i = 0; i < local_24; i = i + 1)
        {
            gameDataPool_[i] = new GameDataPool;
            nsl::pApp->super_DataPools.setCommonDataPool(
                i, gameDataPool_[i]);
        }
        pGameDbConnection = new nsl::DBConnection;
        pAuctionDbConnection = new nsl::DBConnection;
        pGameDbConnection->init();
        pAuctionDbConnection->init();
        nsl::pApp->super_DBConnections.setDBConnection(0, pGameDbConnection);
        nsl::pApp->super_DBConnections.setDBConnection(1, pAuctionDbConnection);
        nsl::pApp->super_IHandlers.setDBHandler(
            0, handlerFor_DB_);
        nsl::pApp->super_IHandlers.setDBHandler(
            1, handlerFor_DB_);
        interHandler = new InterHandler;
        nsl::pApp->super_IHandlers.setInterHandler(
            0, interHandler);
        nsl::pApp->super_IHandlers.setInterHandler(
            1, interHandler);
    }
    catch (nsl::Exception& e)
    {
        if (e.getFunctionName() != NULL)
        {
            printf("Main Exception : %d %s at %d in %s\n",
                   e.getErrorCode(), e.getErrorMsg(), e.getLine(),
                   e.getFunctionName());
        }
        else
        {
            printf("Main Exception : %d %s\n",
                   e.getErrorCode(), e.getErrorMsg());
        }
    }
}

void App::run()
{
    puts("Called run");
    static bool onlyOnce = true;
    if (nsl::pApp->startup() < 0)
    {
        puts("Server Init Error");
    }
    else
    {
        nsl::pApp->super_Dispatchers.getTCPDispatcher()->SetMaxCategory(0x19);
        nsl::IArea* pArea = G_Zone()->mArea[0];
        (void)pArea;
        while (isTerminated_ != true)
        {
            // ORIG expands (IsStopReceived() && onlyOnce) to 0/1 then tests
            if ((IsStopReceived()) && (onlyOnce != false))
            {
                onlyOnce = false;
                INTERNALMSG_SERVICE_UNAVAILABLE internalMsgServiceUnavailable;
                internalMsgServiceUnavailable.reason = 3;
                nsl::Message* pNewMsg =
                    nsl::pApp->super_DataPools
                        .getCommonDataPool(nsl::tlsThreadId)
                        ->createMessage(1);
                nsl::CMsgCell* pNewCell = pNewMsg->getCellFromMessage();
                *pNewCell << &internalMsgServiceUnavailable;
                // ORIG: mov pNewMsg->%ebx then getCell/GetInternalMsg/getWorkThread;
                // PushTransaction(pNewMsg) uses %ebx (no separate pMessage local)
                nsl::pApp->super_Threads
                    .getWorkThread(pNewMsg->getCellFromMessage()
                                       ->GetInternalMsg()->workIndex)
                    ->PushTransaction(pNewMsg);
            }
            nsl::TSystem<nsl::LinuxSystem>::sleep(1000);
        }
        puts("Server Terminated");
        nsl::G_TraceLog()->sysLog(7, "Server Terminated\n");
    }
}

void App::finishRun()
{
    puts("Called finishRun");
}

void App::stop()
{
    puts("Called stop");
    if (nsl::pApp != NULL)
    {
        nsl::pApp->shutdown();
    }
    // ORIG：delete pApp（含空检查 + dtor + operator delete，pApp 一次载入 %ebx）
    delete nsl::pApp;
}

void App::onStop()
{
    puts("Called onStop");
    this->stop();
}

void App::onPause()
{
    puts("Called onPause");
}

void App::onContinue()
{
    puts("Called onContinue");
}

int main(int argc, char** argv)
{
    puts("_S_MOD_DB_RECONNECT_FOR_AUCTION_TAIWAN_");
    puts("_S_MOD_USE_MYSQL_OPT_RECONNECT_");
    try
    {
        std::string gService_name(argv[3]);
        // split remove_if from string range-ctor so SaIc is AFTER remove_if
        // (ORIG call order: bind2nd, end, begin, remove_if, Sa, begin, string ctor)
        std::string::iterator new_end = std::remove_if(
            gService_name.begin(), gService_name.end(),
            std::bind2nd(std::equal_to<char>(), ' '));
        gService_name = std::string(gService_name.begin(), new_end);
        // single App* local (ORIG -0x20 only; no separate LinuxService* copy)
        App* pApp = new App;
        pApp->processCommandLine(
            argc, argv, (char*)gService_name.c_str(), true);
        pApp->main((char*)gService_name.c_str());
    }
    catch (nsl::Exception& e)
    {
        if (e.getFunctionName() != NULL)
        {
            printf("Main Exception : %d %s at %d in %s\n",
                   e.getErrorCode(), e.getErrorMsg(), e.getLine(),
                   e.getFunctionName());
        }
        else
        {
            printf("Main Exception : %d %s\n",
                   e.getErrorCode(), e.getErrorMsg());
        }
    }
    return 0;
}
