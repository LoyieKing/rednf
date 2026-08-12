// df_monitor_r — Arad_MomijiEvent（从 MonitorTypes/App/Table 拆分）
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

#include "Arad_MomijiEvent.h"
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
#include "DNFServerHandler.h"
#include "DNFPacketTranslater.h"

namespace momiji_event
{
StartEffectTask::StartEffectTask(unsigned int time, unsigned int flag)
    : CTaskScheduler::CTask(time, flag)
{
    m_flag = 1;  // ORIG：movb $0x1,0x10(%eax)
    EventManager::Get()->SetStartEffectTask(this);
}

StartEffectTask::~StartEffectTask() {}

void StartEffectTask::_DoExecute()
{
    if (m_flag != 0)
    {
        time_t end = time(0) + (time_t)EventManager::Get()->GetDurationTime();
        EndEffectTask* task = new EndEffectTask((unsigned int)end, 0);
        ((CApplication*)CApplicationInstance())->GetTaskScheduler()->AddTask(task);
        tm* t = localtime((time_t*)&end);
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0xb0,"./log/AradOnly", "[Momiji] start event. next endEffect %02dh:%02dm:%02ds",
            t->tm_hour, t->tm_min, t->tm_sec);
        EventManager::Get()->sendApplyEffect((unsigned int)end);
    }
    EventManager::Get()->SetStartEffectTask(0);
}

EndEffectTask::EndEffectTask(unsigned int time, unsigned int flag)
    : CTaskScheduler::CTask(time, flag)
{
    ((RA_S8<16>*)this)->v = 1;
    EventManager::Get()->SetEndEffectTask(this);
}

EndEffectTask::~EndEffectTask() {}

void EndEffectTask::_DoExecute()
{
    if (((RA_S8<16>*)this)->v != 0)
    {
        time_t next = time(0) +
                      (EventManager::Get()->GetIntervalTime() -
                       EventManager::Get()->GetDurationTime());
        StartEffectTask* task = new StartEffectTask((unsigned int)next, 0);
        ((CApplication*)CApplicationInstance())->GetTaskScheduler()->AddTask(task);
        tm* t = localtime((time_t*)&next);
        DNF_LOG_SCOPE_AT(__FUNCTION__, 200,"./log/AradOnly", "[Momiji] start event. next startEffect %02dh:%02dm:%02ds",
            t->tm_hour, t->tm_min, t->tm_sec);
        EventManager::Get()->sendDeleteEffect();
    }
    EventManager::Get()->SetEndEffectTask(0);
}

EventManager::EventManager()
{
    m_startHour = 0;
    m_interval = 0;
    m_duration = 0;
    m_startTask = 0;
    m_endTask = 0;
}

EventManager::~EventManager() {}

EventManager* EventManager::Get()
{
    static EventManager obj;
    return &obj;
}

unsigned int EventManager::GetDurationTime()
{
    return m_duration;
}

unsigned int EventManager::GetIntervalTime()
{
    return m_interval;
}

void EventManager::SetStartEffectTask(StartEffectTask* task)
{
    m_startTask = task;
}

void EventManager::SetEndEffectTask(EndEffectTask* task)
{
    m_endTask = task;
}

void EventManager::sendApplyEffect(unsigned int time)
{
    Packet_Arad_ApplyEffect pkt(
        (int)(unsigned char)((CApplication*)CApplicationInstance())->Get_ServerGroup(), 2, time);
    ((CApplication*)CApplicationInstance())->Get_ServerHandler()->SendAllTcpGameServer(&pkt);
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x97, "./log/AradOnly", "[Momiji] apply effect. (code:%u)", 2);
}

void EventManager::StartEvent(unsigned char startHour, unsigned char interval,
                              unsigned char duration)
{
    if (duration >= interval)
    {
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x4b,"./log/AradOnly", "[Momiji] (durationTime(%d) >= intervalTime(%d))",
            (unsigned int)duration, (unsigned int)interval);
        return;
    }
    if (startHour >= 0x18)
    {
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x51, "./log/AradOnly", "[Momiji] (startTime(%d) >= 24)", (unsigned int)startHour);
        return;
    }
    m_interval = (unsigned int)(interval * 0x3c);
    m_duration = (unsigned int)(duration * 0x3c);
    m_startHour = startHour;
    unsigned char h = startHour;
    time_t now = time(0);
    tm* t = localtime(&now);
    t->tm_hour = startHour;
    t->tm_min = 0;
    t->tm_sec = 0;
    time_t first = mktime(t);
    while ((int)first <= (int)now)
    {
        first = (unsigned int)interval * 0x3c + first;
    }
    StartEffectTask* task = new StartEffectTask((unsigned int)first, 0);
    ((CApplication*)CApplicationInstance())->GetTaskScheduler()->AddTask(task);
    tm* t2 = localtime((time_t*)&first);
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x6e,"./log/AradOnly", "[Momiji] start event. first time %02dh:%02dm:%02ds",
        t2->tm_hour, t2->tm_min, t2->tm_sec);
}

void EventManager::sendDeleteEffect()
{
    char stack_pad[8]; // 对齐 ORIG 中 pkt 的栈槽（-0x22），与 CMyFileLog 临时对象不重叠
    Packet_Arad_DeleteEffect pkt(
        (int)(unsigned char)((CApplication*)CApplicationInstance())->Get_ServerGroup(), 2);
    ((CApplication*)CApplicationInstance())->Get_ServerHandler()->SendAllTcpGameServer(&pkt);
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x9e, "./log/AradOnly", "[Momiji] delete effect. (code:%u)", 2);
    (void)stack_pad;
}

void EventManager::EndEvent()
{
    sendDeleteEffect();
    if (m_startTask != 0)
    {
        ((RA_S8<16>*)m_startTask)->v = 0;
    }
    if (m_endTask != 0)
    {
        ((RA_S8<16>*)m_endTask)->v = 0;
    }
    Init();
}

EventAction::EventAction() {}

void EventAction::onStartAction(EventParam& param)
{
    if (isRunning() != 1)
    {
        m_running = 1;
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x17,"./log/AradOnly", "[Momiji] onStartAction(%d,%d,%d)",
            (unsigned int)(unsigned char)((RA_S8<0>*)&param)->v,
            (unsigned int)(unsigned char)((RA_S8<1>*)&param)->v,
            (unsigned int)(unsigned char)((RA_S8<2>*)&param)->v);
        EventManager::Get()->StartEvent(((RA_U8<0>*)&param)->v,
                                        ((RA_U8<1>*)&param)->v,
                                        ((RA_U8<2>*)&param)->v);
    }
}

void EventAction::onEndAction()
{
    if (isRunning())
    {
        m_running = 0;
        EventManager::Get()->EndEvent();
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x26, "./log/AradOnly", "[Momiji] end event.");
    }
}

void EventManager::Init()
{
    m_interval = 0;
    m_duration = 0;
    m_startHour = 0;
    m_startTask = 0;
    m_endTask = 0;
}
}
