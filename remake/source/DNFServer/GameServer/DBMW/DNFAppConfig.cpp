// df_dbmw_r - DNFAppConfig (ORIG DNFAppConfig.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

// CTableBase / CAppInit / CAppConfig / CServerConfig
CAppConfig::CAppConfig()
{
    m_cipher.Initialize("qortmddkqortmdcksqordudwlswjdguswn", 0x21, CTEA::sm_chain0, 0, 0);
    memset(m_dbConnInfo, 0, 0x17e8);
    m_tcpPort = 0;
    m_dbmwType = 0;
    m_serverGroup = 0;
}
CAppConfig::~CAppConfig() {}
STDBConnInfo::STDBConnInfo()
{
    memset(this, 0, 0x11);
    memset(m_user, 0, 0x15);
    memset(m_pass, 0, 0x15);
    memset(m_db, 0, 0x1f);
    memset(m_data, 0, 0x100);
    m_port = 0;
    m_tail = 0;
}
int CAppConfig::Load_Table(const std::string& fileName)
{
    CMyFileLog log("Load_Table", 0x35d);
    log("./log/process.log", "CAppConfig::Load_Table :  _S_MOD_CFG_LOADER_BY_NAME\n");
    std::string path = "./cfg/" + fileName + ".cfg";
    int n = Load_Txt_Table_Data(path.c_str(), 0xff);
    if (n > 0 && n <= 0xfe)
        return n;
    CMyFileLog log2("Load_Table", 0x365);
    log2("./log/TableError.log", "App Config Table - ReturnCode = %d\n", n);
    throw CDNFException("CAppConfig::Load_Setup_Table() Exception Break!");
}
STDBConnInfo* CAppConfig::GetDBConnInfo(ENUM_DB_HANDLE_IDX idx)
{
    return (STDBConnInfo*)((char*)this + ((int)idx * 0x168 + 0x60));
}
int CAppConfig::GetServerGroup()
{
    return m_serverGroup;
}
int CAppConfig::DecryptValue(const char* value, char* dst)
{
    char buf1[0x40] = {0};
    char buf2[0x40] = {0};
    if (!DNFFLib::Hex2Binary(value, (unsigned char*)buf2, 0x18))
        return 0;
    m_cipher.Decrypt(buf2, buf1, 0x18);
    strncpy(dst, buf1, 0x14);
    return 1;
}
#define DBMW_PARSE_PWD(i, keyname) \
    else if (strcmp(key, keyname) == 0) { \
        char b1[0x40] = {0}; char b2[0x40] = {0}; \
        if (!DNFFLib::Hex2Binary(value, (unsigned char*)b2, 0x18)) return 0; \
        m_cipher.Decrypt(b2, b1, 0x18); \
        memcpy(m_dbConnInfo[i].m_pass, b1, strlen(b1)); }

int CAppConfig::Parse_Table(char* data, int size)
{
    if (data[0] == '#')
        return 0;
    char* fields[2];
    if (DNFFLib::ExplodeString(data, " =\t\r\n\"", fields, 2) != 2)
        return 0;
    if (size > 0xfe)
        return 0;
    char* key = fields[0];
    char* value = fields[1];
    if (strcmp(key, "tick_value") == 0)
        m_tickValue = (char)atoi(value);
    else if (strcmp(key, "udp_port") == 0)
        m_udpPort = atoi(value);
    else if (strcmp(key, "master_db_ip") == 0)
        memcpy(m_dbConnInfo[0].m_host, value, strlen(value));
    else if (strcmp(key, "master_db_port") == 0)
        m_dbConnInfo[0].m_port = atoi(value);
    else if (strcmp(key, "master_db_acc") == 0)
        memcpy(m_dbConnInfo[0].m_user, value, strlen(value));
    else if (strcmp(key, "master_db_pwd") == 0)
        DecryptValue(value, m_dbConnInfo[0].m_pass);
    else if (strcmp(key, "master_db_name") == 0)
        memcpy(m_dbConnInfo[0].m_db, value, strlen(value));
    else if (strcmp(key, "neople_db_ip") == 0)
        memcpy(m_dbConnInfo[1].m_host, value, strlen(value));
    else if (strcmp(key, "neople_db_port") == 0)
        m_dbConnInfo[1].m_port = atoi(value);
    else if (strcmp(key, "neople_db_acc") == 0)
        memcpy(m_dbConnInfo[1].m_user, value, strlen(value));
    DBMW_PARSE_PWD(1, "neople_db_pwd")
    else if (strcmp(key, "neople_db_name") == 0)
        memcpy(m_dbConnInfo[1].m_db, value, strlen(value));
    else if (strcmp(key, "game_db_ip") == 0)
        memcpy(m_dbConnInfo[2].m_host, value, strlen(value));
    else if (strcmp(key, "game_db_port") == 0)
        m_dbConnInfo[2].m_port = atoi(value);
    else if (strcmp(key, "game_db_acc") == 0)
        memcpy(m_dbConnInfo[2].m_user, value, strlen(value));
    DBMW_PARSE_PWD(2, "game_db_pwd")
    else if (strcmp(key, "game_db_name") == 0)
        memcpy(m_dbConnInfo[2].m_db, value, strlen(value));
    else if (strcmp(key, "log_db_ip") == 0)
        memcpy(m_dbConnInfo[4].m_host, value, strlen(value));
    else if (strcmp(key, "log_db_port") == 0)
        m_dbConnInfo[4].m_port = atoi(value);
    else if (strcmp(key, "log_db_acc") == 0)
        memcpy(m_dbConnInfo[4].m_user, value, strlen(value));
    DBMW_PARSE_PWD(4, "log_db_pwd")
    else if (strcmp(key, "log_db_name") == 0)
        memcpy(m_dbConnInfo[4].m_db, value, strlen(value));
    else if (strcmp(key, "sso_db_ip") == 0)
        memcpy(m_dbConnInfo[6].m_host, value, strlen(value));
    else if (strcmp(key, "sso_db_port") == 0)
        m_dbConnInfo[6].m_port = atoi(value);
    else if (strcmp(key, "sso_db_acc") == 0)
        memcpy(m_dbConnInfo[6].m_user, value, strlen(value));
    DBMW_PARSE_PWD(6, "sso_db_pwd")
    else if (strcmp(key, "sso_db_name") == 0)
        memcpy(m_dbConnInfo[6].m_db, value, strlen(value));
    else if (strcmp(key, "game_db_2nd_ip") == 0)
        memcpy(m_dbConnInfo[3].m_host, value, strlen(value));
    else if (strcmp(key, "game_db_2nd_port") == 0)
        m_dbConnInfo[3].m_port = atoi(value);
    else if (strcmp(key, "game_db_2nd_acc") == 0)
        memcpy(m_dbConnInfo[3].m_user, value, strlen(value));
    DBMW_PARSE_PWD(3, "game_db_2nd_pwd")
    else if (strcmp(key, "game_db_2nd_name") == 0)
        memcpy(m_dbConnInfo[3].m_db, value, strlen(value));
    else if (strcmp(key, "guild_db_ip") == 0)
        memcpy(m_dbConnInfo[8].m_host, value, strlen(value));
    else if (strcmp(key, "guild_db_port") == 0)
        m_dbConnInfo[8].m_port = atoi(value);
    else if (strcmp(key, "guild_db_acc") == 0)
        memcpy(m_dbConnInfo[8].m_user, value, strlen(value));
    DBMW_PARSE_PWD(8, "guild_db_pwd")
    else if (strcmp(key, "guild_db_name") == 0)
        memcpy(m_dbConnInfo[8].m_db, value, strlen(value));
    else if (strcmp(key, "web_db_ip") == 0)
        memcpy(m_dbConnInfo[5].m_host, value, strlen(value));
    else if (strcmp(key, "web_db_port") == 0)
        m_dbConnInfo[5].m_port = atoi(value);
    else if (strcmp(key, "web_db_acc") == 0)
        memcpy(m_dbConnInfo[5].m_user, value, strlen(value));
    DBMW_PARSE_PWD(5, "web_db_pwd")
    else if (strcmp(key, "web_db_name") == 0)
        memcpy(m_dbConnInfo[5].m_db, value, strlen(value));
    else if (strcmp(key, "stat_db_ip") == 0)
        memcpy(m_dbConnInfo[0xf].m_host, value, strlen(value));
    else if (strcmp(key, "stat_db_port") == 0)
        m_dbConnInfo[0xf].m_port = atoi(value);
    else if (strcmp(key, "stat_db_acc") == 0)
        memcpy(m_dbConnInfo[0xf].m_user, value, strlen(value));
    DBMW_PARSE_PWD(0xf, "stat_db_pwd")
    else if (strcmp(key, "stat_db_name") == 0)
        memcpy(m_dbConnInfo[0xf].m_db, value, strlen(value));
    else if (strcmp(key, "dbmw_type") == 0)
        m_dbmwType = (char)atoi(value);
    else if (strcmp(key, "server_group") == 0)
        m_serverGroup = atoi(value);
    else if (strcmp(key, "tcp_port") == 0)
        m_tcpPort = atoi(value);
    else if (strcmp(key, "event_db_ip") == 0)
        memcpy(m_dbConnInfo[9].m_host, value, strlen(value));
    else if (strcmp(key, "event_db_port") == 0)
        m_dbConnInfo[9].m_port = atoi(value);
    else if (strcmp(key, "event_db_acc") == 0)
        memcpy(m_dbConnInfo[9].m_user, value, strlen(value));
    DBMW_PARSE_PWD(9, "event_db_pwd")
    else if (strcmp(key, "event_db_name") == 0)
        memcpy(m_dbConnInfo[9].m_db, value, strlen(value));
    else if (strcmp(key, "se_event_db_ip") == 0)
        memcpy(m_dbConnInfo[0xd].m_host, value, strlen(value));
    else if (strcmp(key, "se_event_db_port") == 0)
        m_dbConnInfo[0xd].m_port = atoi(value);
    else if (strcmp(key, "se_event_db_acc") == 0)
        memcpy(m_dbConnInfo[0xd].m_user, value, strlen(value));
    DBMW_PARSE_PWD(0xd, "se_event_db_pwd")
    else if (strcmp(key, "se_event_db_name") == 0)
        memcpy(m_dbConnInfo[0xd].m_db, value, strlen(value));
    else
        return 0;
    return 1;
}
int CAppConfig::Check_FileName(const std::string& fileName)
{
    std::string cfg = std::string("./cfg/") + fileName + std::string(".cfg");
    if (access(cfg.c_str(), 0) != 0)
        throw CDNFException(
            "CAppConfig::Set_FileName() There is no cfg file Exception Break! Check ./cfg/*.cfg files");
    std::string pid = std::string("./pid/") + fileName + std::string(".pid");
    if (access(pid.c_str(), 0) != 0)
        throw CDNFException(
            "CAppConfig::Set_FileName() There is already pid file! Exception Break! Check ./pid/*.pid files");
    return 1;
}
int CAppConfig::Get_ServerUdpPort() { return m_udpPort; }
int CAppConfig::Get_ServerTcpPort() { return m_tcpPort; }
unsigned short CAppConfig::Get_FrameCountValue() { return (unsigned char)m_tickValue; }
unsigned char CAppConfig::Get_DbmwType() { return m_dbmwType; }
