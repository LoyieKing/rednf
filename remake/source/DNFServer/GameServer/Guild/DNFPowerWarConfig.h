#ifndef DNFPOWERWARCONFIG_H_
#define DNFPOWERWARCONFIG_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"
#include "DNFTableBase.h"
#include "PowerWar.h"

class CPowerWarConfig;

// from GuildDomain.h
class CPowerWarConfig : public CTableBase
{
public:
    CPowerWarConfig();
    virtual ~CPowerWarConfig();
    virtual void Load_Table(const std::string& path);
    virtual int Parse_Table(char* line, int idx);
    void Clear_Table();
    ST_PowerWarEventStartTimeConfig* GetInfo() const;
    ST_PowerWarEventStartTimeConfig m_info;  // +4
};

#endif
