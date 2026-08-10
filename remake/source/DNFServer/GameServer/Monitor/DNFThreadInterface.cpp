// df_monitor_r — DNFThreadInterface（从 MonitorTypes/App/Table 拆分）
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

#include "DNFThreadInterface.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

CThreadInterface::CThreadInterface() {}

CThreadInterface::~CThreadInterface() {}

void CThreadInterface::stop() {}

void CThreadInterface::join()
{
    pthread_join((pthread_t)m_thread, 0);
}

bool CThreadInterface::begin()
{
    int r = pthread_create((pthread_t*)&m_thread, 0, dispatch_proxy, this);
    if (r < 0)
    {
        puts("[ThreadInterface::begin] Can't begin thread");
        return 0;
    }
    return 1;
}

void* CThreadInterface::dispatch_proxy(void* temp) { return 0; }

