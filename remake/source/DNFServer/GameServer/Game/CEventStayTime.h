#ifndef GAME_CEVENTSTAYTIME_H_
#define GAME_CEVENTSTAYTIME_H_

#include "CGuildServerProxy.h"

class CEventStayTime : public CEventBase
{
public:
    CEventStayTime();
    virtual ~CEventStayTime();
    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
    virtual void dailyresetData();
    void RegistNextTimer();
    bool IsFirstReset();
    unsigned int getRewardStayTime() const;
    unsigned int getIntervalStayTime() const;
private:
    unsigned int m_rewardStayTime;
    unsigned int m_intervalStayTime;
    bool m_firstReset;
};

#endif
