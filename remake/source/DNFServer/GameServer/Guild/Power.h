#ifndef POWER_H_
#define POWER_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"

class CGuildManager;
class CPower;

// from GuildDomain.h
class CPower
{
public:
    CPower();
    virtual ~CPower();
    void SetScore(int score);
    int GetScore();
    int IncScore(int score);
    void InitPower();
    void CleanPower();
    void CalcPowerWarRank();
    void UpdatePowerWarInfo(int a, unsigned int b, unsigned int c);
    void RewardGuildPowerWarPoint(CGuildManager& gm, bool a, int b, int c, int d, int e);
    CPowerWarGuildInfo* GetPowerWarGuildInfo();
    CPowerWarCharacInfo* GetPowerWarCharacInfo();
    int m_field4;                      // +4
    CPowerWarGuildInfo m_guildInfo;    // +8
    CPowerWarCharacInfo m_characInfo;  // +0x3c
};

#endif
