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

// ORIG T 0x08630b38：仅 vptr store + ret（P57；m_tid/m_tidCopy 不在此层初始化）。
// 定义必须与派生线程类 TU（GlobalData.cpp 等）分离：4.4 对同 TU 可见空体 ctor 做
// non-throw 推断，导致 GlobalData::Init 的 `new DispatchThread` 等块省略 ORIG 实有
// 的 delete+Unwind cleanup（0x0829a4f3 等 pad，实测 probe 确认）。
#include "OnlyOrigBatch11/ThreadInterface.inc"

// ---- [b28c-l23 代落 C28L18 spec] begin / dispatch_proxy ------------------
// ORIG T 0x08630b8a（nm 实证强符号 ⇒ 外联，头内联只产 W/P119）：
// `lea 0x4(this),%edx` + (esp)=&m_tid / 0x4=NULL / 0x8=&dispatch_proxy /
// 0xc=this → call pthread_create → `mov %eax,-0xc` 具名槽往返 →
// `cmpl $0,-0xc; je OK`；失败块 cerr 四段（串 @0x08ce6620、" " @0x08ce664c、
// operator<<(int)(rc)、endl）；OK 块 `m_tidCopy = m_tid`（+8 ← +4）后
// `mov $1,%eax`（两路恒返 true）。
#include "OnlyOrigBatch11/begin.inc"
{
    int rc = pthread_create(&m_tid, NULL,
                            &ThreadInterface::dispatch_proxy, this);
    if (rc != 0)
    {
        // [Q7S2 2026-09-11] 串内容真错修正：ORIG .rodata @0x08ce6620 实测
        //   `[ThreadInterface::begin] Can't begin thread`（**无末尾句号**，
        //   objdump -s 直读，49 字节含 NUL）；原字面量多一个 '.'。
        std::cerr << "[ThreadInterface::begin] Can't begin thread" << " "
                  << rc << std::endl;
    }
    m_tidCopy = m_tid;
    return true;
}

// ORIG T 0x08630c1a：`mov 0x8(%ebp),%eax; mov %eax,-0xc`（self 具名局部有槽
// 往返，P0）→ `mov -0xc,%eax; mov (%eax),%eax; add $0xc,%eax; mov (%eax),%edx`
// （虚表槽 3）→ (esp)=self（从槽）、0x4=形参槽 → `call *%edx` → `mov $0,%eax`。
#include "OnlyOrigBatch11/dispatch_proxy.inc"
{
    ThreadInterface* self = static_cast<ThreadInterface*>(p);
    self->dispatch(p);
    return 0;
}

// ORIG T 0x08630c46：pthread_join(m_tid)，成功(rc==0)返回 1、失败返回 0。
#include "OnlyOrigBatch11/join.inc"
{
    if (pthread_join(m_tid, 0) != 0)
        return 0;
    return 1;
}

// ORIG W 0x082a4218（非 const）：读 +8（m_tidCopy）。
#include "OnlyOrigBatch11/getThreadID.inc"
{
    return m_tidCopy;
}

// ============================================================================
// StrikerScript.h 域（头已改外联声明）
// ============================================================================

// ORIG T 0x08a9d7a6：m_skillIdx<=level<=m_value 区间判定。
#include "OnlyOrigBatch11/isPenaltyLevel.inc"
{
    if (m_skillIdx <= level && level <= m_value)
        return true;
    return false;
}

// ORIG T 0x08a9d81a：m_type == type。
// [b28d-D66 修法] P248 档②：字段真型是 enum T（不是 unsigned int）。字段 unsigned 时
// 4.4 产 `mov (%eax),%edx; mov 0xc(%ebp),%eax; cmp %eax,%edx`（DIFF 9）；StrikerScript.h
// 的 m_type 改 enum T 后 4.4 复现 ORIG 的 `mov (%eax),%eax; cmp 0xc(%ebp),%eax` 同型
// enum==enum mem 直比形（探针 /tmp/d66_probe4.cpp A/B 双结构 + setType 逐字节不变）。
#include "OnlyOrigBatch11/isType.inc"
{
    if (m_type == type)
        return true;
    return false;
}

// ============================================================================
// UPGRADEJAR
// ============================================================================

// ORIG T 0x089687cc：空实现。
#include "OnlyOrigBatch11/doNothing.inc"
{
}

// ============================================================================
// 包名计数 / 表名常量（直返）
// ============================================================================

// ORIG T 0x08083cd0：mov $0x233 直返。
#include "OnlyOrigBatch11/getNotiPacketNameCount.inc"
{
    return 0x233;
}

// ORIG T 0x08083cda：mov $0x25f 直返。
#include "OnlyOrigBatch11/getCmdPacketNameCount.inc"
{
    return 0x25f;
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
{
    const int daySec = 0x15180;
    return getPrevScheduleTime(hour, time + daySec);
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
{
    const int sum = b + c;
    // [b28d-solo-OnlyOrigBatch11] 语义订正 + 形复现：
    // 真值表（ORIG 逐路径推演）：11→0、10→1、01→1、00→0，即 `oddA ^ oddSum`。
    // 旧实现 `!oddA || !oddSum` 在 00 情形返 1 ⇒ 真语义 bug（P196 族），已按 ORIG 改正。
    // 形：两槽必须是 **bool** 局部（P292 同源）——uchar 局部下 4.4 把取反折成
    // `cmpb $0;je` 直分支，bool 局部才发 ORIG 的 `movzbl; xor $1; test %al,%al; je`。
    // [LaneA 改形 · 依据 = ORIG 块图反推] ORIG 0x0889209d 的块图**不含任何 goto**：
    //   第一段 @+0x24 `cmpb $0,-0x2; je L1` + @+0x2a `cmpb $0,-0x1; jne Lret0`
    //     ⇒ 原码 `if (oddA) { if (oddSum) return 0; }`（A 假落 L1、B 真直落 ret0）；
    //   第二段 @+0x30/@+0x3b 两条 `xor $1; test; je Lret1`（**共享同一目标** +0x4d）
    //     ⇒ 原码 `if (!oddA) { if (!oddSum) return 0; } return 1;`
    //     （ret0 块 @+0x46 直落 ret1 块 @+0x4d）。
    // 旧 goto 形（`if (oddA&&oddSum) goto Lfail; … goto Lsucc;`）语义等价，但 4.4 会
    // 为 goto 目标多布 3 处 jmp/nop 中继岛（ORIG 0 岛）⇒ 换成上述**自然 if/else 形**。
    // 本形未经本泳道编译（纪律：禁止编译），由父泳道 fresh 复验裁定；若仍劣化则回退
    // 至旧 goto 形（探针 /tmp/b28d_s83_probe3.cpp h2 族，21 形读数见该探针）。
    const bool oddA = (a & 1) != 0;
    const bool oddSum = (sum & 1) != 0;
    if (oddA)
    {
        if (oddSum)
            return 0;
    }
    if (!oddA)
    {
        if (!oddSum)
            return 0;
    }
    return 1;
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
{
    // [b28d-solo-OnlyOrigBatch11] P0/P183：ORIG 无 sa/sb 具名槽、且第二段比较
    // **完整重算** a/b 两串字节（a 先 b 后）⇒ 两处独立内联表达式；比较用 int
    // （ORIG jge/jle 有符号序，unsigned 会退化为 jae）。探针 /tmp/b28d_s83_probe_cro.cpp
    // q7 与 ORIG 逐条全等（q1/q2 对照：unsigned 出 jae、第二段 b 先出 jge 反向）。
    if ((int)((const RandomOptionSortKeyView*)a)->m_key1 +
            (int)((const RandomOptionSortKeyView*)a)->m_key2 <
        (int)((const RandomOptionSortKeyView*)b)->m_key1 +
            (int)((const RandomOptionSortKeyView*)b)->m_key2)
        return 1;
    if ((int)((const RandomOptionSortKeyView*)a)->m_key1 +
            (int)((const RandomOptionSortKeyView*)a)->m_key2 >
        (int)((const RandomOptionSortKeyView*)b)->m_key1 +
            (int)((const RandomOptionSortKeyView*)b)->m_key2)
        return -1;
    return 0;
}

// ============================================================================
// 时间 / 随机 / 进程辅助（LinuxService 域）
// ============================================================================

// ORIG T 0x086b18d4：timeout>10 → 0；循环 timeout 次：*p 为真 → 1；
// 否则 sleep_ext(1,0)；超时返 0。
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

// ORIG T 0x086b1a7e get_str_time：sep==0 → "%02d%02d%02d"（时/分/秒）；
// 否则 "%02d%c%02d%c%02d"（实参顺序 tm_hour(+0x8)/tm_min(+0x4)/tm_sec(+0x0)）。
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

// ORIG T 0x086b1c7c：12 次 get_rand_int(100) 求和 → /100（截断）→
// 距中值 (n/2) 的偏移 * 0.95f 截断后 + n（0x3f733333=0.95f；原码久错 0.45f 已改）。
#include "OnlyOrigBatch11/get_regular_rand.inc"
{
    int rounds = 0xc;
    int sum = 0;
    int centered = 0;
    float factor = 0.95f;
    int i = 0;
    for (; i < rounds; ++i)
        sum += get_rand_int(100);
    sum /= 100;
    centered = sum - (rounds / 2);
    centered = static_cast<int>(static_cast<float>(centered) * factor);
    return centered + n;
}

// ORIG T 0x0829e8b9：argv[2]（argv+8 处指针）为 "start"/"start_all" 时
// fork 守护化（父退出、setsid、chdir("./")、umask(0)）；最后 save_pid()，
// 成功返 0、fork 失败返 -1。
#include "OnlyOrigBatch11/daemon_init.inc"
{
    const char* arg = argv[2];
    if (strcmp(arg, "start") == 0 || strcmp(arg, "start_all") == 0)
    {
        const int pid = fork();
        if (pid < 0)
            return -1;
        if (pid != 0)
            exit(0);
        setsid();
        chdir("./");
        umask(0);
    }
    save_pid();
    return 0;
}

// ============================================================================
// 脚本文件路径 / 脚本值
// ============================================================================

// ORIG T 0x088bbbc5：vsnprintf 到临时缓冲（0x104 上限），再拼到
// g_szScriptFileBaseDirectory 后（ss_strcpy + ss_strcat）。
#include "OnlyOrigBatch11/getScriptFileFullPathFormat.inc"
                                 const char* fmt, ...)
{
    char buf[0x104];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, 0x104, fmt, ap);
    va_end(ap);
    ss_strcpy(out, size, g_szScriptFileBaseDirectory);
    ss_strcat(out, size, buf);
}

// ORIG T 0x088bd984：vector 长度 <= idx+4 直接返回；否则
// out.type = (int)at(idx)；out.value[0..3] = at(idx+1+i)。
#include "OnlyOrigBatch11/getScriptValue.inc"
{
    if (static_cast<int>(vec.size()) <= idx + 4)
        return;
    out.type = static_cast<int>(vec.at(idx));
    for (int i = 0; i <= 3; ++i)
        out.value[i] = vec.at(idx + 1 + i);
}

// ============================================================================
// 团队 / 菜单 / 状态串映射
// ============================================================================

// ORIG T 0x088c2d7c："character"→0、"neutral"→0xc8、"monster"→0x64、
// "all"→2、默认 0x64。
#include "OnlyOrigBatch11/getTeam.inc"
{
    if (strcasecmp(name, "character") == 0)
        return 0;
    if (strcasecmp(name, "neutral") == 0)
        return 0xc8;
    if (strcasecmp(name, "monster") == 0)
        return 0x64;
    if (strcasecmp(name, "all") == 0)
        return 2;
    return 0x64;
}

// ORIG T 0x088dcff5：函数局部 static map（guard @0x94f6b78 / map @0x94f6b80）
// 惰性构造；空时调 makeMenuActionHashMap；find(name) 命中返回 second，
// 未命中返回 0x1a。
#include "OnlyOrigBatch11/getMenuAction.inc"
{
    static std::map<std::string, int> menuHashMap;
    if (menuHashMap.empty())
        makeMenuActionHashMap(menuHashMap);
    std::map<std::string, int>::const_iterator it =
        menuHashMap.find(std::string(name));
    if (it != menuHashMap.end())
        return it->second;
    return 0x1a;
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
{
    if (s == "[expUp]")
        return 1;
    return 0;
}

// ORIG T 0x089bcde0："silence"→1、"disease"→3、"mindcontrol"→2、"all"→5、
// 默认 -1。
#include "OnlyOrigBatch11/getExcepttionStatusToEnumValue.inc"
{
    if (s == "silence")
        return 1;
    if (s == "disease")
        return 3;
    if (s == "mindcontrol")
        return 2;
    if (s == "all")
        return 5;
    return -1;
}

// ORIG T 0x089bce65（const string&）：
// "[reduce probability at pvp type module]"→1、
// "[reduce probability at pvp module]"→2、
// "[reduce probability to human armor]"→0、
// "[reduce probability to human armor at pvp type module]"→3、
// "[reduce probability to human armor at pvp module]"→4、默认 5。
#include "OnlyOrigBatch11/getRevisionToEnumValue.inc"
{
    if (s == "[reduce probability at pvp type module]")
        return 1;
    if (s == "[reduce probability at pvp module]")
        return 2;
    if (s == "[reduce probability to human armor]")
        return 0;
    if (s == "[reduce probability to human armor at pvp type module]")
        return 3;
    if (s == "[reduce probability to human armor at pvp module]")
        return 4;
    return 5;
}

// ORIG T 0x089bcfed（switch 跳表 0x8e19c34）：
// 2→0、6→1、7→2、9→3、11→4、其余 -1。
#include "OnlyOrigBatch11/getActiveStatusDamageIndex.inc"
{
    int result = -1;
    switch (static_cast<int>(status))
    {
    case 2:  result = 0; break;
    case 6:  result = 1; break;
    case 7:  result = 2; break;
    case 9:  result = 3; break;
    case 11: result = 4; break;
    }
    return result;
}

// ORIG T 0x089c2a81："jump attack"→0、否则 1。
#include "OnlyOrigBatch11/getEquipmentEventTiming.inc"
{
    if (s == "jump attack")
        return 0;
    return 1;
}


// ============================================================================
// convert_string_to_time / convertToUtf8
// ============================================================================

// ORIG T 0x08358208：std::string 构造 → boost::posix_time::time_from_string →
// to_tm → mktime 写入 *out。异常路径 Unwind（ORIG 有 try/catch 表）。
#include "OnlyOrigBatch11/convert_string_to_time.inc"
{
    // [b28d-solo-OnlyOrigBatch11] 两处形订正（探针/报告 0x08358208）：
    // ①ORIG 在 time_from_string 返回后**立即** D1 析构 string（+0x047），随后才
    //   D1 析构临时 allocator（+0x078）⇒ 原码是 `time_from_string(std::string(str))`
    //   的**临时对象实参**，不是函数作用域具名 `std::string s(str)`（后者析构被
    //   推迟到函数末尾 EH 尾块，实测调用序整体错位）。
    // ②日志第 3 参 ORIG 为 `_ZZ22convert_string_to_timePKcRiE19__PRETTY_FUNCTION__`
    //   局部静态符号（P118/P139）⇒ 必须写 `__PRETTY_FUNCTION__`，手写字面量只生
    //   匿名 .rodata（实测 .o 立即数 0x398 即该伪差）。
    // 余差：catch 内 `e.what()` 间接虚调用目标寄存器 ORIG %edx vs 4.4.7 恒 %eax
    // （N7/P230 补：4.4.6 后端才用 %edx；本 TU 路由 4.4.7 后端不可切）⇒ N7 封顶。
    try
    {
        boost::posix_time::ptime pt =
            boost::posix_time::time_from_string(std::string(str));
        struct tm tmv = boost::posix_time::to_tm(pt);
        out = static_cast<int>(mktime(&tmv));
    }
    catch (const std::exception& e)
    {
        // ORIG catch 块：what() → logFormat(1, "data_manager.cpp",
        // "void convert_string_to_time(const char*, int&)", 0x1920,
        // "Exception: %s")
        LogManager::logFormat(1, "data_manager.cpp",
                              __PRETTY_FUNCTION__,
                              0x1920, "Exception: %s", e.what());
    }
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
{
    int need = 0;
    for (int i = 0; i < srcLength; )
    {
        const int ch = static_cast<int>(src[i]);
        if (ch <= 0x7f)
        {
            need += 1;
            ++i;
        }
        else if (ch <= 0x7ff)
        {
            need += 2;
            ++i;
        }
        else
        {
            need += 3;
            ++i;
        }
    }
    if (need == 0)
    {
        *out = "";
        return true;
    }
    CharStringData* data = CharStringData::createTerminated(need);
    char* buf = data->getBuffer();
    for (int i = 0; i < srcLength; ++i)
    {
        const int ch = static_cast<int>(src[i]);   // ORIG signed（主循环 `cmpl $0x7f; jg`）
        if (ch <= 0x7f)
        {
            *buf++ = static_cast<char>(ch);
        }
        else if (ch <= 0x7ff)
        {
            *buf++ = static_cast<char>(0xc0 | (ch >> 6));
            *buf++ = static_cast<char>(0x80 | (ch & 0x3f));
        }
        else
        {
            *buf++ = static_cast<char>(0xe0 | (ch >> 12));
            *buf++ = static_cast<char>(0x80 | ((ch >> 6) & 0x3f));
            *buf++ = static_cast<char>(0x80 | (ch & 0x3f));
        }
    }
    // ORIG 0x8adf75a..0x8adf861：构造临时 CharString tmp(data)（lea -0x28）→
    // *out = tmp（aSERKS_）→ 析构 tmp（D1，含 EH 保护区）；非 attachData
    // 直挂（那样缺 C1EP14CharStringData / aSERKS_ / D1 三个调用）。
    // [b28d-solo-OnlyOrigBatch11] tmp 用**显式块**限定生存期（块尾析构），使
    // `return true` 的 `mov $1,%eax` 落在 D1 之后（ORIG +0x1a0 形），避免 4.4
    // 把返回值提前物化到 %ebx 跨 D1 call 保活（P78）。
    {
        CharString tmp(data);
        *out = tmp;
    }
    return true;
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
{
    static month_map_ptr_type month_map_ptr(new month_map_type);
    // 2026-08-31：ORIG 0x87dcdfa 额外 new 16B 辅助对象（存全局 0x9491d4c，
    // vtable 0x8de0de0；本实现建表不用它——纯分配行为对齐）。
    static void* auxMonthObj = ::operator new(16);
    static bool initialized = false;
    (void)auxMonthObj;
    if (!initialized)
    {
        for (unsigned short i = 1; i <= 12; ++i)
        {
            std::string s(kLongMonthNames[i - 1]);
            std::string abbr(kShortMonthNames[i - 1]);
            for (std::string::iterator it = s.begin(); it != s.end(); ++it)
                *it = static_cast<char>(std::tolower(static_cast<unsigned char>(*it)));
            for (std::string::iterator it = abbr.begin(); it != abbr.end(); ++it)
                *it = static_cast<char>(std::tolower(static_cast<unsigned char>(*it)));
            (*month_map_ptr)[s] = i;
            (*month_map_ptr)[abbr] = i;
        }
        initialized = true;
    }
    return month_map_ptr;
}
}  // namespace gregorian
}  // namespace boost

// [b29-SymForceBatch] StaticPool<expert_job::CEnchanter,300>::CNode D0 force 块
// 删除：forceCNodeD0 本身是发明符号（ORIG 无）。两池 CNode 的 C1/C2/D0/D1/D2 +
// vtable/typeinfo 已由真实宿主 GlobalData.cpp（CExpertJobMgr ctor →
// pool C1 → new CNode[N]）自然发射（实测 tu_GlobalData.o 全套在），无需本 TU
// 重复触发。
