// W9_EventSmall.h —— Wave9 事件小类批声明。
// CTrcOnOffEvent / CFatigueBuffEvent / CWeekendBonusEvent /
// CAutoMarketConditionsControlEvent / CCharacBloodDungeon。
// 全部签名/mangled 与 ORIG(df_game_r) 一致；定义位于 W9_EventSmall.cpp。
#ifndef GAME_W9_EVENTSMALL_H_
#define GAME_W9_EVENTSMALL_H_

#include "CGuildServerProxy.h"   // CEventBase/Word_Param
#include "CBoosterGage.h"        // charac_expand::CData

// ---- DB 依赖（定义 W9_EventSmall.cpp；ORIG T 0x08433ada）----
class DB_LoadAutoMarketConditionsControl
{
public:
    static bool makeRequest(bool flag);   // _ZN34DB_LoadAutoMarketConditionsControl11makeRequestEb
};

// ============================ CTrcOnOffEvent ============================
// 纯日志开关事件：Start/End 仅 SetEventFlag + cMyTrace；ORIG 地址：
//   C1/C2 0x08285a44 D1/D2 0x08285a60 D0 0x08285a90
//   StartEvent() 0x08285aae StartEvent(Word_Param) 0x08285b00
//   EndEvent() 0x08285b52 IsEventing 0x08285ba4
class CTrcOnOffEvent : public CEventBase
{
public:
    CTrcOnOffEvent();
    ~CTrcOnOffEvent();

    virtual void StartEvent();                  // SetEventFlag(true)+trace
    virtual void EndEvent();                    // SetEventFlag(false)+trace
    virtual void StartEvent(Word_Param param);  // 同上(Word_Param 重载)
    virtual bool IsEventing(CUser* user) const; // 透传基类判活
};

// ============================ CFatigueBuffEvent ============================
// ORIG：C1/C2 0x0811bbea D1/D2 0x0811bc06 D0 0x0811bc36
//       StartEvent() 0x0811bc54 StartEvent(Word_Param) 0x0811bca6
//       EndEvent() 0x0811bcc0
class CFatigueBuffEvent : public CEventBase
{
public:
    CFatigueBuffEvent();
    ~CFatigueBuffEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

// ============================ CWeekendBonusEvent ============================
// ORIG：C1/C2 0x08117550 D1/D2 0x0811756c D0 0x0811759c
//       StartEvent() 0x081175ba StartEvent(Word_Param) 0x0811760c
//       EndEvent() 0x08117626
class CWeekendBonusEvent : public CEventBase
{
public:
    CWeekendBonusEvent();
    ~CWeekendBonusEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

// ==================== CAutoMarketConditionsControlEvent ====================
// 市场条件控制事件：StartEvent()=flag+trace；
// StartEvent(Word_Param)=SetEventFlag(true)+
// DB_LoadAutoMarketConditionsControl::makeRequest(true)（0x8109cd0）。
// ORIG：C1/C2 0x08109c14 D1/D2 0x08109c30 D0 0x08109c60
class CAutoMarketConditionsControlEvent : public CEventBase
{
public:
    CAutoMarketConditionsControlEvent();
    ~CAutoMarketConditionsControlEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

// ============================ CCharacBloodDungeon ============================
// 血色地下城角色计数器（charac_expand::CData 派生，vptr 0x08c279a8）。
// ORIG：C1/C2 0x0832af6e D1/D2 0x0832a716 D0 0x0832a746
//   _reset 0x832a2d4 _saveData 0x832a330 loadData 0x832a4c6
//   getData 0x832a53a ResetDaily 0x832a5b6
//   IncPoint(W) 0x85bfed2 IncUseGold(W) 0x85bfef6 IncEnterCount(W) 0x85bfea0
//   GetEnterCount(W,K) 0x85bfe88 GetWeeklyEnterCount(W,K) 0x85bfe94
class CCharacBloodDungeon : public charac_expand::CData
{
public:
    CCharacBloodDungeon();

    virtual bool loadData(CUser* pUser, char* pData);      // 返回 true
    virtual void getData(char* pData) const;               // 镜像到 sig+4..
    virtual void ResetDaily();                             // 日清 + 周一 weekly 清零
    virtual bool _saveData(CUser* pUser);                  // SIG_BLOOD_DUNGEON 入队
    virtual void _reset();                                 // 全字段清零

    void IncPoint(int delta);                              // m_point += delta
    void IncUseGold(int gold);                             // m_useGold += gold
    void IncEnterCount();                                  // m_enterCount/m_weeklyEnterCount ++

    int GetEnterCount() const;                             // 返回 m_enterCount(+0x18)
    int GetWeeklyEnterCount() const;                       // 返回 m_weeklyEnterCount(+0x0c)

private:
    // ORIG 布局：基类 charac_expand::CData(0x08) 之后，_reset 覆盖 +0x08..+0x28，
    // sizeof(this)==0x2c。语义标注 [推断]（Inc*/Get* 反汇编实证，其余字段名仅占位）。
    unsigned int m_point;    // +0x08（IncPoint 写入；b28d-lane6：unsigned 复现 ORIG add 段 P211）
    int m_weeklyEnterCount;  // +0x0c（GetWeeklyEnterCount / IncEnterCount ++）
    unsigned int m_useGold;  // +0x10（IncUseGold 写入；b28d-lane6：同 P211）
    int m_field14;           // +0x14 [推断]
    int m_enterCount;        // +0x18（GetEnterCount / IncEnterCount ++）
    int m_field1c;           // +0x1c [推断]
    unsigned char m_flag20;  // +0x20 [推断]（_saveData 后置 0 的持久标志）
    int m_field24;           // +0x24 [推断]
    int m_field28;           // +0x28 [推断]
};

#endif  // GAME_W9_EVENTSMALL_H_
