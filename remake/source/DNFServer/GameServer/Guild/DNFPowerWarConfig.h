#ifndef DNFPOWERWARCONFIG_H_
#define DNFPOWERWARCONFIG_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"
#include "DNFTableBase.h"
#include "PowerWar.h"

class CPowerWarConfig;

// from GuildDomain.h
// [P377/RTTI 轴] ORIG 的 `_ZTI15CPowerWarConfig` 为 24B `__vmi_class_type_info`，
//   解码 .rodata @0x811b8c8：__flags=0、__base_count=1、__base_type=&_ZTI10CTableBase、
//   __offset_flags=0 ⇒ 非 virtual + 非 public 基类（我方原写 public ⇒ 12B `__si_...`）。
class CPowerWarConfig : private CTableBase
{
public:
    CPowerWarConfig();
    virtual ~CPowerWarConfig();
    virtual void Load_Table(const std::string& path);
    virtual bool Parse_Table(char* line, int idx);
    void Clear_Table();
    ST_PowerWarEventStartTimeConfig* GetInfo() const;
    ST_PowerWarEventStartTimeConfig m_info;  // +4
};

#endif
