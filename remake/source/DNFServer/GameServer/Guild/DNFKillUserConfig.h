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
    int m_type;    // +0（DNFApplication switch 分支）
    int m_param1;  // +4
    int m_param2;  // +8
    int m_param3;  // +0xc
};

// from GuildTable.h
class CKillUSRConfig : public CTableBase
{
public:
    CKillUSRConfig();
    virtual ~CKillUSRConfig();
    virtual void Load_Table(const std::string& path);
    virtual bool Parse_Table(char* line, int idx);
    void Clear_Table();
    const std::vector<ST_KillUSRConfig*>* GetInfo() const;
    std::vector<ST_KillUSRConfig*> m_infos;  // +4
};

#endif
