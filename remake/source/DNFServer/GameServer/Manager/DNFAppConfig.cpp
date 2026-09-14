// df_manager_r — CAppConfig（ORIG DNFAppConfig.cpp）
#include "DNFAppConfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

CAppConfig::CAppConfig() throw() {}
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
bool CAppConfig::Parse_Table(char* data, int size)
{
    if (data[0] == '#')
        return 0;
    char* fields[2];
    int n;  // ORIG 布局：额外槽位（fields 落在 -0x14）
    if (DNFFLib::ExplodeString(data, " \t\r\n\"", fields, 2) == 2)
    {
        if (size <= 0x12)
        {
            switch (size)
            {
            case 0:
                m_frameCount = (char)atoi(fields[1]);
                break;
            case 1:
                m_serverUdpPort = atoi(fields[1]);
                break;
            case 2:
                m_serverTcpPort = atoi(fields[1]);
                break;
            default:
                return 0;
            }
            return 1;
        }
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
    // [2026-09-14 修复] ORIG df_manager_r 0x08069bd3/0x08069bda 实证：pid 检查用的是
    //   `call access; **sete** al; test al,al; je <放行>` ⇒ **access==0（pid 文件存在）才抛**。
    //   旧转写写成 `!= 0`（不存在就抛）⇒ 极性反了，manager 在干净的 pid 目录下**永远起不来**
    //   （A/B 里表现为「There is already pid file!」后自杀）；cfg 检查那处 ORIG 是
    //   `setne`（0x08069ad9），本来就是「不存在才抛」，无需改。
    if (access(pid.c_str(), 0) == 0)
        throw CDNFException(
            "CAppConfig::Set_FileName() There is already pid file! Exception Break! Check ./pid/*.pid files");
    return 1;
}

int CAppConfig::Get_ServerUdpPort() { return m_serverUdpPort; }
int CAppConfig::Get_ServerTcpPort() { return m_serverTcpPort; }
unsigned int CAppConfig::Get_FrameCountValue() { return m_frameCount; }
