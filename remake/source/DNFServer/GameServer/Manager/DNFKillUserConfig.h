#ifndef DNF_KILL_USER_CONFIG_H_
#define DNF_KILL_USER_CONFIG_H_

#include <vector>

#include "DNFTableBase.h"

// ---- ST_KillUSRConfig：0x10 ----
class ST_KillUSRConfig
{
public:
    ST_KillUSRConfig();
    int m_type;    // +0
    int m_field4;  // +4
    int m_field8;  // +8
    int m_fieldC;  // +0xc
};

// ---- CKillUSRConfig ----
class CKillUSRConfig : public CTableBase
{
public:
    CKillUSRConfig();
    virtual ~CKillUSRConfig();
    virtual int Load_Table(const std::string& fileName);
    virtual int Parse_Table(char* data, int size);
    void Clear_Table();
    void* GetInfo() const;
    std::vector<ST_KillUSRConfig*> m_list;  // +4（sizeof=0x10）
};

#endif  // DNF_KILL_USER_CONFIG_H_
