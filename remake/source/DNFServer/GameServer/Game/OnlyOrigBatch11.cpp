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
ThreadInterface::ThreadInterface() {}

// ORIG T 0x08630c46：pthread_join(m_tid)，返回 rc!=0。
int ThreadInterface::join()
{
    int rc = pthread_join(m_tid, 0);
    return rc != 0;
}

// ORIG W 0x082a4218（非 const）：读 +8（m_tidCopy）。
unsigned long ThreadInterface::getThreadID()
{
    return m_tidCopy;
}

// ============================================================================
// StrikerScript.h 域（头已改外联声明）
// ============================================================================

// ORIG T 0x08a9d7a6：m_skillIdx<=level<=m_value 区间判定。
bool TagSkillPenalty::isPenaltyLevel(int level)
{
    if (level < m_skillIdx || m_value < level)
        return false;
    return true;
}

// ORIG T 0x08a9d81a：m_type == type。
bool StrikerDirectionType::isType(T type)
{
    if (m_type == static_cast<int>(type))
        return true;
    return false;
}

// ============================================================================
// UPGRADEJAR
// ============================================================================

// ORIG T 0x089687cc：空实现。
void UPGRADEJAR::doNothing()
{
}

// ============================================================================
// 包名计数 / 表名常量（直返）
// ============================================================================

// ORIG T 0x08083cd0：mov $0x233 直返。
int getNotiPacketNameCount()
{
    return 0x233;
}

// ORIG T 0x08083cda：mov $0x25f 直返。
int getCmdPacketNameCount()
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
long getNextScheduleTime(int hour, long time)
{
    const int daySec = 0x15180;
    return getPrevScheduleTime(hour, time + daySec);
}

// ORIG T 0x0889209d：返回 1 当 (a&1)==0 或 ((b+c)&1)==0；二者皆奇数返回 0。
int checkGetItemPacket(int a, int b, int c)
{
    const int sum = b + c;
    const bool oddA = (a & 1) != 0;
    const bool oddSum = (sum & 1) != 0;
    if (oddA && oddSum)
        return 0;
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
int _CompareRandomOption(const void* a, const void* b)
{
    const unsigned int sa =
        static_cast<unsigned char>(((const char*)a)[1]) +
        static_cast<unsigned char>(((const char*)a)[2]);
    const unsigned int sb =
        static_cast<unsigned char>(((const char*)b)[1]) +
        static_cast<unsigned char>(((const char*)b)[2]);
    if (sa < sb)
        return 1;
    if (sb < sa)
        return -1;
    return 0;
}

// ============================================================================
// 时间 / 随机 / 进程辅助（LinuxService 域）
// ============================================================================

// ORIG T 0x086b18d4：timeout>10 → 0；循环 timeout 次：*p 为真 → 1；
// 否则 sleep_ext(1,0)；超时返 0。
int WaitForSingleObject(bool* p, int timeout)
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
void get_str_time(char* out, int c, char sep)
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
// 距中值 (n/2) 的偏移 + 0.45f 截断后 + n。
int get_regular_rand(int n)
{
    const int rounds = 0xc;
    int sum = 0;
    for (int i = 0; i < rounds; ++i)
        sum += get_rand_int(100);
    sum /= 100;
    int centered = sum - (rounds / 2);
    int v = static_cast<int>(static_cast<float>(centered) * 0.45f);
    return v + n;
}

// ORIG T 0x0829e8b9：argv[2]（argv+8 处指针）为 "start"/"start_all" 时
// fork 守护化（父退出、setsid、chdir("./")、umask(0)）；最后 save_pid()，
// 成功返 0、fork 失败返 -1。
int daemon_init(char** argv)
{
    const char* arg = argv[2];
    if (strcmp(arg, "start") != 0 && strcmp(arg, "start_all") != 0)
    {
        save_pid();
        return 0;
    }
    const int pid = fork();
    if (pid < 0)
        return -1;
    if (pid > 0)
        exit(0);
    setsid();
    chdir("./");
    umask(0);
    save_pid();
    return 0;
}

// ============================================================================
// 脚本文件路径 / 脚本值
// ============================================================================

// ORIG T 0x088bbbc5：vsnprintf 到临时缓冲（0x104 上限），再拼到
// g_szScriptFileBaseDirectory 后（ss_strcpy + ss_strcat）。
void getScriptFileFullPathFormat(char* out, unsigned int size,
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
void getScriptValue(ScriptValue& out, std::vector<float>& vec, int idx)
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
int getTeam(const char* name)
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
int getMenuAction(const char* name)
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
int getStackableIncreaseStatusType(std::string& s)
{
    if (s == "[expUp]")
        return 1;
    return 0;
}

// ORIG T 0x089bcde0："silence"→1、"disease"→3、"mindcontrol"→2、"all"→5、
// 默认 -1。
int getExcepttionStatusToEnumValue(std::string& s)
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
int getRevisionToEnumValue(const std::string& s)
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
int getActiveStatusDamageIndex(ENUM_ACTIVESTATUS status)
{
    switch (static_cast<int>(status))
    {
    case 2:  return 0;
    case 6:  return 1;
    case 7:  return 2;
    case 9:  return 3;
    case 11: return 4;
    default: return -1;
    }
}

// ORIG T 0x089c2a81："jump attack"→0、否则 1。
int getEquipmentEventTiming(const std::string& s)
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
void convert_string_to_time(const char* str, int& out)
{
    std::string s(str);
    try
    {
        boost::posix_time::ptime pt =
            boost::posix_time::time_from_string(s);
        struct tm tmv = boost::posix_time::to_tm(pt);
        out = static_cast<int>(mktime(&tmv));
    }
    catch (const std::exception& e)
    {
        // ORIG catch 块：what() → logFormat(1, "data_manager.cpp",
        // "void convert_string_to_time(const char*, int&)", 0x1920,
        // "Exception: %s")
        LogManager::logFormat(1, "data_manager.cpp",
                              "void convert_string_to_time(const char*, int&)",
                              0x1920, "Exception: %s", e.what());
    }
}

// ORIG T 0x08adf6e7：宽字符数组按 UTF-8 编码宽度累计（<=0x7f → 1 字节、
// <=0x7ff → 2 字节、其余 3 字节）；总长 0 → out="" 返 1；否则
// CharStringData::createTerminated + 逐字符编码写入 out。
bool convertToUtf8(const wchar_t* src, int srcLength, CharString* out)
{
    int need = 0;
    for (int i = 0; i < srcLength; ++i)
    {
        const int ch = static_cast<int>(src[i]);
        if (ch <= 0x7f)
            need += 1;
        else if (ch <= 0x7ff)
            need += 2;
        else
            need += 3;
    }
    if (need == 0)
    {
        *out = "";
        return true;
    }
    CharStringData* data = CharStringData::createTerminated(need);
    char* buf = data->getBuffer();
    int pos = 0;
    for (int i = 0; i < srcLength; ++i)
    {
        const unsigned int ch = static_cast<unsigned int>(src[i]);
        if (ch <= 0x7f)
        {
            buf[pos++] = static_cast<char>(ch);
        }
        else if (ch <= 0x7ff)
        {
            buf[pos++] = static_cast<char>(0xc0 | (ch >> 6));
            buf[pos++] = static_cast<char>(0x80 | (ch & 0x3f));
        }
        else
        {
            buf[pos++] = static_cast<char>(0xe0 | (ch >> 12));
            buf[pos++] = static_cast<char>(0x80 | ((ch >> 6) & 0x3f));
            buf[pos++] = static_cast<char>(0x80 | (ch & 0x3f));
        }
    }
    buf[pos] = '\0';
    out->attachData(data);
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

greg_month::month_map_ptr_type greg_month::get_month_map_ptr()
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

// ============================================================================
// StaticPool<expert_job::CEnchanter,300>::CNode D0 force（P147）：
// 显式 new/delete 触发 D0 发射（ORIG W 0x0849fe04）。
// ============================================================================
#include "CExpertJob.h"
namespace expert_job {
namespace {
void forceCNodeD0()
{
    StaticPool<CEnchanter, 300>::CNode* n = new StaticPool<CEnchanter, 300>::CNode;
    delete n;
    // OnlyOrigBatch14：CDisjointer 池 CNode D0（ORIG W 0x084a00a8）。
    StaticPool<CDisjointer, 600>::CNode* m = new StaticPool<CDisjointer, 600>::CNode;
    delete m;
}
}  // namespace
}  // namespace expert_job
