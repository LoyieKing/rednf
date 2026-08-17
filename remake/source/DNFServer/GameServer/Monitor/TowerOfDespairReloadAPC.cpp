// df_monitor_r — TowerOfDespairReloadAPC（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "TowerOfDespairReloadAPC.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFApplication.h"
#include "DNFPacketTranslater.h"
#include "DNFServerHandler.h"

bool TowerOfDespairReloadAPC_Task::returnUpdateMessageFromGameServer = false;

TowerOfDespairReloadAPC_Task::TowerOfDespairReloadAPC_Task(unsigned int a, unsigned int b)
    : CTaskScheduler::CTask(a, b)
{
}

TowerOfDespairReloadAPC_Task::~TowerOfDespairReloadAPC_Task() {}

bool TowerOfDespairReloadAPC_Task::isReturnedMessage()
{
    return returnUpdateMessageFromGameServer;
}

void TowerOfDespairReloadAPC_Task::SendRequest_DoRandomSelectUserAPC()
{
    DNF_LOG_SCOPE_LINE(0x37, "./log/GameServer", "TOD : order to RandomSelect main GameServer\n");
    if (isReturnedMessage())
        return;
    CServerHandler* handler;
    unsigned int first;
    Packet_TOD_DoRandomSelect pkt;
    handler = static_cast<CApplication*>(CApplicationInstance())->Get_ServerHandler();
    first = handler->getfirstLinkedServer();
    DNF_LOG_SCOPE_LINE(0x40, "./log/GameServer", "TOD : main GameServerChannel %u\n", first);
    handler->SendToGameServer((unsigned char)first, &pkt);
}

TowerOfDespairWaitGameServerResponse_Task::TowerOfDespairWaitGameServerResponse_Task(
    unsigned int a, unsigned int b) throw()
    : CTaskScheduler::CTask(a, b)
{
}

TowerOfDespairWaitGameServerResponse_Task::~TowerOfDespairWaitGameServerResponse_Task() {}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0130 | monitor | 与ORIG差异=DIFF | TowerOfDespairWaitGameServerResponse_Task::_DoExecute | 详见 function_reports/monitor/_ZN41TowerOfDespairWaitGameServerResponse_Task10_DoExecuteEv.md
void TowerOfDespairWaitGameServerResponse_Task::_DoExecute()
{
    DNF_LOG_SCOPE_LINE(0x46, "./log/GameServer", "TOD : Waiting main GameServer Response...");
    if (TowerOfDespairReloadAPC_Task::isReturnedMessage() != 1)
    {
        TowerOfDespairReloadAPC_Task::SendRequest_DoRandomSelectUserAPC();
        unsigned int t = (unsigned int)time(0);
        ((CApplication*)CApplicationInstance())->GetTaskScheduler()->AddTask(
            new TowerOfDespairWaitGameServerResponse_Task(t + 0x3c, 0));
    }
}

void TowerOfDespairReloadAPC_Task::_DoExecute()
{
    char buf[0x40];
}
