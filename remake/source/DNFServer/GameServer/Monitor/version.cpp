// df_monitor_r — version（从 MonitorTypes/App/Table 拆分）
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

#include "version.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

CSourceVersionMgr::CSourceVersionMgr()
{
    InsertSourceVersion((char*)".svn/all-wcprops", 0x19daa);
    InsertSourceVersion((char*)"../ServerCommon/.svn/all-wcprops", 0x19daa);
}

CSourceVersionMgr::SourceVersion::SourceVersion(char* path, int version)
{
    m_path = path;
    m_version = version;
}

CSourceVersionMgr::SourceVersion::~SourceVersion() {}

void CSourceVersionMgr::InsertSourceVersion(char* path, int version)
{
    m_versions.push_back(SourceVersion(path, version));
}

CVersionMgr::CVersionMgr(int a, int b, int c, int d)
{
    m_a = a;
    m_b = b;
    m_c = c;
    m_d = d;
}


