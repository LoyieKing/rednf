#include "CEventCreateDnf.h"
#include "GlobalData.h"     // s_systemTime_（LinkE2）
#include "TimerQueue.h"     // G_TimerQueue/InsertTimer/TIMER_MESSAGE（LinkE2）
#include "GameTypes.h"      // cMyTrace（LinkE2）
#include "CreateDnfEventSciprt.h"   // CreateDnfEventSciprtServer::GetItemInfo（StubS3A SendReward）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_.findString
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper 静态邮件接口
#include "CUser.h"                    // CUser::getCurCharacNo/GetServerGroup/Send（完整类型）
#include "PacketGuard.h"
#include "DBSelectCreateDnfEventInfo.h"   // makeRequest（RegistStartTimer ORIG 0x08163fc2）
#include "LogManager.h"                   // logFormat（StartEvent(Word_Param) ORIG 0x08163cb6）


#include <ctime>

CEventCreateDnf::CEventCreateDnf()
    : m_state(State::kWaiting), m_startHour(0), m_endHour(0),
      m_reloadInterval(0), m_reserved(0)
{
    // ORIG C1 0x08163aa4：只清 +0xc/+0x10/+0x11/+0x12/+0x14；+0x16 m_rate 不初始化。
}

CEventCreateDnf::~CEventCreateDnf() {}   // LinkE2：=default 为 C++11，c6root C++98 基线改空体

void CEventCreateDnf::setState(State::T state)
{
    m_state = state;
}

unsigned short CEventCreateDnf::GetRate()
{
    return m_rate;
}

bool CEventCreateDnf::IsRewardCondition()
{
    // ORIG 0x08164092：call GetState() 判 ==2，再比较 16 位 m_rate==100。
    // P56：bool 返回分支形态（cmp/jcc/mov $1/jmp/mov $0 双层汇聚），
    // 直 return 表达式只生成单层汇聚 → 写 if/return。
    // 帧差：ORIG sub $0x18，本工具链 sub $0x4（gcc4.1/4.4 栈槽布局差异，
    // P112/跨版本豁免，语义与指令序列逐条一致）。
    if (GetState() == State::kReward && m_rate == 100)
        return true;
    return false;
}

void CEventCreateDnf::SetEventInfo(int hour, int rate, int reloadInterval)
{
    // ORIG 0x081640d4：GetState()==2 且 hour 落在 [m_endHour, m_startHour) →
    // 只打 trace（保持 kReward），不更新 rate。
    if (GetState() == State::kReward && hour < m_startHour && hour >= m_endHour)
    {
        cMyTrace trace("CEventCreateDnf::SetEventInfo(int, int, int)", 0x8e, 0);
        trace("[Taiwan, CreateDnfEvent] State:kReward rate:%d", (int)m_rate);
        return;
    }
    if (hour < m_startHour || hour >= m_endHour)
    {
        setState(State::kReward);
        cMyTrace trace("CEventCreateDnf::SetEventInfo(int, int, int)", 0x9a, 0);
        trace("[Taiwan, CreateDnfEvent] State:kReward rate:%d", rate);
    }
    else
    {
        setState(State::kIng);
        cMyTrace trace("CEventCreateDnf::SetEventInfo(int, int, int)", 0x95, 0);
        trace("[Taiwan, CreateDnfEvent] State:kIng rate:%d", rate);
    }

    m_rate = static_cast<unsigned short>(rate);
    if (reloadInterval != 0)
        m_reserved = static_cast<unsigned char>(reloadInterval);
}

void CEventCreateDnf::StartEvent()
{
    SetEventFlag(true);
    setState(State::kWaiting);
    RegistStartTimer(true);
    cMyTrace trace("CEventCreateDnf::StartEvent()", 0x1d, 0);
    trace("[Taiwan, CreateDnfEvent] Start! startTime:%2dh, endTime:%2dh, reLoadInterval:%d",
          (int)m_startHour, (int)m_endHour, (int)m_reloadInterval);
}

void CEventCreateDnf::StartEvent(Word_Param param)
{
    m_startHour = static_cast<unsigned char>(param.m_duration & 0xff);
    m_endHour = static_cast<unsigned char>((param.m_duration >> 8) & 0xff);
    m_reloadInterval = static_cast<unsigned short>(
        static_cast<unsigned char>(param.m_field2 & 0xff) * 60);
    if (m_startHour < 24 && m_endHour < 24 &&
        m_reloadInterval > 59 && m_reloadInterval < 3601)
        StartEvent();
    else
    {
        // ORIG 0x08163cb6：LogManager::logFormat + 虚调 EndEvent()
        LogManager::logFormat(
            1, "localtaiwan/Event/EventCreateDnf.cpp",
            __PRETTY_FUNCTION__, 0x31,
            "[Taiwan, CreateDnfEvent] Error! startTime:%2dh, endTime:%2dh, reLoadInterval:%d",
            (int)m_startHour, (int)m_endHour, (int)m_reloadInterval);
        EndEvent();
    }
}

void CEventCreateDnf::EndEvent()
{
    cMyTrace trace("CEventCreateDnf::EndEvent()", 0x38, 0);
    trace("[Taiwan, CreateDnfEvent] End Event.");
    m_startHour = 0;
    m_endHour = 0;
    m_reloadInterval = 0;
    setState(State::kWaiting);
    SetEventFlag(false);
    // ORIG 0x08163cd0：不清 +0x16 m_rate。
}

void CEventCreateDnf::RegistStartTimer(bool immediate)
{
    (void)immediate;
    if (!IsEventing(0))
        return;
    // ORIG 0x08163e7c：SetEventInfo(0,0,0) → 按 startHour 计算今日起始时刻
    SetEventInfo(0, 0, 0);
    const std::time_t now = static_cast<std::time_t>(GlobalData::s_systemTime_.getCurSec());
    std::tm local;
    localtime_r(&now, &local);
    local.tm_hour = m_startHour;
    local.tm_min = 0;
    local.tm_sec = 0;
    const std::time_t startSec = mktime(&local);

    if (startSec < now)
    {
        // 起始时刻已过：DB makeRequest + setState(kIng) +
        // 以 reloadInterval 步长推进到下一个 >=now 的时刻，注册定时器。
        DBSelectCreateDnfEventInfo::makeRequest();
        setState(State::kIng);
        int next = static_cast<int>(m_reloadInterval) + static_cast<int>(startSec);
        while (next < static_cast<int>(now))
            next += static_cast<int>(m_reloadInterval);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0xae,
                                    next - static_cast<int>(now), 0, 0);
        cMyTrace trace("CEventCreateDnf::RegistStartTimer(bool)", 0x7e, 0);
        trace("[Taiwan, CreateDnfEvent] next db load time:%d", next);
    }
    else
    {
        // 起始时刻未到：setState(kWaiting) + 注册到 startSec 的定时器。
        setState(State::kWaiting);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0xae,
                                    static_cast<int>(startSec - now), 0, 0);
        cMyTrace trace("CEventCreateDnf::RegistStartTimer(bool)", 0x6e, 0);
        trace("[Taiwan, CreateDnfEvent] next db load time:%d",
              static_cast<int>(startSec));
    }
}

void CEventCreateDnf::RegistNextTimer()
{
    // LinkE2 按 ORIG T 0x08163d4c sz0x12f 逐指令转写：
    // !IsEventing(0)（vtable 槽调用，虚函数 IsEventing）→ 直接返回。
    if (!IsEventing(0))
        return;

    // curSec = CSystemTime::getCurSec()（全局 s_systemTime_ @0x941f714，ORIG 0x080cbc9e）
    std::time_t curSec = static_cast<std::time_t>(GlobalData::s_systemTime_.getCurSec());
    std::tm local;
    localtime_r(&curSec, &local);

    // tm_sec=0 / tm_min=0 / tm_hour=m_endHour（this+0x11 byte，ORIG movzbl 0x11）
    local.tm_sec = 0;
    local.tm_min = 0;
    local.tm_hour = m_endHour;
    const std::time_t startSec = mktime(&local);

    // startSec >= curSec → setState(kReward=2)（ORIG jl 8163ddd 跳过路径取反）
    if (startSec >= curSec)
        setState(State::kReward);

    // InsertTimer(G_TimerQueue(), OBJ_TYPE 2, 0, TIMER_MESSAGE 0xae,
    //   m_reloadInterval, 0, 0)（ORIG 0x8630e16；0xae 无具名枚举值，cast 忠实；
    //   sec 参数 = this+0x12 reloadInterval）
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0xae,
                                static_cast<unsigned int>(m_reloadInterval), 0, 0);

    // trace("[Taiwan, CreateDnfEvent] next db load time:%d", m_reloadInterval+getCurSec())
    const int next = static_cast<int>(m_reloadInterval) +
                     GlobalData::s_systemTime_.getCurSec();
    cMyTrace trace("CEventCreateDnf::RegistNextTimer()", 0x55, 0);
    trace("[Taiwan, CreateDnfEvent] next db load time:%d", next);
}

void CEventCreateDnf::SendReward(CUser* user)
{
    std::vector<std::pair<int, int> > items;                     // ORIG local_30

    // ORIG 0x0816428d：ARAD::Singleton<CreateDnfEventSciprtServer>::Get()；
    // lea 0x4(%eax) → Server @+0x04 的 CreateDnfEventSciprt 基类子对象
    // （b28c-32 已把原成员 m_script 改为首个非主基类，故此处走基类引用）。
    CreateDnfEventSciprtServer* server =
        ARAD::Singleton<CreateDnfEventSciprtServer>::Get();
    static_cast<CreateDnfEventSciprt&>(*server).GetItemInfo(items, m_reserved);   // ORIG 第二参 = this+0x14 byte

    // ORIG 0x081642a8..0x0816431c：RDARScriptStringManager::findString(4, key, 0) + strncpy
    char szTitle[30];                                            // ORIG -0x4a（0x1d cap）
    strncpy(szTitle,
            g_scriptStringManager_.findString(4, "tw_cb_launching_title_01", 0),
            0x1d);
    char szContents[256];                                        // ORIG -0x158（0xff cap）
    strncpy(szContents,
            g_scriptStringManager_.findString(4, "game_server_msg_81", 0),
            0xff);

    // ORIG 0x08164321：循环构造 10 个 Inven_Item（0x3d 步长）→ 栈数组等价
    Inven_Item mailItems[10];
    // ORIG 0x08164348：MakeSystemMultiMailPostal(items, mailItems, 10)
    WongWork::CMailBoxHelper::MakeSystemMultiMailPostal(items, mailItems, 10);

    // ORIG 0x0816439a：ReqDBSendNewSystemMultiMail(title, items, size, 0, characNo,
    //                  contents, strlen, 0, serverGroup, 0)
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
        szTitle, mailItems, (unsigned int)items.size(), 0,
        (unsigned int)user->getCurCharacNo(), szContents,
        (int)strlen(szContents), 0, user->GetServerGroup(), false);

    // ORIG 0x081643e1..0x081644c3：PacketGuard → put_header(1, 0x258) →
    // byte(1) → byte(size) → [int(first), byte(second)]*size → finalize(true) → Send
    PacketGuard packet;
    InterfacePacketBuf& pkt = *(InterfacePacketBuf*)&packet;     // 项目既有模式
    pkt.put_header(1, 0x258);
    pkt.put_byte(1);
    pkt.put_byte((int)items.size());
    for (unsigned int i = 0; i < items.size(); ++i)
    {
        pkt.put_int(items[i].first);
        pkt.put_byte(items[i].second);                            // ORIG put_byte（Ghidra 0x08164480 实证）
    }
    pkt.finalize(true);
    user->Send(packet);
}
