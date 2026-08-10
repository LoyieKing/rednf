// df_coserver_r — CAppConfig（ORIG DNFAppConfig.cpp）
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "DNFAppConfig.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

CAppConfig::CAppConfig()
{
    memset(m_udpPorts, 0, sizeof(m_udpPorts));
}

CAppConfig::~CAppConfig()
{
}

unsigned short CAppConfig::Get_FrameCountValue()
{
    return m_frameCount;
}

unsigned int CAppConfig::Get_ServerUdpPort(unsigned char idx)
{
    if (idx < 0x65)
    {
        return m_udpPorts[idx];
    }
    return 0;
}

bool CAppConfig::Parse_Table(char* line, int idx)
{
    if (line[0] == '#')
    {
        return 0;
    }
    char* tok0;
    char* tok1;
    int n;
    if (DNFFLib::ExplodeString(line, " \t\r\n\"", &tok0, 2) == 2)
    {
        if (idx < 0x14)
        {
            if (idx == 0)
            {
                m_frameCount = (char)atoi(tok1);
            }
            else
            {
                int i = atoi(tok0);
                if (i < 0x65)
                {
                    m_udpPorts[i] = (unsigned int)atoi(tok1);
                }
            }
            return 1;
        }
    }
    return 0;
}

void CAppConfig::Load_Table(const std::string& filename)
{
    std::string path = "./cfg/" + filename + ".cfg";
    int rc = Load_Txt_Table_Data(path.c_str(), 0x14);
    if (0 < rc && rc < 0x15)
    {
        return;
    }
    DNF_LOG_SCOPE_LINE(0x60, "./log/TableError", "App Config Table - ReturnCode = %d\n", rc);
    throw CDNFException("CAppConfig::Load_Setup_Table() Exception Break!");
}

void CAppConfig::Check_FileName(const std::string& filename)
{
    std::string cfg = "./cfg/" + filename + ".cfg";
    std::string pid = "./pid/" + filename + ".pid";
    if (access(cfg.c_str(), 0) != 0)
    {
        register const char* p_pid = pid.c_str();
        register const char* p_cfg = cfg.c_str();
        DNF_LOG_SCOPE_LINE(0x73, "./log/Init", "Check_FileName - %s,%s\n", p_cfg, p_pid);
        throw CDNFException(
            "CAppConfig::Set_FileName() There is no cfg file Exception Break! Check ./cfg/*.cfg files");
    }
    if (access(pid.c_str(), 0) == 0)
    {
        register const char* p_pid = pid.c_str();
        register const char* p_cfg = cfg.c_str();
        DNF_LOG_SCOPE_LINE(0x78, "./log/Init", "Check_FileName - %s,%s\n", p_cfg, p_pid);
        throw CDNFException(
            "CAppConfig::Set_FileName() There is already pid file! Exception Break! Check ./pid/*.pid files");
    }
}
