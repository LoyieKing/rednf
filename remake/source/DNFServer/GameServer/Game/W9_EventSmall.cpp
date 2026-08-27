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
    guard << 0x15a;
    guard << -1;
    guard << flag;
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
    cMyTrace trace("StartEvent", 0x13, 0);
    trace("Trc On Off Event Start!");
}

void CTrcOnOffEvent::EndEvent()              // ORIG 0x08285b52
{
    SetEventFlag(false);
    cMyTrace trace("EndEvent", 0x1f, 0);
    trace("Trc On Off Event Event End!");
}

void CTrcOnOffEvent::StartEvent(Word_Param)  // ORIG 0x08285b00
{
    SetEventFlag(true);
    cMyTrace trace("StartEvent", 0x19, 0);
    trace("Trc On Off Event Event Start!");
}

bool CTrcOnOffEvent::IsEventing(CUser* user) const  // ORIG 0x08285ba4：透传基类
{
    return CEventBase::IsEventing(user) == true;
}

// ---- CFatigueBuffEvent ----
CFatigueBuffEvent::CFatigueBuffEvent() {}    // ORIG 0x0811bbea
CFatigueBuffEvent::~CFatigueBuffEvent() {}   // ORIG 0x0811bc06

void CFatigueBuffEvent::StartEvent()         // ORIG 0x0811bc54
{
    SetEventFlag(true);
    cMyTrace trace("StartEvent", 0x11, 0);
    trace("CFatigueBuffEvent Event Start!");
}

void CFatigueBuffEvent::EndEvent()           // ORIG 0x0811bcc0
{
    SetEventFlag(false);
    cMyTrace trace("EndEvent", 0x1b, 0);
    trace("CFatigueBuffEvent Event End!");
}

void CFatigueBuffEvent::StartEvent(Word_Param)  // ORIG 0x0811bca6（同构）
{
    SetEventFlag(true);
    cMyTrace trace("StartEvent", 0x19, 0);
    trace("CFatigueBuffEvent Event Start!");
}

// ---- CWeekendBonusEvent ----
CWeekendBonusEvent::CWeekendBonusEvent() {}  // ORIG 0x08117550
CWeekendBonusEvent::~CWeekendBonusEvent() {} // ORIG 0x0811756c

void CWeekendBonusEvent::StartEvent()        // ORIG 0x081175ba
{
    SetEventFlag(true);
    cMyTrace trace("StartEvent", 0x12, 0);
    trace("CWeekendBonusEvent Event Start!");
}

void CWeekendBonusEvent::EndEvent()          // ORIG 0x08117626
{
    SetEventFlag(false);
    cMyTrace trace("EndEvent", 0x1c, 0);
    trace("CWeekendBonusEvent Event End!");
}

void CWeekendBonusEvent::StartEvent(Word_Param)  // ORIG 0x0811760c（同构）
{
    SetEventFlag(true);
    cMyTrace trace("StartEvent", 0x19, 0);
    trace("CWeekendBonusEvent Event Start!");
}

// ---- CAutoMarketConditionsControlEvent ----
CAutoMarketConditionsControlEvent::CAutoMarketConditionsControlEvent() {}  // 0x08109c14
CAutoMarketConditionsControlEvent::~CAutoMarketConditionsControlEvent() {} // 0x08109c30

void CAutoMarketConditionsControlEvent::StartEvent()  // ORIG 0x08109c7e
{
    SetEventFlag(true);
    cMyTrace trace("StartEvent", 0x13, 0);
    trace("CAutoMarketConditionsControlEvent Start!");
}

void CAutoMarketConditionsControlEvent::EndEvent()    // ORIG 0x08109cf8
{
    SetEventFlag(false);
    cMyTrace trace("EndEvent", 0x1f, 0);
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
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("CharacBloodDungeon.cpp", 0x1e);
    CStreamGuard guard(stream, true);
    guard << 0x197;
    guard << (int)pUser->GetUID();

    SIG_BLOOD_DUNGEON* sig = guard.GetInBuffer<SIG_BLOOD_DUNGEON>();
    memset(sig, 0, sizeof(SIG_BLOOD_DUNGEON));   // ORIG memset 0x28
    sig->m_characNo = (int)pUser->getCurCharacNo();
    sig->m_point = m_point;                      // this+0x08 → sig+0x04
    sig->m_weeklyEnterCount = m_weeklyEnterCount;
    sig->m_useGold = m_useGold;
    sig->m_dailyInitTime = getDailyInitializeTime(6, 0, 0);  // 覆写 +0x10
    sig->m_field14 = m_field14;
    sig->m_enterCount = m_enterCount;
    sig->m_flag1c = m_flag20;                    // byte 镜像
    sig->m_field20 = m_field24;
    sig->m_field24 = m_field28;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    m_flag20 = 0;                                // ORIG 发送后清标志
    return true;                                 // 返回 1
}

bool CCharacBloodDungeon::loadData(CUser*, char* pData)  // ORIG 0x832a4c6
{
    // DB 记录镜像读入（sig+0x04.. ↔ this 字段映射与 _saveData 一致；
    // sig+0 处 characNo 不读取、m_flag20 从 sig+0x10 前一字段无关位置跳过
    // —— 按 ORIG 汇编只复制列出的 7 个 int，this+0x20 byte 保持原值）。
    m_point             = *reinterpret_cast<int*>(pData + 0x04);
    m_weeklyEnterCount  = *reinterpret_cast<int*>(pData + 0x08);
    m_useGold           = *reinterpret_cast<int*>(pData + 0x0c);
    m_field14           = *reinterpret_cast<int*>(pData + 0x10);
    m_enterCount        = *reinterpret_cast<int*>(pData + 0x14);
    m_field1c           = *reinterpret_cast<int*>(pData + 0x18);
    m_field24           = *reinterpret_cast<int*>(pData + 0x20);
    m_field28           = *reinterpret_cast<int*>(pData + 0x24);
    return true;
}

void CCharacBloodDungeon::getData(char* pData) const  // ORIG 0x832a53a
{
    // 与 loadData 完全镜像的反向写出。
    *reinterpret_cast<int*>(pData + 0x04) = m_point;
    *reinterpret_cast<int*>(pData + 0x08) = m_weeklyEnterCount;
    *reinterpret_cast<int*>(pData + 0x0c) = m_useGold;
    *reinterpret_cast<int*>(pData + 0x10) = m_field14;
    *reinterpret_cast<int*>(pData + 0x14) = m_enterCount;
    *reinterpret_cast<int*>(pData + 0x18) = m_field1c;
    pData[0x1c] = static_cast<char>(m_flag20);   // byte 写出
    *reinterpret_cast<int*>(pData + 0x20) = m_field24;
    *reinterpret_cast<int*>(pData + 0x24) = m_field28;
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
