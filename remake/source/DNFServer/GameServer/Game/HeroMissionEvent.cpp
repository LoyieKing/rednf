#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 HeroMissionEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "HeroMissionEvent.h"

#include "CUser.h"
#include "LogManager.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "TaiwanInternal.h"      // Taiwan::internal_stream / TaiwanInternalPack::T
#include "TaiwanQueries.h"      // Taiwan::SigLoadHeroMissionData（init/GetInBuffer）
#include "GameTypes.h"          // cMyTrace（StartEvent/EndEvent trace）
#include "ClearDungeonMission.h"
#include "UseFatigueMission.h"
#include "UseCubeMission.h"
#include "LevelUpMission.h"
#include "AttendanceMission.h"
#include <boost/tokenizer.hpp> // char_separator / tokenizer（setEventData 切分）
#include "TotalMission.h"
#include <sstream>             // stringstream（getStringToken 拼串）
#include <string.h>            // strncpy（saveEventData 写回 SigSaveHeroMissionData）

#include "CItem.h"             // CItem::make_item（LevelUpMission 发奖虚调用 vptr+0x08）
#include "Inven_Item.h"        // Inven_Item / set_add_info
#include "CDataManager.h"      // G_CDataManager / find_item
#include "CMailBoxHelper.h"    // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "DNFLexWrapper.h"     // loadRDARScriptFile / ScanType / ScanInt / ScanStr
#include "CEnvironment.h"      // less_TCHAR / SpinLock / G_CEnvironment
#include "CSystemTime.h"       // CSystemTime::getCurSec（AttendanceMission 判活时刻）

#include "HeroMissionEvent/importHeroMissionConditionsData.inc"
                                    const char* path);

// ---- 显式实例化：ARAD::Singleton<HeroMissionEvent>::Get()（ORIG W 0x0811c9ce）----
namespace ARAD
{
template HeroMissionEvent* Singleton<HeroMissionEvent>::Get();
}

#include "HeroMissionEvent/HeroMissionEvent.inc"


#include "HeroMissionEvent/findMissionValue.inc"


#include "HeroMissionEvent/processMission.inc"


#include "HeroMissionEvent/loadEventData.inc"


#include "HeroMissionEvent/setMissionValue.inc"

#include "HeroMissionEvent/StartEvent.inc"


#include "HeroMissionEvent/StartEvent_1.inc"


#include "HeroMissionEvent/EndEvent.inc"


#include "HeroMissionEvent/TotalMission.inc"

#include "HeroMissionEvent/dtor_TotalMission.inc"
#include "HeroMissionEvent/processMission_1.inc"



#include "HeroMissionEvent/ClearDungeonMission.inc"

#include "HeroMissionEvent/dtor_ClearDungeonMission.inc"
#include "HeroMissionEvent/processMission_2.inc"


#include "HeroMissionEvent/rewardMission.inc"


#include "HeroMissionEvent/UseFatigueMission.inc"

#include "HeroMissionEvent/dtor_UseFatigueMission.inc"
#include "HeroMissionEvent/processMission_3.inc"


#include "HeroMissionEvent/rewardMission_1.inc"


#include "HeroMissionEvent/UseCubeMission.inc"

#include "HeroMissionEvent/dtor_UseCubeMission.inc"
#include "HeroMissionEvent/processMission_4.inc"


#include "HeroMissionEvent/rewardMission_2.inc"


#include "HeroMissionEvent/LevelUpMission.inc"

#include "HeroMissionEvent/dtor_LevelUpMission.inc"
#include "HeroMissionEvent/processMission_5.inc"


#include "HeroMissionEvent/rewardMission_3.inc"


#include "HeroMissionEvent/AttendanceMission.inc"

#include "HeroMissionEvent/dtor_AttendanceMission.inc"
#include "HeroMissionEvent/processMission_6.inc"
                                       unsigned int /*param*/)
{
    // [b28d-solo-HeroMissionEvent] ORIG 0x08166002 条件 1 为 `limit <= field4`
    // （cmp %ax,%dx; setbe; jne 跳尾 return）；tm 槽序 today=-0x40 浅于
    // target=-0x6c ⇒ 4.4「先声明浅」需 today 先声明。
    if (m_condition.m_extra[0] <= value.m_field4)
        return;
    {
        struct tm today;       // ORIG -0x40
        struct tm target;      // ORIG -0x6c
        time_t cur;            // ORIG -0x14
        time_t todaySec;       // ORIG -0x10
        time_t targetSec;      // ORIG -0xc
        cur = GlobalData::s_systemTime_.getCurSec();
        localtime_r(&cur, &today);
        localtime_r(&cur, &target);
        today.tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
        today.tm_min = 0;
        today.tm_sec = 0;
        todaySec = mktime(&today);
        target.tm_mon = m_condition.m_extra[1] - 1;
        target.tm_mday = m_condition.m_extra[2] + value.m_field8;
        target.tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
        target.tm_min = 0;
        target.tm_sec = 0;
        targetSec = mktime(&target);
        if (0x15180 < todaySec - targetSec)
        {
            value.reset();
        }
        // [b28d-solo-HeroMissionEvent] ORIG 0x0816613e 条件 3 为
        // `limit <= field4+1`（cmp %eax,%edx; setle; je 跳 else 累加块）。
        if (m_condition.m_extra[0] <= value.m_field4 + 1)
        {
            value.m_field4 = m_condition.m_extra[0];
        }
        else
        {
            value.m_field4 = value.m_field4 + 1;
        }
        value.m_field8 = value.m_field8 +
            static_cast<unsigned short>((todaySec - targetSec) / 0x15180);
    }
}

#include "HeroMissionEvent/rewardMission_4.inc"


// ============================================================================
// ConvertString2EnumMissionType —— ORIG t 0x89613d7（无报告，objdump 0x 转写）。
// [type] 标签的字符串枚举 → 整型（"normal"→1 / "payment"→2 / "final"→3）。
// 函数级 static 惰性表（lookup @0x94f7598 / SpinLock @0x94f75b0 / 就绪字节
// @0x94f75b4，双检锁 + 首轮 fallthrough 建表后重查，P21 范式）。
// [b28d-solo-HeroMissionEvent] 判词 AE：fresh clean ndiff=0（orig=213 our=213，
// gap=0）；清单 NEAR19 系旧读数。
// ============================================================================
static int ConvertString2EnumMissionType(std::string& s)
{
loop_top:
    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static bool __inited;
    int code;
    if (__inited)
    {
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            __lookup.find(s.c_str());
        code = (it != __lookup.end()) ? it->second : -1;
    }
    else
    {
        __init_lock.enter();
        if (__inited)
        {
            __init_lock.leave();
            goto loop_top;
        }
        code = 0;
    }

    switch (code)
    {
    case 0:
    case 0x3d1:   // "normal"
        if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("normal", 0x3d1));
        else
            return 1;
    case 0x3d3:   // "payment"
        if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("payment", 0x3d3));
        else
            return 2;
    case 0x3d5:   // "final"
        if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("final", 0x3d5));
        else
            return 3;
    }

    if (!__inited)
    {
        __init_lock.leave();
        __inited = true;
        goto loop_top;
    }
    return 0;
}

#include "HeroMissionEvent/importHeroMissionConditionsData_1.inc"


#include "HeroMissionEvent/loadScript.inc"

#include "HeroMissionEvent/setEventData.inc"

#include "HeroMissionEvent/getStringToken.inc"

#include "HeroMissionEvent/setEventData_1.inc"

#include "HeroMissionEvent/setMissionForGm.inc"

#include "HeroMissionEvent/resetMissionForGm.inc"

#include "HeroMissionEvent/saveEventData.inc"


