// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CEventCreateDnf/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include "CEventCreateDnf/CEventCreateDnf.inc"
    : m_state(State::kWaiting), m_startHour(0), m_endHour(0),
      m_reloadInterval(0), m_reserved(0)
{
    // ORIG C1 0x08163aa4：只清 +0xc/+0x10/+0x11/+0x12/+0x14；+0x16 m_rate 不初始化。
}

#include "CEventCreateDnf/dtor_CEventCreateDnf.inc"

#include "CEventCreateDnf/setState.inc"
{
    m_state = state;
}

#include "CEventCreateDnf/GetRate.inc"
{
    return m_rate;
}

#include "CEventCreateDnf/IsRewardCondition.inc"
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

#include "CEventCreateDnf/SetEventInfo.inc"
{
    // b28d-D42 verdict=AE(IDENTICAL) ORIG 0x081640d4 orig/ours=96/93。
    // 修法：P227 全表达式 cMyTrace 临时链（3 处）+ P272 两臂发射序反写
    // （kIng 0x95 在前、kReward 0x9a 在后，判据 hour>=m_startHour && <m_endHour）。
    // ORIG 0x081640d4：GetState()==2 且 hour 落在 [m_endHour, m_startHour) →
    // 只打 trace（保持 kReward），不更新 rate。
    if (GetState() == State::kReward && hour < m_startHour && hour >= m_endHour)
    {
        // b28d-D42 P227：ORIG 0x081640d4 的 cMyTrace 为全表达式临时链
        // （varargs 在 ctor 前右起求值并保活 callee-saved）。
        cMyTrace("CEventCreateDnf::SetEventInfo(int, int, int)", 0x8e, 0)(
            "[Taiwan, CreateDnfEvent] State:kReward rate:%d", (int)m_rate);
        return;
    }
    // b28d-D42 P272：ORIG 0x081640d4 两臂发射序为 kIng(0x95) 在前、
    // kReward(0x9a) 在后且判据 `jle`（正极性德摩根形）⇒ 反写条件。
    if (hour >= m_startHour && hour < m_endHour)
    {
        setState(State::kIng);
        cMyTrace("CEventCreateDnf::SetEventInfo(int, int, int)", 0x95, 0)(
            "[Taiwan, CreateDnfEvent] State:kIng rate:%d", rate);
    }
    else
    {
        setState(State::kReward);
        cMyTrace("CEventCreateDnf::SetEventInfo(int, int, int)", 0x9a, 0)(
            "[Taiwan, CreateDnfEvent] State:kReward rate:%d", rate);
    }

    m_rate = static_cast<unsigned short>(rate);
    if (reloadInterval != 0)
        m_reserved = static_cast<unsigned char>(reloadInterval);
}

#include "CEventCreateDnf/StartEvent.inc"
{
    SetEventFlag(true);
    setState(State::kWaiting);
    RegistStartTimer(true);
    // b28d-D42 verdict=AE(IDENTICAL) ORIG 0x08163b36 orig/ours=46/46。
    // 修法 P227/P87：ORIG 在 cMyTrace ctor 之前右起求 varargs
    // 并保活 callee-saved（edi=reloadInterval、esi=endHour、ebx=startHour），
    // 具名 trace 两语句形使 4.4 退化为栈槽 ⇒ 全表达式临时链复现。
    cMyTrace("CEventCreateDnf::StartEvent()", 0x1d, 0)(
        "[Taiwan, CreateDnfEvent] Start! startTime:%2dh, endTime:%2dh, reLoadInterval:%d",
        (int)m_startHour, (int)m_endHour, (int)m_reloadInterval);
}

#include "CEventCreateDnf/StartEvent_1.inc"
{
    // b28d-D42 + round1 R19：ORIG 0x08163be0 逐字节读参（`lea 0xc(%ebp),%eax;
    // mov %eax,-0xc(%ebp); movzbl (%eax),%edx; movzbl 0x1(%eax),%edx;
    // movzbl 0x2(%eax),%eax`）⇒ P244 具名指针槽 + P311 成员视图（`p[i]` 下标形
    // 被 4.4 折成 `add $0xN,%eax` 两条，成员视图才出 disp 单条）。
    struct WordParamBytes
    {
        unsigned char m_b0;  // +0x00 → m_startHour
        unsigned char m_b1;  // +0x01 → m_endHour
        unsigned char m_b2;  // +0x02 → m_reloadInterval（分钟数）
        unsigned char m_b3;  // +0x03（ORIG 未读）
    };
    const WordParamBytes* p = reinterpret_cast<const WordParamBytes*>(&param);
    m_startHour = p->m_b0;
    m_endHour = p->m_b1;
    m_reloadInterval = static_cast<unsigned short>(p->m_b2 * 60);
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

#include "CEventCreateDnf/EndEvent.inc"
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

#include "CEventCreateDnf/RegistStartTimer.inc"
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

    // b28d-D42 verdict=NEAR [N7] ORIG 0x08163e7c orig/ours=124/124。
    // 修法：P272 臂序反写（`startSec >= now` 使 kWaiting 块直落）+ P227 trace
    // 临时链；残余仅间接 call 目标寄存器 %edx↔%eax（N7，4.4.7 恒 %eax）。
    // P272：ORIG 0x08163fc2 为 `jl <kIng 块>` + kWaiting 块直落
    // （else-first）⇒ 条件取反成 `startSec >= now` 复现臂序。
    if (startSec >= now)
    {
        // 起始时刻未到：setState(kWaiting) + 注册到 startSec 的定时器。
        setState(State::kWaiting);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0xae,
                                    static_cast<int>(startSec - now), 0, 0);
        cMyTrace("CEventCreateDnf::RegistStartTimer(bool)", 0x6e, 0)(
            "[Taiwan, CreateDnfEvent] next db load time:%d",
            static_cast<int>(startSec));
    }
    else
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
        cMyTrace("CEventCreateDnf::RegistStartTimer(bool)", 0x7e, 0)(
            "[Taiwan, CreateDnfEvent] next db load time:%d", next);
    }
}

#include "CEventCreateDnf/RegistNextTimer.inc"
{
    // b28d-D42 verdict=DIFF [N7+N82] ORIG 0x08163d4c orig/ours=74/76 ndiff=33。
    // 残差：①间接 call 目标 %edx↔%eax（N7）；②tm 字段槽基 -0x44/-0x4c 平移 4
    // 与部分指令序（P112/N82 跨版本槽分配）。语义已对齐（P28 赋值序已按 ORIG）。
    // LinkE2 按 ORIG T 0x08163d4c sz0x12f 逐指令转写：
    // !IsEventing(0)（vtable 槽调用，虚函数 IsEventing）→ 直接返回。
    if (!IsEventing(0))
        return;

    // curSec = CSystemTime::getCurSec()（全局 s_systemTime_ @0x941f714，ORIG 0x080cbc9e）
    std::time_t curSec = static_cast<std::time_t>(GlobalData::s_systemTime_.getCurSec());
    std::tm local;
    localtime_r(&curSec, &local);

    // tm_sec=0 / tm_min=0 / tm_hour=m_endHour（this+0x11 byte，ORIG movzbl 0x11）
    // b28d-D42 P28：ORIG 0x08163d4c 的赋值序为 tm_hour 先（-0x44）、再 tm_min
    // （-0x48）、tm_sec（-0x4c）。
    local.tm_hour = m_endHour;
    local.tm_min = 0;
    local.tm_sec = 0;
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

    // b28d-D42 + round1 R19：ORIG 0x08163e20 无 next 具名槽——`movzwl 0x12(%eax);
    // movzwl %ax,%ebx` 先物化 reloadInterval，ctor 前 `call getCurSec; add %eax,%ebx`
    // 跨 ctor 保活（N80/P0）⇒ 删具名 next、内联回 vararg 位（P227 全表达式临时链）。
    cMyTrace("CEventCreateDnf::RegistNextTimer()", 0x55, 0)(
        "[Taiwan, CreateDnfEvent] next db load time:%d",
        static_cast<int>(m_reloadInterval) + GlobalData::s_systemTime_.getCurSec());
}

#include "CEventCreateDnf/SendReward.inc"
{
    // b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0816426c orig/ours=177/179 ndiff=149。
    // 残差为帧/槽号平移（局部大缓冲 szTitle/szContents 槽基差，P112/N82）与
    // vector/pair STL 内联差（N89）；语义按报告逐条对齐。
    std::vector<std::pair<int, int> > items;                     // ORIG local_30

    // ORIG 0x0816428d：ARAD::Singleton<CreateDnfEventSciprtServer>::Get()；
    // lea 0x4(%eax) → Server @+0x04 的 CreateDnfEventSciprt 基类子对象
    // （b28c-32 已把原成员 m_script 改为首个非主基类，故此处走基类引用）。
    // round1 R19：ORIG 无 server 具名槽（`movzbl 0x14(%eax),%eax; movzbl %al,%ebx;
    // call Get; lea 0x4(%eax),%edx`）⇒ P0/P183 全表达式内联，删具名指针。
    static_cast<CreateDnfEventSciprt&>(*ARAD::Singleton<CreateDnfEventSciprtServer>::Get())
        .GetItemInfo(items, m_reserved);   // ORIG 第二参 = this+0x14 byte

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
    // round1 R19：①ORIG 每个 put_* 前重新 `lea -0x58(%ebp),%eax`（P183 地址重物化）
    // ⇒ 删具名 `pkt` 引用；②循环上界 ORIG 在循环外算一次（`mov %eax,-0x1c`），
    // i 先声明（`movl $0x0,-0x20`）⇒ 逗号声明 i/count 并缓存 size()。
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, 0x258);
    ((InterfacePacketBuf*)&packet)->put_byte(1);
    ((InterfacePacketBuf*)&packet)->put_byte((int)items.size());
    for (unsigned int i = 0, count = items.size(); i < count; ++i)
    {
        ((InterfacePacketBuf*)&packet)->put_int(items[i].first);
        ((InterfacePacketBuf*)&packet)->put_byte(items[i].second);   // ORIG put_byte（Ghidra 0x08164480 实证）
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    user->Send(packet);
}
