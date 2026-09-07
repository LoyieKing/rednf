// ============================================================================
// W9_EventSmall.cpp —— Wave9 事件小类批（5 类）真实实现。
// 声明见 W9_EventSmall.h。逐函数依据 docs/class_func_reports/ 对应报告 +
// ORIG objdump（df_game_r）转写；无独立报告的小函数（Inc*/Get*/D0 等）
// 由 objdump 直接核对，地址注释在各函数头。
//
//   CTrcOnOffEvent                    报告目录 CTrcOnOffEvent/
//   CFatigueBuffEvent                 无报告，ORIG 0x0811bbxx.. 反汇编实证
//   CWeekendBonusEvent                无报告，ORIG 0x0811755x.. 反汇编实证
//   CAutoMarketConditionsControlEvent 报告目录 CAutoMarketConditionsControlEvent/
//   CCharacBloodDungeon               报告目录 CCharacBloodDungeon/ + 0x85bfexx
// ============================================================================

#include <ctime>
#include "W9_EventSmall.h"
#include "GameTypes.h"       // cMyTrace
#include "GlobalData.h"
#include "CSystemTime.h"
#include "StreamPool.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"
#include "CUser.h"

// getDailyInitializeTime：定义 GameUtils.cpp（DB_LoadEtc.cpp 已声明）。
int getDailyInitializeTime(int hour, int min, int sec);

// ============================ DB 依赖 ============================
bool DB_LoadAutoMarketConditionsControl::makeRequest(bool flag)
{
    // ORIG T 0x08433ada _ZN34DB_LoadAutoMarketConditionsControl11makeRequestEb
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7c43);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x15a);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(flag);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

// ============================ 事件基类模板 ============================
// 四个小类的 Start/End 全部为「SetEventFlag + cMyTrace(function 名, len, 0) +
// trace(msg)」同构（各报告反编译一致）。构造/析构只置 vptr（编译器生成），
// IsEventing 为基类透传。

// ---- CTrcOnOffEvent ----
CTrcOnOffEvent::CTrcOnOffEvent() {}          // ORIG 0x08285a44
CTrcOnOffEvent::~CTrcOnOffEvent() {}         // ORIG 0x08285a60（D0 编译器生成）

void CTrcOnOffEvent::StartEvent()            // ORIG 0x08285aae
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x13, 0);
    trace("Trc On Off Event Start!");
}

void CTrcOnOffEvent::EndEvent()              // ORIG 0x08285b52
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x1f, 0);
    trace("Trc On Off Event Event End!");
}

void CTrcOnOffEvent::StartEvent(Word_Param)  // ORIG 0x08285b00
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x19, 0);
    trace("Trc On Off Event Event Start!");
}

bool CTrcOnOffEvent::IsEventing(CUser* user) const  // ORIG 0x08285ba4：透传基类
{
    // ORIG 0x08285ba4：第二参数恒传 0（NULL），不转发本函数形参。返回仍等于
    // 基类结果，但 4.1 走「取反+反极性岛」：`call; xor $1; test; je +岛mov$1;
    // mov $0` ⇒ 技巧 1/22 register bool asm("al") 绑 !call 后反向 if 复现。
    register bool t __asm__("al");
    t = !CEventBase::IsEventing(static_cast<CUser*>(0));
    if (t)
        return false;
    return true;
}

// ---- CFatigueBuffEvent ----
CFatigueBuffEvent::CFatigueBuffEvent() {}    // ORIG 0x0811bbea
CFatigueBuffEvent::~CFatigueBuffEvent() {}   // ORIG 0x0811bc06

void CFatigueBuffEvent::StartEvent()         // ORIG 0x0811bc54
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x11, 0);
    trace("CFatigueBuffEvent Event Start!");
}

void CFatigueBuffEvent::EndEvent()           // ORIG 0x0811bcc0
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x1b, 0);
    trace("CFatigueBuffEvent Event End!");
}

void CFatigueBuffEvent::StartEvent(Word_Param)  // ORIG 0x0811bca6：虚调用 slot2（StartEvent()）
{
    StartEvent();
}

// ---- CWeekendBonusEvent ----
CWeekendBonusEvent::CWeekendBonusEvent() {}  // ORIG 0x08117550
CWeekendBonusEvent::~CWeekendBonusEvent() {} // ORIG 0x0811756c

void CWeekendBonusEvent::StartEvent()        // ORIG 0x081175ba
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x12, 0);
    trace("CWeekendBonusEvent Event Start!");
}

void CWeekendBonusEvent::EndEvent()          // ORIG 0x08117626
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x1c, 0);
    trace("CWeekendBonusEvent Event End!");
}

void CWeekendBonusEvent::StartEvent(Word_Param)  // ORIG 0x0811760c：虚调用 slot2（StartEvent()）
{
    StartEvent();
}

// ---- CAutoMarketConditionsControlEvent ----
CAutoMarketConditionsControlEvent::CAutoMarketConditionsControlEvent() {}  // 0x08109c14
CAutoMarketConditionsControlEvent::~CAutoMarketConditionsControlEvent() {} // 0x08109c30

void CAutoMarketConditionsControlEvent::StartEvent()  // ORIG 0x08109c7e
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x13, 0);
    trace("CAutoMarketConditionsControlEvent Start!");
}

void CAutoMarketConditionsControlEvent::EndEvent()    // ORIG 0x08109cf8
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x1f, 0);
    trace("CAutoMarketConditionsControlEvent End!");
}

void CAutoMarketConditionsControlEvent::StartEvent(Word_Param)  // ORIG 0x08109cd0
{
    SetEventFlag(true);
    DB_LoadAutoMarketConditionsControl::makeRequest(true);
}

// ============================ CCharacBloodDungeon ============================
CCharacBloodDungeon::CCharacBloodDungeon()
{
    // ORIG 0x832af6e：仅调基类 ctor 0x80c8cba + 置 vptr；帧内无字段清零
    // （ORIG 汇编无 memset/逐字段赋值），故与 ORIG 一致不做显式初始化。
}

void CCharacBloodDungeon::_reset()           // ORIG 0x832a2d4
{
    m_point = 0;
    m_weeklyEnterCount = 0;
    m_useGold = 0;
    m_field14 = 0;
    m_enterCount = 0;
    m_field1c = 0;
    m_flag20 = 0;
    m_field24 = 0;
    m_field28 = 0;
}

bool CCharacBloodDungeon::_saveData(CUser* pUser)  // ORIG 0x832a330
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("CharacBloodDungeon.cpp", 0x1e),
                       true);
    guard.operator*()->operator<<(0x197);       // op*(guard)→op<<(int)
    guard.operator*()->operator<<((int)pUser->GetUID());

    SIG_BLOOD_DUNGEON* sig = guard.operator->()->GetInBuffer<SIG_BLOOD_DUNGEON>();
    memset(sig, 0, sizeof(SIG_BLOOD_DUNGEON));   // ORIG memset 0x28
    sig->m_characNo = (int)pUser->getCurCharacNo();
    sig->m_point = m_point;                      // this+0x08 → sig+0x04
    sig->m_weeklyEnterCount = m_weeklyEnterCount;
    sig->m_useGold = m_useGold;
    sig->m_dailyInitTime = getDailyInitializeTime(6, 0, 0);  // 覆写 +0x10
    // ORIG 跳过 this+0x14：sig+0x14←this+0x18、sig+0x18←this+0x1c（与 loadData
    // 镜像一致；原写 this+0x14/0x18 系真错位 bug）。
    sig->m_field14 = m_enterCount;
    sig->m_enterCount = m_field1c;
    sig->m_flag1c = m_flag20;                    // byte 镜像
    sig->m_field20 = m_field24;
    sig->m_field24 = m_field28;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    m_flag20 = 0;                                // ORIG 发送后清标志
    return true;                                 // 返回 1
}

bool CCharacBloodDungeon::loadData(CUser*, char* pData)  // ORIG 0x832a4c6
{
    // pData 即 _saveData 落库的 SIG_BLOOD_DUNGEON 记录（ORIG 0x832a4c6 汇编：
    // 8 条 movl，逐条 sig+0x04/08/0c/10/14/18/20/24 → this+08/0c/10/14/18/1c/24/28）。
    // sig+0x00 characNo 不读取；sig+0x1c（m_flag1c byte）不读取，故 this+0x20
    // m_flag20 保持原值 —— 与 getData 非严格互逆，ORIG 本身即如此。
    const SIG_BLOOD_DUNGEON* sig = reinterpret_cast<const SIG_BLOOD_DUNGEON*>(pData);
    m_point             = sig->m_point;              // sig+0x04（int）
    m_weeklyEnterCount  = sig->m_weeklyEnterCount;   // sig+0x08（int）
    m_useGold           = sig->m_useGold;            // sig+0x0c（int）
    m_field14           = sig->m_dailyInitTime;      // sig+0x10（int）
    m_enterCount        = sig->m_field14;            // sig+0x14（int）
    m_field1c           = sig->m_enterCount;         // sig+0x18（int）
    m_field24           = sig->m_field20;            // sig+0x20（int）
    m_field28           = sig->m_field24;            // sig+0x24（int）
    return true;
}

void CCharacBloodDungeon::getData(char* pData) const  // ORIG 0x832a53a
{
    // loadData 的反向写出（ORIG 0x832a53a：8 条 movl + 1 条 movzbl/movb）。
    SIG_BLOOD_DUNGEON* sig = reinterpret_cast<SIG_BLOOD_DUNGEON*>(pData);
    sig->m_point             = m_point;                      // this+0x08 → sig+0x04
    sig->m_weeklyEnterCount  = m_weeklyEnterCount;           // this+0x0c → sig+0x08
    sig->m_useGold           = m_useGold;                    // this+0x10 → sig+0x0c
    sig->m_dailyInitTime     = m_field14;                    // this+0x14 → sig+0x10
    sig->m_field14           = m_enterCount;                 // this+0x18 → sig+0x14
    sig->m_enterCount        = m_field1c;                    // this+0x1c → sig+0x18
    sig->m_flag1c            = m_flag20;                     // this+0x20 → sig+0x1c（byte）
    sig->m_field20           = m_field24;                    // this+0x24 → sig+0x20
    sig->m_field24           = m_field28;                    // this+0x28 → sig+0x24
}

void CCharacBloodDungeon::ResetDaily()       // ORIG 0x832a5b6
{
    m_enterCount = 0;

    time_t curSec =
        static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    struct tm tmBuf;
    localtime_r(&curSec, &tmBuf);
    if (tmBuf.tm_wday == 1)                  // 周一：weekly 区清零
    {
        m_point = 0;
        m_weeklyEnterCount = 0;
        m_useGold = 0;
        m_flag20 = 0;
    }
    // ORIG 尾部对 localtime 结果的 tm_hour/min/sec 归零是死写入，
    // 未影响任何持久状态，忠实省略。
}

void CCharacBloodDungeon::IncPoint(int delta)        // ORIG 0x85bfed2 (W)
{
    charac_expand::CData::alter();
    m_point += delta;
}

void CCharacBloodDungeon::IncUseGold(int gold)       // ORIG 0x85bfef6 (W)
{
    charac_expand::CData::alter();
    m_useGold += gold;
}

void CCharacBloodDungeon::IncEnterCount()            // ORIG 0x85bfea0 (W)
{
    charac_expand::CData::alter();
    ++m_enterCount;
    ++m_weeklyEnterCount;
}

int CCharacBloodDungeon::GetEnterCount() const       // ORIG 0x85bfe88 (W)
{
    return m_enterCount;                             // 读 +0x18
}

int CCharacBloodDungeon::GetWeeklyEnterCount() const // ORIG 0x85bfe94 (W)
{
    return m_weeklyEnterCount;                       // 读 +0x0c
}
