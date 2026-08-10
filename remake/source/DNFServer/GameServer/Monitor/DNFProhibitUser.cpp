// df_monitor_r — DNFProhibitUser（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "DNFProhibitUser.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

void CDNFProhibitUser::operator delete(void* p, unsigned int size) { ::operator delete(p); }

CDNFProhibitUser::CDNFProhibitUser()
{
    m_dbid = 0;
    m_remain = 0;
    m_channel = 0xff;
    m_flag = 0;
}

CDNFProhibitUser::~CDNFProhibitUser()
{
    m_remain = 0;
    m_dbid = 0;
    m_channel = 0xff;
}

void* CDNFProhibitUser::operator new(unsigned int size) { return ::operator new(size); }

void CDNFProhibitUser::operator delete(void* p) { ::operator delete(p); }

unsigned int CDNFProhibitUser::GetDBID() { return *(unsigned int*)this; }

unsigned short CDNFProhibitUser::GetProhibitRemainTime()
{
    return *(unsigned short*)((char*)this + 4);
}

bool CDNFProhibitUser::IsTimeOutConnectable()
{
    *(unsigned short*)((char*)this + 4) =
        (unsigned short)(*(unsigned short*)((char*)this + 4) - 1);
    if (*(unsigned short*)((char*)this + 4) <= 0)
    {
        return 1;
    }
    return 0;
}

char CDNFProhibitUser::GetChannelNo() { return *(char*)((char*)this + 6); }

char CDNFProhibitUser::fromWeb() { return 0; }

void CDNFProhibitUser::SetUserConnectableTime(unsigned int dbid, short time, char channel,
                                              bool flag)
{
    if (*(char*)((char*)this + 7) != 1)
    {
        *(short*)((char*)this + 4) = time;
        *(unsigned int*)this = dbid;
        *(char*)((char*)this + 6) = channel;
        *(char*)((char*)this + 7) = (char)flag;
    }
}

