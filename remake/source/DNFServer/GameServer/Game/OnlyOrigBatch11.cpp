// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 OnlyOrigBatch11/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// OnlyOrigBatch11.cpp —— df_game_r 重建终局冲刺 Batch11 独立符号集中落地。
//
// 逐符号依据 docs/class_func_reports/ 对应报告与 ORIG objdump 反汇编转写；
// 归属他 TU 的符号（WindowsPackFiles/ThreadInterface/StrikerScript/
// TimerDispatcher/UsedFatigueRewardItem/ScriptDepsA/STMapScript/…
// 见 /tmp/df_batches/OnlyOrigBatch11.md）不在此重复。
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

// ORIG T 0x08630b38：仅 vptr store + ret（P57；m_tid/m_tidCopy 不在此层初始化）。
// 定义必须与派生线程类 TU（GlobalData.cpp 等）分离：4.4 对同 TU 可见空体 ctor 做
// non-throw 推断，导致 GlobalData::Init 的 `new DispatchThread` 等块省略 ORIG 实有
// 的 delete+Unwind cleanup（0x0829a4f3 等 pad，实测 probe 确认）。
#include "OnlyOrigBatch11/ThreadInterface.inc"
#include "OnlyOrigBatch11/begin.inc"
}

// ORIG T 0x08630c1a：`mov 0x8(%ebp),%eax; mov %eax,-0xc`（self 具名局部有槽
// 往返，P0）→ `mov -0xc,%eax; mov (%eax),%eax; add $0xc,%eax; mov (%eax),%edx`
// （虚表槽 3）→ (esp)=self（从槽）、0x4=形参槽 → `call *%edx` → `mov $0,%eax`。
#include "OnlyOrigBatch11/dispatch_proxy.inc"
}

// ORIG T 0x08630c46：pthread_join(m_tid)，成功(rc==0)返回 1、失败返回 0。
#include "OnlyOrigBatch11/join.inc"
}

// ORIG W 0x082a4218（非 const）：读 +8（m_tidCopy）。
#include "OnlyOrigBatch11/getThreadID.inc"
}

// ============================================================================
// StrikerScript.h 域（头已改外联声明）
// ============================================================================

// ORIG T 0x08a9d7a6：m_skillIdx<=level<=m_value 区间判定。
#include "OnlyOrigBatch11/isPenaltyLevel.inc"
}

// ORIG T 0x08a9d81a：m_type == type。
// [b28d-D66 修法] P248 档②：字段真型是 enum T（不是 unsigned int）。字段 unsigned 时
// 4.4 产 `mov (%eax),%edx; mov 0xc(%ebp),%eax; cmp %eax,%edx`（DIFF 9）；StrikerScript.h
// 的 m_type 改 enum T 后 4.4 复现 ORIG 的 `mov (%eax),%eax; cmp 0xc(%ebp),%eax` 同型
// enum==enum mem 直比形（探针 /tmp/d66_probe4.cpp A/B 双结构 + setType 逐字节不变）。
#include "OnlyOrigBatch11/isType.inc"
}

// ============================================================================
// UPGRADEJAR
// ============================================================================

// ORIG T 0x089687cc：空实现。
#include "OnlyOrigBatch11/doNothing.inc"
}

// ============================================================================
// 包名计数 / 表名常量（直返）
// ============================================================================

// ORIG T 0x08083cd0：mov $0x233 直返。
#include "OnlyOrigBatch11/getNotiPacketNameCount.inc"
}

// ORIG T 0x08083cda：mov $0x25f 直返。
#include "OnlyOrigBatch11/getCmdPacketNameCount.inc"
}

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

// ORIG T 0x0846c34a：getPrevScheduleTime(hour, time + 0x15180)。
#include "OnlyOrigBatch11/getNextScheduleTime.inc"
}

// ORIG T 0x0889209d —— 逐段反汇编定语义（b28c-l23 修正旧"奇偶相异"判词）：
// ①`-0x8 = b + c`；`-0x2 = a & 1`；`-0x1 = sum & 1`（三条独立槽 store）；
// ②@+0x24 `cmpb $0,-0x2; je L1` + @+0x2a `cmpb $0,-0x1; jne Lret0`
//   ⇒ 第一段 = `(oddA == 0) || (oddSum == 0)`（与零比较 ⇒ 直分支 cmpb 形）；
// ③L1 @+0x30 `movzbl -0x2; xor $1; test; je Lret1` + @+0x3b 同族测 -0x1
//   ⇒ 第二段 = `(!oddA) || (!oddSum)`（bool 取反 ⇒ P168a X 族物化形）；
// ④两段的真出口共享 Lret1（mov $1），假出口共享 Lret0（mov $0）。
// 合并即"至少一个为偶 ⇒ 1；两者皆奇 ⇒ 0"（偶偶也返 1）。
#include "OnlyOrigBatch11/checkGetItemPacket.inc"
}

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
}

// ============================================================================
// 时间 / 随机 / 进程辅助（LinuxService 域）
// ============================================================================

// ORIG T 0x086b18d4：timeout>10 → 0；循环 timeout 次：*p 为真 → 1；
// 否则 sleep_ext(1,0)；超时返 0。
#include "OnlyOrigBatch11/WaitForSingleObject.inc"
}

// ORIG T 0x086b1a7e get_str_time：sep==0 → "%02d%02d%02d"（时/分/秒）；
// 否则 "%02d%c%02d%c%02d"（实参顺序 tm_hour(+0x8)/tm_min(+0x4)/tm_sec(+0x0)）。
#include "OnlyOrigBatch11/get_str_time.inc"
}

// ORIG T 0x086b1c7c：12 次 get_rand_int(100) 求和 → /100（截断）→
// 距中值 (n/2) 的偏移 * 0.95f 截断后 + n（0x3f733333=0.95f；原码久错 0.45f 已改）。
#include "OnlyOrigBatch11/get_regular_rand.inc"
}

// ORIG T 0x0829e8b9：argv[2]（argv+8 处指针）为 "start"/"start_all" 时
// fork 守护化（父退出、setsid、chdir("./")、umask(0)）；最后 save_pid()，
// 成功返 0、fork 失败返 -1。
#include "OnlyOrigBatch11/daemon_init.inc"
}

// ============================================================================
// 脚本文件路径 / 脚本值
// ============================================================================

// ORIG T 0x088bbbc5：vsnprintf 到临时缓冲（0x104 上限），再拼到
// g_szScriptFileBaseDirectory 后（ss_strcpy + ss_strcat）。
#include "OnlyOrigBatch11/getScriptFileFullPathFormat.inc"
}

// ORIG T 0x088bd984：vector 长度 <= idx+4 直接返回；否则
// out.type = (int)at(idx)；out.value[0..3] = at(idx+1+i)。
#include "OnlyOrigBatch11/getScriptValue.inc"
}

// ============================================================================
// 团队 / 菜单 / 状态串映射
// ============================================================================

// ORIG T 0x088c2d7c："character"→0、"neutral"→0xc8、"monster"→0x64、
// "all"→2、默认 0x64。
#include "OnlyOrigBatch11/getTeam.inc"
}

// ORIG T 0x088dcff5：函数局部 static map（guard @0x94f6b78 / map @0x94f6b80）
// 惰性构造；空时调 makeMenuActionHashMap；find(name) 命中返回 second，
// 未命中返回 0x1a。
#include "OnlyOrigBatch11/getMenuAction.inc"
}

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

// ORIG T 0x089aee71："[expUp]" → 1，否则 0。
#include "OnlyOrigBatch11/getStackableIncreaseStatusType.inc"
}

// ORIG T 0x089bcde0："silence"→1、"disease"→3、"mindcontrol"→2、"all"→5、
// 默认 -1。
#include "OnlyOrigBatch11/getExcepttionStatusToEnumValue.inc"
}

// ORIG T 0x089bce65（const string&）：
// "[reduce probability at pvp type module]"→1、
// "[reduce probability at pvp module]"→2、
// "[reduce probability to human armor]"→0、
// "[reduce probability to human armor at pvp type module]"→3、
// "[reduce probability to human armor at pvp module]"→4、默认 5。
#include "OnlyOrigBatch11/getRevisionToEnumValue.inc"
}

// ORIG T 0x089bcfed（switch 跳表 0x8e19c34）：
// 2→0、6→1、7→2、9→3、11→4、其余 -1。
#include "OnlyOrigBatch11/getActiveStatusDamageIndex.inc"
}

// ORIG T 0x089c2a81："jump attack"→0、否则 1。
#include "OnlyOrigBatch11/getEquipmentEventTiming.inc"
}


// ============================================================================
// convert_string_to_time / convertToUtf8
// ============================================================================

// ORIG T 0x08358208：std::string 构造 → boost::posix_time::time_from_string →
// to_tm → mktime 写入 *out。异常路径 Unwind（ORIG 有 try/catch 表）。
#include "OnlyOrigBatch11/convert_string_to_time.inc"
}

// ORIG T 0x08adf6e7：宽字符数组按 UTF-8 编码宽度累计（<=0x7f → 1 字节、
// <=0x7ff → 2 字节、其余 3 字节）；总长 0 → out="" 返 1；否则
// CharStringData::createTerminated + 逐字符编码写入 out。
// [batch-19 S→A] 原 S 档 missing-calls（C1EP14CharStringData/D1/aSERKS_）为
// 形错：ORIG 主路径构造**临时 CharString tmp(data)** 再 `*out = tmp`（拷贝
// 赋值 + 临时析构，含 EH 保护区），非 attachData 直挂；且两循环编码游标用
// `char* buf++` 指针自增、ch 为 signed int（`cmpl $0x7f; jg`）、首循环逐臂
// `++i`（while 形）、createTerminated 已含终止符无需 buf[pos]='\0'。订正后
// ndiff 103→11（残差 = 尾部 tmp 析构公共块布局 + 返回值寄存器调度）。
// [b28d-solo-OnlyOrigBatch11 收口] 旧 D66 的「P78 N 封顶」判词作废：把尾部
// `CharString tmp(data); *out = tmp; return true;` 的 tmp 收进**显式块**
// `{ CharString tmp(data); *out = tmp; } return true;` 后，`mov $1,%eax` 落到
// D1 之后（ORIG +0x1a0 形），4.4 不再把返回值提前物化到 %ebx ⇒ fndiff clean
// 135=135 **AE-EQUAL**（`movl $"",0x4(%esp)` 的 0x24b addend 为 P264 未链接伪差）。
#include "OnlyOrigBatch11/convertToUtf8.inc"
}


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
namespace boost
{
namespace gregorian
{
namespace
{
const char* const kLongMonthNames[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
const char* const kShortMonthNames[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
}  // namespace

#include "OnlyOrigBatch11/get_month_map_ptr.inc"
}
}  // namespace gregorian
}  // namespace boost

// [b29-SymForceBatch] StaticPool<expert_job::CEnchanter,300>::CNode D0 force 块
// 删除：forceCNodeD0 本身是发明符号（ORIG 无）。两池 CNode 的 C1/C2/D0/D1/D2 +
// vtable/typeinfo 已由真实宿主 GlobalData.cpp（CExpertJobMgr ctor →
// pool C1 → new CNode[N]）自然发射（实测 tu_GlobalData.o 全套在），无需本 TU
// 重复触发。
