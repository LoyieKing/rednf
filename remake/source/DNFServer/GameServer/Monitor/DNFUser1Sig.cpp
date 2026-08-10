// df_monitor_r — DNFUser1Sig（从 MonitorTypes/App/Table 拆分）
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

#include "DNFUser1Sig.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFApplication.h"
#include "DNFUser.h"

CUser1Sig::CUser1Sig() {}

CUser1Sig::~CUser1Sig() {}

void CUser1Sig::handle(int sig)
{
    CMyFileLog log("handle", 0x13);
    log("./log/Signal", "CUser1Sig::handle( int sig )");
    if (m_app != 0)
    {
        m_app->SendTestPacket_2();
    }
}

CUser2Sig::CUser2Sig() {}

CUser2Sig::~CUser2Sig() {}

void CUser2Sig::handle(int sig)
{
    CMyFileLog log("handle", 0x20);
    log("./log/Signal", "CUser2Sig::handle( int sig )");
    if (m_app != 0)
    {
        m_app->TranslateSignal();
    }
}
