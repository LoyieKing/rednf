// df_monitor_r — DNFBuddy（从 MonitorTypes/App/Table 拆分）
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

#include "DNFBuddy.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFMember.h"

void CBuddy::operator delete(void* p, unsigned int size) { ::operator delete(p); }

unsigned int* CBuddy::getBuddyDBInfo() { return (unsigned int*)this; }

void* CBuddy::operator new(unsigned int size) { return ::operator new(size); }

void CBuddy::operator delete(void* ptr) { ::operator delete(ptr); }

CBuddy::CBuddy(STBuddyDBInfo& info)
{
    memcpy(this, &info, 0x27);
}

CBuddy::~CBuddy() {}

