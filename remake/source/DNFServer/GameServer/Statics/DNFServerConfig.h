#ifndef DNF_SERVER_CONFIG_H_
#define DNF_SERVER_CONFIG_H_

#include <string>

#include "DNFTableBase.h"

// ST_ServerInfo：0x0 有效标志 / 0x1 服务器类型 / 0x2 服务器索引(0xff 无效)
//               + std::string@4 + ushort@8（总 0xc）
struct ST_ServerInfo
{
    ST_ServerInfo();
    ~ST_ServerInfo();
    char m_field0;           // +0（1=游戏服务器 / 2=DB / 4=Manager）
    char m_field1;           // +1
    char m_field2;           // +2（服务器索引）
    std::string m_string;    // +4
    unsigned short m_ushort; // +8
};

// CServerConfig：CTableBase + ST_ServerInfo[0xff]@4（总 0xbf8）
class CServerConfig : public CTableBase
{
public:
    CServerConfig();
    virtual ~CServerConfig();
    virtual bool Parse_Table(char* line, int idx);
    void Load_Table(const std::string& path);
    ST_ServerInfo* GetServerInfo();
    ST_ServerInfo m_servers[0xff];  // +4
};

#endif // DNF_SERVER_CONFIG_H_
