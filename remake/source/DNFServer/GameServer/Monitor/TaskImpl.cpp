// df_monitor_r — TaskImpl（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
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

#include "TaskImpl.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFServerHandler.h"
#include "DNFApplication.h"
#include "DNFPacketTranslater.h"

int CTask_ChristmasEvent::DecideEventTime()
{
    int hours[25] = {
        0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
        23, 24, 1, 2, 3, 4, 5
    };
    int weight[25] = {
        0, 1449, 2898, 4348, 7246, 10145, 13043, 17391, 23188, 28985,
        34782, 42029, 49275, 56521, 63768, 69565, 75362, 81159, 85507,
        89855, 92753, 95652, 97101, 98550, 100000
    };
    int result = 0;
    int r = (int)get_rand_int(100000);
    int i = 1;
    while (i <= 24)
    {
        if (r < weight[i] && weight[i - 1] < r)
        {
            result = hours[i];
            break;
        }
        i++;
    }
    return result;
}

unsigned int CTask_ChristmasEvent::getEventStartTime() { return 0x47698650; }

unsigned int CTask_ChristmasEvent::getEventEndTime() { return 0x47726c70; }

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0128 | monitor | 与ORIG差异=DIFF | CTask_ChristmasEvent::MakeEventStartTick | 详见 function_reports/monitor/_ZN20CTask_ChristmasEvent18MakeEventStartTickEi.md
unsigned int CTask_ChristmasEvent::MakeEventStartTick(int param_1)
{
    int eventHour = DecideEventTime();
    time_t now = time(0);
    tm nowTm = *localtime(&now);

    if (now < (time_t)getEventStartTime())
    {
        tm t = nowTm;
        t.tm_hour = 0;
        t.tm_min = 0;
        t.tm_sec = 0;
        time_t midnight = mktime(&t);
        param_1 = (int)(((time_t)getEventStartTime() - midnight) / 86400);
    }
    if (param_1 == 0 && eventHour < nowTm.tm_hour + 1)
    {
        int n = 0;
        while (n < 3 && (eventHour = DecideEventTime(), eventHour < nowTm.tm_hour + 1))
        {
            n++;
        }
        if (n == 3)
        {
            eventHour = (nowTm.tm_hour + eventHour + 1) % 25;
        }
    }
    tm t2 = nowTm;
    t2.tm_mday = nowTm.tm_mday + param_1;
    t2.tm_hour = eventHour - 1;
    t2.tm_min = 0;
    t2.tm_sec = 0;
    time_t result = mktime(&t2);
    char* s = ctime(&result);
    DNF_LOG_SCOPE_LINE(0x96, "./log/GameServer", "Next X_Mas Event Time! (%s)", s);
    return (unsigned int)result;
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0127 | monitor | 与ORIG差异=DIFF | CTask_ChristmasEvent::_DoExecute | 详见 function_reports/monitor/_ZN20CTask_ChristmasEvent10_DoExecuteEv.md
void CTask_ChristmasEvent::_DoExecute()
{
    Packet_Monitor_ServerEvent_Start pkt;
    pkt.m_eventCode = 1;
    pkt.m_param1 = 0x14;
    pkt.m_param2 = 0xe10;
    ((CApplication*)CApplicationInstance())->Get_ServerHandler()->SendAllToGameServer(
        (char*)&pkt, 0x12);
    unsigned int t = MakeEventStartTick(1);
    if ((int)getEventEndTime() < (int)t)
    {
        DNF_LOG_SCOPE_LINE(0xc3, "./log/GameServer", "End X_Mas Event!");
    }
    else
    {
        CTask_ChristmasEvent* task = new CTask_ChristmasEvent(t, 0);
        ((CApplication*)CApplicationInstance())->GetTaskScheduler()->AddTask(task);
        DNF_LOG_SCOPE_LINE(200, "./log/GameServer", "Start X_Mas Event!");
    }
}

CTask_ChristmasEvent::CTask_ChristmasEvent(unsigned int tick, unsigned int flag)
    : CTaskScheduler::CTask(tick, flag)
{
}

CTask_ChristmasEvent::~CTask_ChristmasEvent() {}
