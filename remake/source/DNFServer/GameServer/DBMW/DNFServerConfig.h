#ifndef DBMW_DNFSERVERCONFIG_H_
#define DBMW_DNFSERVERCONFIG_H_

#include "DBMWCommon.h"


class CServerConfig : public CTableBase
{
public:
    CServerConfig();
    virtual ~CServerConfig();
    virtual int Load_Table(const std::string& fileName);
    virtual bool Parse_Table(char* data, int size);
    void* GetServerInfo();
    ST_ServerInfo m_servers[0xff];  // +4（255 × 0xc = 0xbf4，至 0xbf8）
};

#endif  // DBMW_DNFSERVERCONFIG_H_
