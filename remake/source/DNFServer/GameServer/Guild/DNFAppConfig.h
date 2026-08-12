#ifndef DNFAPPCONFIG_H_
#define DNFAPPCONFIG_H_

#include <map>
#include <string>
#include <vector>

#include "PacketHeader.h"
#include "DNFTableBase.h"

class CAppConfig;
class stServerInfo;

// from GuildTable.h
class CAppConfig : public CTableBase
{
public:
    CAppConfig();
    virtual ~CAppConfig();
    virtual void Load_Table(const std::string& path);
    virtual bool Parse_Table(char* line, int idx);
    void Check_FileName(const std::string& filename);
    unsigned char Get_ServerGroup();
    unsigned short Get_FrameCountValue();
    unsigned short Get_ServerUdpPort();
    const char* Get_DBMWTcpIP();
    unsigned short Get_DBMWTcpPort();
    unsigned short Get_ServerTcpPort();
    std::multimap<unsigned int, stServerInfo*>* GetServerInfoMap();
    void clearServerInfoMap();

    unsigned char m_frameCount;   // +4
    unsigned char m_field5;       // +5
    unsigned short m_udpPort;     // +6
    unsigned short m_tcpPort;     // +8
    unsigned char m_group;        // +0xa
    char m_pad;                   // +0xb
    std::string m_name;           // +0xc
    unsigned short m_dbmwTcpPort; // +0x10
    char m_pad2[2];               // +0x12
    std::multimap<unsigned int, stServerInfo*> m_serverInfo;  // +0x14
};

#endif
