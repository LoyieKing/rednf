// df_monitor_r — DNFSignal（从 MonitorTypes/App/Table 拆分）
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
#include <sys/resource.h>
#include <algorithm>

#include "DNFSignal.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "DNFPacketTracer.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFApplication.h"
#include "DNFPacketTracer.h"

void CSignal::attachApp(CApplication* app)
{
    m_app = app;
}

void CSignal::handle(int sig) {}

CSignal::~CSignal() {}

void CSignal::dump_core_file()
{
    CPacketTracerInstance()->AbsoluteWriteLog();
    struct rlimit rl;
    int ret = getrlimit(RLIMIT_CORE, &rl);
    rl.rlim_cur = (rlim_t)-1;
    ret = setrlimit(RLIMIT_CORE, &rl);
    abort();
}
