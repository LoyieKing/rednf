#ifndef DNF_SERVER_CONFIG_H_
#define DNF_SERVER_CONFIG_H_

#include <string>

#include "DNFTableBase.h"

// ---- ST_ServerInfo ----
struct ST_ServerInfo
{
    ST_ServerInfo();
    ~ST_ServerInfo();
    char m_index;      // +0
    char m_type;       // +1
    char m_pad2[2];
    std::string m_name;  // +4
    unsigned short m_port;  // +8
    char m_padA[2];
};

// ---- CServerConfig ----
class CServerConfig : public CTableBase
{
public:
    CServerConfig();
    virtual ~CServerConfig();
    virtual int Load_Table(const std::string& fileName);
    virtual int Parse_Table(char* data, int size);
    void* GetServerInfo();
    ST_ServerInfo m_servers[0x65];  // +4（101 × 0xc = 0x4bc；ORIG C1/D1 循环计数 101；sizeof=0x4c0）
};

#endif  // DNF_SERVER_CONFIG_H_
