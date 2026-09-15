#ifndef DBMW_DNFKILLUSERCONFIG_H_
#define DBMW_DNFKILLUSERCONFIG_H_

#include "DBMWCommon.h"


class ST_KillUSRConfig
{
public:
    ST_KillUSRConfig();
    int m_type;    // +0
    int m_field4;  // +4
    int m_field8;  // +8
    int m_fieldC;  // +0xc
};

class CKillUSRConfig : private CTableBase
{
public:
    CKillUSRConfig();
    virtual ~CKillUSRConfig();
    virtual void Load_Table(const std::string& fileName);
    virtual bool Parse_Table(char* data, int size);
    void Clear_Table();
    void* GetInfo() const;
    std::vector<ST_KillUSRConfig*> m_list;  // +4（sizeof=0x10）
};

#endif  // DBMW_DNFKILLUSERCONFIG_H_
