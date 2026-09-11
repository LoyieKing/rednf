// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OnlyOrigBatch11/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <pthread.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/stat.h>   // umask
#include <unistd.h>     // fork/setsid/chdir
#include <map>
#include <string>
#include <vector>

#include <strings.h>      // strcasecmp

#include "ThreadInterface.h"
#include "StrikerScript.h"   // TagSkillPenalty / StrikerDirectionType
#include "UPGRADEJAR.h"
#include "GlobalData.h"      // s_systemTime_（get_str_time）
#include "GameRand.h"        // get_rand_int
#include "ScriptValue.h"     // ScriptValue
#include "StringResource.h"  // CStringManager::getStr（getAvatarRentDayString）
#include "Quest.h"           // ENUM_CHARACTERJOB
#include "LogManager.h"     // convert_string_to_time catch 日志
#include "STStackableScript.h"   // ENUM_MODULE_TYPE（DataManagerInitDepends.h 依赖）
#include "DataManagerInitDepends.h"  // ENUM_ACTIVESTATUS / RandomOptionScript 视图
#include "CDataManager.h"    // G_CDataManager + m_randomOption（_GetRandomOption）
#include "StringTypes.h"   // CharString / CharStringData（convertToUtf8）
#include <boost/date_time/posix_time/posix_time.hpp>  // convert_string_to_time

// ---- 跨 TU 符号（ORIG 地址随注）----
extern void makeMenuActionHashMap(std::map<std::string, int>& menuMap); // T 0x088dcf25（OnlyOrigBatch12.cpp）
extern void sleep_ext(int sec, int usec);                               // T 0x086b1b48（GameUtils.cpp）
extern long getPrevScheduleTime(int hour, long time);                   // T 0x0846c2e9（GameUtils.cpp）
extern int ss_strcpy(char* dest, size_t count, const char* src);        // T 0x08ad463f（SecureStdio）
extern int ss_strcat(char* dest, size_t count, const char* src);        // T 0x08ad46dd
extern char g_szScriptFileBaseDirectory[0x104];                         // B 0x949b180（ScriptExtras.cpp）
extern void save_pid();                                                 // ORIG 0x0829eeb2（同 TU）

// ============================================================================
// ThreadInterface（头 ThreadInterface.h 已改外联声明）
// ============================================================================

#include "OnlyOrigBatch11/ThreadInterface.inc"

#include "OnlyOrigBatch11/begin.inc"


#include "OnlyOrigBatch11/dispatch_proxy.inc"


#include "OnlyOrigBatch11/join.inc"


#include "OnlyOrigBatch11/getThreadID.inc"


// ============================================================================
// StrikerScript.h 域（头已改外联声明）
// ============================================================================

#include "OnlyOrigBatch11/isPenaltyLevel.inc"


#include "OnlyOrigBatch11/isType.inc"


// ============================================================================
// UPGRADEJAR
// ============================================================================

// [SplitC 2026-09-11 · TU 迁移] UPGRADEJAR::doNothing 已迁出至 RDARScriptEventUpgradeJar.cpp（ORIG GI 键对齐）。



// ============================================================================
// 包名计数 / 表名常量（直返）
// ============================================================================

#include "OnlyOrigBatch11/getNotiPacketNameCount.inc"


#include "OnlyOrigBatch11/getCmdPacketNameCount.inc"


// ORIG T 0x083fbe11：mov $0x8c366b5（"event_1003_pcroom_item"）。
const char* get_pcroom_item_table_name()
{
    return "event_1003_pcroom_item";
}

// ORIG T 0x083fbe1b：mov $0x8c366cc（"event_1107_pcroom_fatigue"）。
const char* get_pcroom_fatigue_table_name()
{
    return "event_1107_pcroom_fatigue";
}

// ============================================================================
// 职业转换 / 调度时间 / 物品包校验
// ============================================================================

// ORIG T 0x088946f7：9→0、10→3、其余原样返回。
ENUM_CHARACTERJOB convertNewCharacterJob(ENUM_CHARACTERJOB job)
{
    if (job == static_cast<ENUM_CHARACTERJOB>(9))
        return static_cast<ENUM_CHARACTERJOB>(0);
    if (job == static_cast<ENUM_CHARACTERJOB>(10))
        return static_cast<ENUM_CHARACTERJOB>(3);
    return job;
}

#include "OnlyOrigBatch11/getNextScheduleTime.inc"


#include "OnlyOrigBatch11/checkGetItemPacket.inc"


// ============================================================================
// 随机选项域
// ============================================================================

// ORIG T 0x085f8484：返回 &G_CDataManager()->m_randomOption（+0x4e10）。
// ORIG 反编译 C 返回 int（指针）；重建按成员地址返回。
RandomOptionScript& _GetRandomOption()
{
    return G_CDataManager()->m_randomOption;
}

// ORIG T 0x085f2c4a：按 +1/+2 两字节和排序（小于 1、大于 -1、等于 0）。
namespace
{
// [b28d-solo-OnlyOrigBatch11] ORIG 用 `movzbl 0x1(%eax),%eax` / `movzbl 0x2(%eax),%eax`
// 的 **disp 形**逐字段重读 ⇒ 原码是具名结构视图（P80/P82），不是 `((char*)a)[k]`
// （后者 4.4 恒发 `add $k,%eax; movzbl (%eax)` 两步，实测旧版多 8 条 + 栈帧）。
struct RandomOptionSortKeyView
{
    char m_pad0;
    unsigned char m_key1;
    unsigned char m_key2;
};
}  // namespace

#include "OnlyOrigBatch11/_CompareRandomOption.inc"


// ============================================================================
// 时间 / 随机 / 进程辅助（LinuxService 域）
// ============================================================================

#include "OnlyOrigBatch11/WaitForSingleObject.inc"
{
    if (timeout > 10)
        return 0;
    for (int i = 0; i < timeout; ++i)
    {
        if (*p)
            return 1;
        sleep_ext(1, 0);
    }
    return 0;
}

#include "OnlyOrigBatch11/get_str_time.inc"
{
    time_t curSec = GlobalData::s_systemTime_.getCurSec();
    struct tm tmv;
    struct tm* ptm = localtime_r(&curSec, &tmv);
    // ORIG 0x086b1ac7/0x086b1b01：栈实参 0x8(%esp)=tm_hour(+0x8)、
    // 0xc/0x10(%esp)=tm_min(+0x4)、末位=tm_sec(+0x0)，即 时→分→秒 顺序
    // （汇编取值顺序与压栈槽位相反，此前按取值顺序写反）。
    if (sep == 0)
    {
        sprintf(out, "%02d%02d%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    }
    else
    {
        sprintf(out, "%02d%c%02d%c%02d", ptm->tm_hour, sep, ptm->tm_min, sep,
                ptm->tm_sec);
    }
}

#include "OnlyOrigBatch11/get_regular_rand.inc"


#include "OnlyOrigBatch11/daemon_init.inc"


// ============================================================================
// 脚本文件路径 / 脚本值
// ============================================================================

#include "OnlyOrigBatch11/getScriptFileFullPathFormat.inc"


#include "OnlyOrigBatch11/getScriptValue.inc"


// ============================================================================
// 团队 / 菜单 / 状态串映射
// ============================================================================

#include "OnlyOrigBatch11/getTeam.inc"


#include "OnlyOrigBatch11/getMenuAction.inc"


// ORIG T 0x0898c9d2：1→getStr(0xdde)、2→getStr(0xddf)、0/3/4→getStr(0xde0)、
// 默认空串。
const char* getAvatarRentDayString(int n)
{
    if (n == 1)
        return CStringManager::getStr(0xdde);
    if (n == 2)
        return CStringManager::getStr(0xddf);
    if (n == 0 || n == 3)
        return CStringManager::getStr(0xde0);
    if (n == 4)
        return CStringManager::getStr(0xde0);
    return "";
}

#include "OnlyOrigBatch11/getStackableIncreaseStatusType.inc"


#include "OnlyOrigBatch11/getExcepttionStatusToEnumValue.inc"


#include "OnlyOrigBatch11/getRevisionToEnumValue.inc"


#include "OnlyOrigBatch11/getActiveStatusDamageIndex.inc"


#include "OnlyOrigBatch11/getEquipmentEventTiming.inc"



// ============================================================================
// convert_string_to_time / convertToUtf8
// ============================================================================

#include "OnlyOrigBatch11/convert_string_to_time.inc"


#include "OnlyOrigBatch11/convertToUtf8.inc"



// （base64_encode/base64_decode/is_base64 + static std::string base64_chars
//  已按 ORIG TU 归属证据迁至 PacketTypes.cpp：__static_init 注册序列 +
//  _GLOBAL__I__Z13base64_encodePKhjRSs 键名 + 0x0807e9f8..0x0807ef30 地址紧邻
//  域。本 TU 不留副本，避免重复动态注册项与 _ZL12base64_chars 双占位。）

// ============================================================================
// boost::gregorian::greg_month::get_month_map_ptr —— ORIG T 0x087dcd60
// （boost 库内部：静态 shared_ptr<month_map_type> 守卫构造；月名小写化建表）。
// 语义同 boost date_time greg_month.cpp 标准实现（P148 语义相等；被
// month_str_to_ushort 模板实例化引用，无实现则链接失败）。
// ============================================================================
// [SplitC 2026-09-11 · TU 迁移] boost::gregorian::greg_month::get_month_map_ptr
//   + 月名常量表（匿名 namespace）已整体迁出至 greg_month.cpp
//   （ORIG GI 键 _GLOBAL__I__ZN5boost9gregorian10greg_month17get_month_map_ptrEv 对齐）。

// [b29-SymForceBatch] StaticPool<expert_job::CEnchanter,300>::CNode D0 force 块
// 删除：forceCNodeD0 本身是发明符号（ORIG 无）。两池 CNode 的 C1/C2/D0/D1/D2 +
// vtable/typeinfo 已由真实宿主 GlobalData.cpp（CExpertJobMgr ctor →
// pool C1 → new CNode[N]）自然发射（实测 tu_GlobalData.o 全套在），无需本 TU
// 重复触发。
