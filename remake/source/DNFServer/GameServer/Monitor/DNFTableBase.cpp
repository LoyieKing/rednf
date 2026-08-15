// df_monitor_r — DNFTableBase（从 MonitorTypes/App/Table 拆分）
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

#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

CDNFException::CDNFException(const std::string& msg) : m_msg(msg) {}

CDNFException::~CDNFException() throw() {}

const char* CDNFException::what() const throw()
{
    register const char* p = m_msg.c_str();
    CMyFileLog log(__FUNCTION__, 0x1a);
    log("./log/Except", "%s", p);
    return m_msg.c_str();
}

CTableBase::CTableBase() throw()
{
}

CTableBase::~CTableBase()
{
}

void CTableBase::Load_Table(const std::string& path)
{
}

int CTableBase::Load_Txt_Table_Data(const char* path, int maxCount)
{
    int count = 0;
    FILE* f;
    if ((f = fopen(path, "rb")) == 0)
    {
        return -1;
    }
    char line[1024];
    while (!feof(f) && fgets(line, 0x400, f) != 0)
    {
        if (line[0] == '#')
        {
            continue;
        }
        if (count >= maxCount)
        {
            return -2;
        }
        if (Parse_Table(line, count))
        {
            count++;
        }
    }
    fclose(f);
    return count;
}
