#ifndef POWERWAR_H_
#define POWERWAR_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"
#include "Scheduler.h"

class CPowerWar;
struct ST_PowerWarEventStartTimeConfig;

// from GuildDomain.h
struct ST_PowerWarEventStartTimeConfig
{
    ST_PowerWarEventStartTimeConfig();
    ~ST_PowerWarEventStartTimeConfig();
    unsigned char m_day;    // +0
    unsigned char m_hour;   // +1
    unsigned char m_min;    // +2
    char m_pad;             // +3
    int m_field4;           // +4
    std::vector<STPowerWarScheduleTime> m_schedule;  // +8
};

// from GuildDomain.h
class CPowerWar
{
public:
    CPowerWar();
    virtual ~CPowerWar();
    unsigned char IsPowerWarOn();
    unsigned char IsPowerWarOn() const;
    unsigned short getPowerWarEndKillPoint();
    unsigned short getPowerWarEndKillPoint() const;
    void setPowerWarEndKillPoint(unsigned short point);
    virtual void resetEvent();
    virtual void setEvent();
    void setProlongTime();
    void GetPowerWarConfigTbl(unsigned char& a, unsigned char& b, unsigned char& c,
                              unsigned char& d);
    void LoadPowerWarTableFile(char* path);
    int ProcessByMinuteEndEvent();
    int ProcessByMinuteStartEvent();
    int GetPowerWarRankingUpdateTime();
    char m_data[0x34];
};

#endif
