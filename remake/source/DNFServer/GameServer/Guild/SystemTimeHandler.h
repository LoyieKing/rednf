#ifndef SYSTEMTIMEHANDLER_H_
#define SYSTEMTIMEHANDLER_H_

#include <string>
#include <vector>

#include "PacketHeader.h"

class CommonTime;

// from GuildMisc.h
class CommonTime
{
public:
    void SetCurTime();
    char m_time[6];
};

#endif
