// df_monitor_r — DNFProhibitUser（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
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
    return m_remain;
}

char CDNFProhibitUser::GetChannelNo() { return m_channel; }

char CDNFProhibitUser::fromWeb() { return m_flag; }

void CDNFProhibitUser::SetUserConnectableTime(unsigned int dbid, short time, char channel,
                                              bool flag)
{
    if (!m_flag)
    {
        m_remain = (unsigned short)time;
        m_dbid = dbid;
        m_channel = channel;
        m_flag = flag;
    }
}
