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
// [2026-09-15 RTTI 轴] ORIG 的 `_ZTI14CKillUSRConfig` = 0x18 (24B) 即 __vmi_class_type_info
//   ⇒ CTableBase 在 ORIG 中是非 public 基类；我方 public 会退化为 __si_class_type_info (12B)。
//   实测 ORIG: _ZTI14CKillUSRConfig 0x18 / _ZTI10CTableBase 0x8（四服务同源）。
class CKillUSRConfig : private CTableBase
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
