// df_monitor_r — DNFProhibitUser（从 MonitorTypes/App/Table 拆分）
// MemPool<T> 的完整定义在 DNFPacketBuffer.h（静态池成员需要完整类型才能定义）
#include "DNFPacketBuffer.h"
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

void CDNFProhibitUser::operator delete(void* p, unsigned int size) { m_ProhibitUsermemPool_.free(p, size); }

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

// ORIG：`CDNFProhibitUser::operator new` = `MemPool<CDNFProhibitUser>::m_ProhibitUsermemPool_.alloc()`（忽略入参 size）。
MemPool<CDNFProhibitUser> CDNFProhibitUser::m_ProhibitUsermemPool_(0x2710);

void* CDNFProhibitUser::operator new(unsigned int size)
{
    (void)size;
    return m_ProhibitUsermemPool_.alloc();
}

void CDNFProhibitUser::operator delete(void* p) { m_ProhibitUsermemPool_.free(p); }

unsigned int CDNFProhibitUser::GetDBID() { return *(unsigned int*)this; }

unsigned short CDNFProhibitUser::GetProhibitRemainTime()
{
    return m_remain;
}

char CDNFProhibitUser::GetChannelNo() { return m_channel; }

char CDNFProhibitUser::fromWeb() { return m_flag; }

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0042 | monitor | 与ORIG差异=DIFF | CDNFProhibitUser::SetUserConnectableTime | 详见 function_reports/monitor/_ZN16CDNFProhibitUser22SetUserConnectableTimeEjscb.md
void CDNFProhibitUser::SetUserConnectableTime(unsigned int dbid, short time, char channel,
                                              bool flag)
{
    if (!m_flag)
    {
        m_remain = time;
        m_dbid = dbid;
        m_channel = channel;
        m_flag = flag;
    }
}
