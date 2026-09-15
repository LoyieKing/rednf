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

// ---------------------------------------------------------------------------
// 自由函数 get_rand_int —— ORIG df_monitor_r 0x0809bf18 定义于本 TU：
//   .text 中紧邻 comp_by_time(0x0809bf04)，其后即本文件的 CTask_ChristmasEvent
//   （0x0809c00e 起），该 .o 的 GI 键 = _GLOBAL__I__Z12get_rand_inti(0x0809c4a3)
//   ⇒ 本函数是该 TU 的首个强实体，必须定义在下方 CTask_ChristmasEvent 之前。
//   与 ORIG 的 DNFFLib::get_rand_int(0x0805f17e)/df_game_r 0x086b1b87 逐指令一致
//   （73 条），同源副本；返回类型 int（依据见 DNFApplication.h:255 起注释），
//   与本 TU 内 DNFFunctionLib.h:34 的声明一致。
// ---------------------------------------------------------------------------
__attribute__((target("arch=i586")))
int get_rand_int(int range)
{
    if (range < 0)
    {
        return 0;
    }
    switch (range)
    {
    case 0:
        return rand();
    default:
        break;
    }
    int r = rand();
    if (r > range)
    {
        return rand() % range;
    }
    r *= 0x41c64e6d;
    r += 0x3039;
    unsigned int result = (r / 65536) & 0x7ff;
    r *= 0x41c64e6d;
    r += 0x3039;
    result <<= 10;
    result ^= (r / 65536) & 0x3ff;
    r *= 0x41c64e6d;
    r += 0x3039;
    result <<= 10;
    result ^= (r / 65536) & 0x3ff;
    if (result > (unsigned int)range)
    {
        return result % range;
    }
    return result;
}

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

    // signed 比较（ORIG setg）⇒ 与 getEventStartTime() 的 unsigned 返回需显式 int 化；
    // 余下 ndiff=6：比较处多一条 `mov %eax,%edx` 中转 + 循环退出少一条 `jmp`。
    // 已试：`(time_t)`/无 cast、返回类型改 time_t（6→8）、goto 形式循环（181 条更差）。
    if ((int)now < (int)getEventStartTime())
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
        do
        {
            eventHour = DecideEventTime();
            if (!(eventHour < nowTm.tm_hour + 1))
            {
                break;
            }
            n++;
        } while (n < 3);
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
    DNF_LOG_SCOPE_LINE(0x96, "./log/GameServer", "Next X_Mas Event Time! (%s)",
        ctime(&result));
    return (unsigned int)result;
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0 AE | CTask_ChristmasEvent::_DoExecute（原 DNF-MON-DIFF-0127 已消除）
void CTask_ChristmasEvent::_DoExecute()
{
    Packet_Monitor_ServerEvent_Start pkt;
    pkt.m_eventCode = 1;
    pkt.m_param1 = 0x14;
    pkt.m_param2 = 0xe10;
    CServerHandler* h = ((CApplication*)CApplicationInstance())->Get_ServerHandler();
    h->SendAllToGameServer((char*)&pkt, 0x12);
    int t = (int)MakeEventStartTick(1);
    if ((int)getEventEndTime() < t)
    {
        DNF_LOG_SCOPE_LINE(0xc3, "./log/GameServer", "End X_Mas Event!");
    }
    else
    {
        ((CApplication*)CApplicationInstance())->GetTaskScheduler()->AddTask(
            new CTask_ChristmasEvent(t, 0));
        DNF_LOG_SCOPE_LINE(200, "./log/GameServer", "Start X_Mas Event!");
    }
}

CTask_ChristmasEvent::CTask_ChristmasEvent(unsigned int tick, unsigned int flag) throw()
    : CTaskScheduler::CTask(tick, flag)
{
}

CTask_ChristmasEvent::~CTask_ChristmasEvent() {}
