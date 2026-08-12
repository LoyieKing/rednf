// df_monitor_r — TaskScheduler（从 MonitorTypes/App/Table 拆分）
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

#include "TaskScheduler.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

CTaskScheduler::CTask::~CTask() {}

CTaskScheduler::CTask::CTask(unsigned int tick, unsigned int flag)
{
    m_tick = tick;
    m_flag = flag;
    m_taskID = 0;
}

CTaskScheduler::CTaskScheduler() {}

CTaskScheduler::~CTaskScheduler() {}

int CTaskScheduler::AddTask(CTask* task)
{
    CTaskProxy proxy(task);
    m_queue.push(proxy);
    return 1;
}

void CTaskScheduler::ProcessTask(unsigned int tick)
{
    if (m_queue.empty())
        return;
    CTaskProxy proxy = m_queue.top();
    if (proxy.GetDeliveryTime() <= tick)
    {
        m_queue.pop();
        printf("m_queTask pop size(%d)\n", (unsigned int)m_queue.size());
        proxy.DoExcute();
        proxy.Destroy();
    }
    else
        return;
}

int CTaskScheduler::RemoveTask(unsigned int taskID)
{
    return 1;
}

int CTaskScheduler::RemoveTask(CTask* task)
{
    RemoveTask(task->GetTaskID());
    return 1;
}
