#ifndef DNFKILLUSERCONFIG_H_
#define DNFKILLUSERCONFIG_H_

#include <map>
#include <string>
#include <vector>

#include "PacketHeader.h"
#include "DNFTableBase.h"

class CKillUSRConfig;
struct ST_KillUSRConfig;

// from GuildTable.h
struct ST_KillUSRConfig
{
    ST_KillUSRConfig();
    int m_field0;
    int m_field1;
    int m_field2;
    int m_field3;
};

// from GuildTable.h
class CKillUSRConfig : public CTableBase
{
public:
    CKillUSRConfig();
    virtual ~CKillUSRConfig();
    virtual void Load_Table(const std::string& path);
    virtual int Parse_Table(char* line, int idx);
    void Clear_Table();
    std::vector<ST_KillUSRConfig*>* GetInfo() const;
    std::vector<ST_KillUSRConfig*> m_infos;  // +4
};

#endif
