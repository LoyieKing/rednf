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
// [P377/RTTI 轴] ORIG 的 `_ZTI14CKillUSRConfig` 为 24B（__vmi_class_type_info）
//   ⇒ CTableBase 是非 public 基类（我方原写 public ⇒ 退化为 12B __si_class_type_info）。
class CKillUSRConfig : private CTableBase
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
