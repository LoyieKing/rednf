// df_manager_r — CAppConfig（ORIG DNFAppConfig.cpp）
#include "DNFAppConfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

CAppConfig::CAppConfig() {}
CAppConfig::~CAppConfig() {}

int CAppConfig::Load_Table(const std::string& fileName)
{
    std::string path = std::string("./cfg/") + fileName + std::string(".cfg");
    int n = Load_Txt_Table_Data(path.c_str(), 0x13);
    if (n > 0 && n <= 0x12)
        return 1;
    CMyFileLog log("Load_Table", 0x4e);
    log("./log/TableError.log", "App Config Table - ReturnCode = %d\n", n);
    throw CDNFException("CAppConfig::Load_Setup_Table() Exception Break!");
}
int CAppConfig::Parse_Table(char* data, int size)
{
    if (data[0] == '#')
        return 0;
    char* fields[2];
    if (DNFFLib::ExplodeString(data, " \t\r\n\"", fields, 2) == 2 && size <= 0x12)
    {
        switch (size)
        {
        case 0:
            m_frameCount = (char)atoi(fields[0]);
            break;
        case 1:
            m_serverUdpPort = atoi(fields[0]);
            break;
        case 2:
            m_serverTcpPort = atoi(fields[0]);
            break;
        default:
            return 0;
        }
        return 1;
    }
    return 0;
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

int CAppConfig::Get_ServerUdpPort() { return m_serverUdpPort; }
int CAppConfig::Get_ServerTcpPort() { return m_serverTcpPort; }
unsigned int CAppConfig::Get_FrameCountValue() { return m_frameCount; }
