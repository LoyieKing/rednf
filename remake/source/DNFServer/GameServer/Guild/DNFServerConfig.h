#ifndef DNFSERVERCONFIG_H_
#define DNFSERVERCONFIG_H_

#include <map>
#include <string>
#include <vector>

#include "PacketHeader.h"
#include "DNFServerConfig.h"
#include "DNFTableBase.h"

class CServerConfig;
struct ST_ServerInfo;
class stServerInfo;

// from GuildTable.h
struct stServerInfo
{
    unsigned char m_group;     // +0
    unsigned char m_field1;    // +1
    unsigned char m_field2;    // +2
    char m_name[0x10];         // +3
    unsigned short m_port;     // +0x14
};

// from GuildTable.h
struct ST_ServerInfo
{
    ST_ServerInfo();
    unsigned char m_field0;    // +0
    unsigned char m_field1;    // +1
    unsigned char m_field2;    // +2（0xff）
    std::string m_string;      // +4
    unsigned short m_ushort;   // +8
};

// from GuildTable.h
class CServerConfig : public CTableBase
{
public:
    CServerConfig();
    virtual ~CServerConfig();
    virtual void Load_Table(const std::string& path);
    virtual int Parse_Table(char* line, int idx);
    ST_ServerInfo* GetServerInfo();
    ST_ServerInfo m_info[255];   // +4
};

#endif
