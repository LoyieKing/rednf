// df_monitor_r — DNFAppStopInit（从 MonitorTypes/App/Table 拆分）
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

#include "DNFAppStopInit.h"
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
#include "DNFTableBase.h"

CAppStopInit::CAppStopInit() {}

CAppStopInit::~CAppStopInit() {}

void CAppStopInit::Init(CApplication* app, int argc, char** argv)
{
    puts("RECV STOP, \xb0\xfc\xb8\xae\xc0\xda\xbf\xa1 \xc0\xc7\xc7\xd8 \xb0\xad\xc1\xa6\xb7\xce \xc1\xbe\xb7\xe1 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
    app->Clear();
    register bool fail;
    {
        std::allocator<char> alloc;
        std::string pidFile(argv[1], alloc);
        fail = !app->Send_Term_Signal(pidFile);
    }
    if (fail)
    {
        throw CDNFException(
            "CAppStopInit::Init()\xbf\xa1 \xc0\xc7\xc7\xd8 \xb0\xad\xc1\xa6\xb7\xce \xc1\xbe\xb7\xe1\xb5\xc7\xbe\xfa\xc0\xbd!");
    }
    throw CDNFException(
        "CAppStopInit::Init()\xbf\xa1 \xc0\xc7\xc7\xd8 \xb0\xad\xc1\xa6\xb7\xce \xc1\xbe\xb7\xe1\xb5\xc7\xbe\xfa\xc0\xbd!_1");
}
