// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CEventScriptMng/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ EventClassify::CEventScriptMng 实现。
#include "CEventScriptMng.h"

#include "CUser.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "PacketGuard.h"
#include "DB_SelectLevelUpReward.h"   // process_level_up_reward 的 DB 确认请求
#include "GameWorld.h"
#include "CDataManager.h"
#include "DB_UpdateInGameEventHistory.h"
#include "InGameEventScripts.h"
#include <ctime>
#include <cstring>

#include "SyncScriptDeps.h"          // toTString

#include "CEventScriptMng/CEventEntity.inc"


// InGameEventInfo / InGameEventInfoList 为全局作用域类型（见 CEventScriptMng.h），
// 故其方法定义位于任何命名空间之外。

// ============ InGameEventInfo ============

#include "CEventScriptMng/InGameEventInfo.inc"


#include "CEventScriptMng/dtor_InGameEventInfo.inc"


#include "CEventScriptMng/InGameEventInfo_1.inc"


// ORIG _ZN15InGameEventInfoaSERKS_：无自赋值检查，逐字段复制。
InGameEventInfo& InGameEventInfo::operator=(const InGameEventInfo& o)
{
    m_eventId = o.m_eventId;
    m_field4 = o.m_field4;
    m_name = o.m_name;
    m_startTime = o.m_startTime;
    m_endTime = o.m_endTime;
    m_isEventing = o.m_isEventing;
    return *this;
}

// ============ InGameEventInfoList ============

#include "CEventScriptMng/InGameEventInfoList.inc"

// [SplitC 2026-09-11 batch2 · TU 迁移] insertEventInfo 已迁出至 RDARScriptInGameEvent.cpp（ORIG GI 键对齐）。



// ============ InGameEventScriptLoader ============


#include "CEventScriptMng/InGameEventScriptLoader.inc"

#include "CEventScriptMng/insertEventScript.inc"


#include "CEventScriptMng/getEventScript.inc"
                                                           const std::string& name)
{
    std::map<std::string, InGameEventScript*>::iterator it =
        m_scripts.find(name);
    if (it != m_scripts.end())
        return it->second;

    return insertEventScript(name, importInGameEventScript(id, name));
}

namespace EventClassify
{

// ============ CEventEntity ============

#include "CEventScriptMng/dtor_CEventEntity.inc"


#include "CEventScriptMng/is_eventing.inc"


#include "CEventScriptMng/set_event_flag.inc"


#include "CEventScriptMng/set_event_data.inc"


#include "CEventScriptMng/get_event_record_type.inc"


#include "CEventScriptMng/set_event_record_type.inc"


#include "CEventScriptMng/send_init_data.inc"


#include "CEventScriptMng/get_db_table_name.inc"


#include "CEventScriptMng/get_event_start_time.inc"


#include "CEventScriptMng/set_event_start_time.inc"


#include "CEventScriptMng/get_event_end_time.inc"


#include "CEventScriptMng/set_event_end_time.inc"


// ============ CEventScriptMng ============

#include "CEventScriptMng/CEventScriptMng.inc"


#include "CEventScriptMng/dtor_CEventScriptMng.inc"


#include "CEventScriptMng/is_eventing_1.inc"


#include "CEventScriptMng/process_add_item_count_db.inc"


#include "CEventScriptMng/get_db_table_name_1.inc"


#include "CEventScriptMng/process_account_first_login.inc"


#include "CEventScriptMng/process_add_fatigue_count.inc"


#include "CEventScriptMng/process_add_quest_count_db.inc"


#include "CEventScriptMng/process_level_up_reward_recv_level.inc"


#include "CEventScriptMng/inc_user_fatigue.inc"


#include "CEventScriptMng/send_gift_fatigue_item.inc"


#include "CEventScriptMng/update_purchase_target_item_count.inc"


#include "CEventScriptMng/send_purchase_bonus_mail.inc"


#include "CEventScriptMng/send_bring_up_item_fatigue.inc"


#include "CEventScriptMng/send_pc_room_play_init_time.inc"


#include "CEventScriptMng/send_event_init_data.inc"


#include "CEventScriptMng/get_convert_to_user_confirmflag.inc"


#include "CEventScriptMng/get_event_entity.inc"


#include "CEventScriptMng/is_event_entity.inc"


#include "CEventScriptMng/get_record_type.inc"


#include "CEventScriptMng/get_today_time.inc"


#include "CEventScriptMng/get_today_time_day.inc"


#include "CEventScriptMng/is_current_ingame_event_record_type.inc"
{
    std::map<int, InGameEventInfo>::iterator it = m_info.find(key);
    if (it != m_info.end())
        return true;
    return false;
}

#include "CEventScriptMng/get_ingame_event_record_type_list.inc"
{
    out.clear();
    for (std::map<int, InGameEventInfo>::iterator it = m_info.begin(),
             endIt = m_info.end();
         it != endIt; ++it)
        out.push_back(it->first);
}

#include "CEventScriptMng/get_pass_day.inc"
{
    if (yyyymmdd1 < 1 && yyyymmdd2 < 1)
        return -1;

    // ORIG 0x810c974：{0} 初始化产生 rep stos 内联（P4），非 call memset。
    struct tm t1 = {0};
    t1.tm_year = yyyymmdd1 / 10000 - 1900;
    t1.tm_mon  = (yyyymmdd1 % 10000) / 100 - 1;
    t1.tm_mday = yyyymmdd1 % 100;
    t1.tm_hour = 0;
    time_t v1 = std::mktime(&t1);

    struct tm t2 = {0};
    t2.tm_year = yyyymmdd2 / 10000 - 1900;
    t2.tm_mon  = (yyyymmdd2 % 10000) / 100 - 1;
    t2.tm_mday = yyyymmdd2 % 100;
    t2.tm_hour = 0;
    time_t v2 = std::mktime(&t2);

    if (v2 >= v1)
        return static_cast<int>((v2 - v1) / 0x15180);
    return -1;
}

#include "CEventScriptMng/update_time_values.inc"
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    // ORIG 对 now 与 prev 各独立调一次 getCurSec()（两次 call），
    // prev = getCurSec() - 0x15180，不复用 now 结果。
    time_t prev = GlobalData::s_systemTime_.getCurSec() - 0x15180;
    struct tm localT, prevT;
    // ORIG 保存 localtime_r 返回的指针局部（-0x14=&localT / -0x10=&prevT），
    // 后续经指针成员访问（==&localT/+0x8 tm_hour 等），非每次 lea 重求址。
    struct tm* localTp = localtime_r(&now, &localT);
    struct tm* prevTp = localtime_r(&prev, &prevT);

    // ORIG 0x0810c628：`movl $0x0,-0xc(%ebp)` 死局部（此后从不被读；P232 死槽
    // 占帧，使 now/prev/localT/prevT/指针组整体下移 4，槽位与 ORIG 全等）。
    int deadFlag = 0;

    // ORIG 0x0810c62f：两条守卫（js/jge）同落**出线**的 localTp 块，prevTp 块为
    // fall-through（4.1 的 `||` 条件 then 块外置）⇒ 4.4 复现形为德摩根正向包裹
    // （P222/P272）：条件取反、两块互换，语义与 ORIG 完全一致。
    if (localTp->tm_hour >= 0 && localTp->tm_hour < hour)
    {
        m_todayTimeDay = static_cast<unsigned char>(prevTp->tm_mday);
        m_todayTime = prevTp->tm_year * 10000 + (prevTp->tm_mon + 1) * 100
                      + 19000000 + static_cast<unsigned char>(m_todayTimeDay);
    }
    else
    {
        m_todayTimeDay = static_cast<unsigned char>(localTp->tm_mday);
        m_todayTime = localTp->tm_year * 10000 + (localTp->tm_mon + 1) * 100
                      + 19000000 + static_cast<unsigned char>(m_todayTimeDay);
    }
}

#include "CEventScriptMng/init_event_nums.inc"


#include "CEventScriptMng/init_evnet_entity.inc"


#include "CEventScriptMng/initScriptlist.inc"



#include "CEventScriptMng/process_level_up_reward.inc"


}  // namespace EventClassify
