#ifndef DNF_KILL_USER_CONFIG_H_
#define DNF_KILL_USER_CONFIG_H_

#include <string>
#include <vector>

#include "DNFTableBase.h"

// ---- ST_KillUSRConfig：4 int ----
struct ST_KillUSRConfig
{
    ST_KillUSRConfig();
    int m_field0;
    int m_field1;
    int m_field2;
    int m_field3;
};

// ---- CKillUSRConfig：CTableBase + vector<ST_KillUSRConfig*>@4（0x10）----
class CKillUSRConfig : public CTableBase
{
public:
    CKillUSRConfig();
    virtual ~CKillUSRConfig();
    virtual void Load_Table(const std::string& path);
    virtual bool Parse_Table(char* line, int idx);
    void Clear_Table();
    std::vector<ST_KillUSRConfig*>* GetInfo() const;
    std::vector<ST_KillUSRConfig*> m_infos;  // +4
};

#endif // DNF_KILL_USER_CONFIG_H_
