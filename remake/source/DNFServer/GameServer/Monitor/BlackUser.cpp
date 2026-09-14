// df_monitor_r — BlackUser（从 MonitorTypes/App/Table 拆分）
// MemPool<T> 的完整定义在 DNFPacketBuffer.h（静态池成员需要完整类型才能定义）
#include "DNFPacketBuffer.h"
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

#include "BlackUser.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

// ORIG：`CBlackUser::operator new` = `MemPool<CBlackUser>::m_memPool_.alloc()`（忽略入参 size）。
MemPool<CBlackUser> CBlackUser::m_memPool_(0x186a0);

void* CBlackUser::operator new(unsigned int size)
{
    (void)size;
    return m_memPool_.alloc();
}

void CBlackUser::operator delete(void* p, unsigned int size) { m_memPool_.free(p, size); }

void CBlackUser::SetBlackUser(char* name, unsigned int time)
{
    memcpy(m_name, name, 0x1d);
    m_occurTime = time;
}

void CBlackUser::ChangeCharName(char* name)
{
    memset(m_name, 0, 0x1e);
    memcpy(m_name, name, 0x1d);
}

char* CBlackUser::GetName() { return (char*)this; }

unsigned int CBlackUser::GetOccurTime() { return m_occurTime; }

void CBlackUser::operator delete(void* p) { m_memPool_.free(p); }

CBlackUser::CBlackUser() throw()
{
    m_occurTime = 0;
    memset(m_name, 0, 0x1e);
}

STBlackUserDBType::STBlackUserDBType()
{
    m_dbid = 0;
    m_occurTime = 0;
    memset(m_name, 0, 0x1e);
}
