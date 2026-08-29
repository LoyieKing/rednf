#ifndef DNF_GAME_CEVENTCREATEDNF_H_
#define DNF_GAME_CEVENTCREATEDNF_H_

// 台湾 CreateDnfEvent：创建角色奖励事件状态机。
#include "CGuildServerProxy.h"  // CEventBase 权威声明

// Word_Param 由 CGuildServerProxy.h 权威定义。
class CUser;

class CEventCreateDnf : public CEventBase
{
public:
    struct State { enum T { kWaiting = 0, kIng = 1, kReward = 2 }; };

    CEventCreateDnf();
    virtual ~CEventCreateDnf();

    virtual void StartEvent();
    virtual void StartEvent(Word_Param param);
    virtual void EndEvent();
    void RegistNextTimer();
    void RegistStartTimer(bool immediate);
    void SetEventInfo(int hour, int rate, int reloadInterval);
    bool IsRewardCondition();
    int GetState();
    unsigned short GetRate();
    void SendReward(CUser* user);
    void setState(State::T state);

private:
    State::T m_state;
    unsigned char m_startHour;
    unsigned char m_endHour;
    unsigned short m_reloadInterval;
    unsigned char m_reserved;
    unsigned short m_rate;

};
#endif
