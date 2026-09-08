// df_game_r Game/ 工具函数实现（独立 TU，从 GameStubs.cpp 拆出）。
// 包含：trim / less_TCHAR / TimerEntry / SECOND_TO_MILISECOND / cMyTrace。
#include "CEnvironment.h"
#include "TimerQueue.h"
#include "CLog.h"
#include "GameBase.h"
#include "GlobalData.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <sys/time.h>
#include <sys/select.h>
#include <time.h>

// ---- trim（CEnvironment::parse 用，ORIG 全局函数 _Z4trimPc @0x0829f0ef；返回入参）----
// ORIG 语义：跳过 0x01..0x20 的 ASCII 控制字符（保留 >=0x80 高位字节），
// 子串拷贝到 0x400 栈缓冲后 strcpy 回写（反汇编 0x0829f0ef 逐指令转写）。
char* trim(char* s)
{
    // [b28d-34] 复核维持 DIFF：ORIG 0x0829f0ef 循环变量/条件（js 与 cmp 0x20 顺序）
    // 与 4.4 槽序镜像（P212/N98 族）；语义等价。
    char buf[0x400];
    int len = strlen(s);
    int start = 0;
    int i = 0;
    // ORIG do-while 形态：先存 start=i 再检查——跳出时 start 即第一个非空白
    // （我们原 while 先检后存，start 落最后一个空白——前导空白未被去除）。
    while (i < len)
    {
        start = i;
        if ((signed char)s[i] < 0 || s[i] > 0x20)
            break;
        ++i;
    }
    int end = len;
    i = len - 1;
    while (i >= 0 && end > start &&
           (signed char)s[i] >= 0 && s[i] <= 0x20)
    {
        --end;
        --i;
    }
    buf[end - start] = 0;
    memcpy(buf, s + start, end - start);
    strcpy(s, buf);
    return s;
}

// ---- trim(std::string&)（ORIG _Z4trimRSs @0x0829f068：字符集仅空格）----
void trim(std::string& s)
{
    // P0：ORIG 两调用结果复用同一槽 -0xc ⇒ 原码单变量赋值两次。
    size_t pos = s.find_first_not_of(" ", 0);
    if (pos != std::string::npos)
        s.erase(0, pos);
    pos = s.find_last_not_of(" ", std::string::npos);
    if (pos != std::string::npos)
        s.erase(pos + 1, std::string::npos);
}

// ---- less_TCHAR（CEnvironment map 比较器）----
bool less_TCHAR::operator()(const char* x, const char* y) const
{
    return strcmp(x, y) < 0;
}

// ---- SECOND_TO_MILISECOND（GameBase.h 声明）----
long long SECOND_TO_MILISECOND(int sec)
{
    return (long long)sec * 1000;
}

// ---- cMyTrace（GameTypes.h 声明，ORIG 0x854f718/0x854f746）----
// ORIG 3 参 0x854f718：m_field_c=1、m_field_d=0、m_name/m_line/m_flag 直存。
cMyTrace::cMyTrace(const char* name, int line, int flag)
{
    m_field_c = 1;
    m_field_d = 0;
    m_name = name;
    m_line = line;
    m_flag = flag;
}
// ORIG 5 参 0x854f746：反汇编实测 0x18(%ebp)=第 5 参(bFieldC)→+0x0d、
// 0x1c(%ebp)=第 6 参(bFieldD)→+0x0c——**字段与形参名交叉**（m_field_c 存的是
// bFieldD，m_field_d 存的是 bFieldC）。冒烟实测 logInit 的 bTimestamp 因此差
// 异（ORIG 输出行首是否带CRLF），已按ORIG实测映射修正。
cMyTrace::cMyTrace(const char* name, int line, int flag, bool bFieldC, bool bFieldD)
{
    m_name = name;
    m_line = line;
    m_flag = flag;
    // [b28c-35] 字段已 bool 化（GameTypes.h）⇒ 直赋（P211 同型 LHS-first）；旧
    // `(char)` cast 产值先形（ORIG 0x0854f746 尾 store this 先）。
    m_field_c = bFieldD;   // ORIG +0x0c ← 第 6 参 bFieldD
    m_field_d = bFieldC;   // ORIG +0x0d ← 第 5 参 bFieldC
}
void cMyTrace::operator()(const char* fmt, ...)
{
    // [b28d-34] 复核维持 DIFF：ORIG 0x0854f788 无 `cmpl $0,0xc(%ebp)` 前置，
    // 我们多 1 条且帧 0x1020→0x1030（P0 表达式直用族）；语义等价。
    // ORIG 0x854f788（0x207 字节）：0x1000 栈缓冲清零；level(m_flag) 非 8/9 时
    // sprintf("%s(%d): ", m_name, m_line) 前缀；vsnprintf 追加变参（上限 0xfff）；
    // Guard<Mutex>(&CLog::instance()->m_mutexes[m_flag*0x18]) 加锁后按 level 分发：
    // 0=logNotice 1=logError 2=logDebug 3=logConsole 4=logMoney 5=logCritical
    // 8=logSnapShot+logInit(m_field_c,m_field_d)；6/7/9 无操作。
    if (!fmt)
        return;
    char buf[0x1000];
    std::memset(buf, 0, sizeof(buf));
    if (m_flag != 8 && m_flag != 9)
        std::sprintf(buf, "%s(%d): ", m_name, m_line);
    size_t used = std::strlen(buf);
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf + used, 0xfff - used, fmt, ap);
    va_end(ap);

    CLog* log = CLog::instance();
    Mutex& mutex = *reinterpret_cast<Mutex*>(&log->m_mutexes[m_flag * 0x18]);
    {
        Guard<Mutex> guard(&mutex);
        switch (m_flag)
        {
        case 0: CLog::instance()->logNotice(buf); break;
        case 1: CLog::instance()->logError(buf); break;
        case 2: CLog::instance()->logDebug(buf); break;
        case 3: CLog::instance()->logConsole(buf); break;
        case 4: CLog::instance()->logMoney(buf); break;
        case 5: CLog::instance()->logCritical(buf); break;
        case 8:
            // ORIG case8 → logSnapShot 后无 break，落到 case9 共用唯一 logInit call site。
            CLog::instance()->logSnapShot(buf);
        case 9:
            CLog::instance()->logInit(m_field_d != 0, m_field_c != 0, buf);
            break;
        default:  // 6/7：ORIG 跳表指向收尾 0x854f95f，无日志输出
            break;
        }
    }
}

// ---- CheckDailyScheduleTime（每日调度时间检查，ORIG _Z22CheckDailyScheduleTimeill）----
// ORIG：localtime_r 拆 lCurTime，复制 tm 后置 nScheduleTime:00:00，mktime；
// 若原时刻小时 < nScheduleTime 则回退一天；返回 lLastPlayTime <= 该时刻。
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime, long lCurTime)
{
    time_t cur = lCurTime;
    struct tm tmv;
    localtime_r(&cur, &tmv);
    struct tm t2 = tmv;
    t2.tm_hour = nScheduleTime;
    t2.tm_min = 0;
    t2.tm_sec = 0;
    time_t t = mktime(&t2);
    if (tmv.tm_hour < nScheduleTime)
        t -= 86400;
    return lLastPlayTime <= t;
}

// ============================================================================
// 基础时间工具（ORIG-only 符号，被多个 TU 依赖；逐函数核对 ORIG 反汇编）
// ============================================================================

// ---- sleep_ext（ORIG _Z9sleep_extii，0x86b1b48，0x3f 字节）----
// ORIG：构造 {sec, usec} timeval 后 select(0,0,0,0,&tv)（sleep 的 select 实现）。
void sleep_ext(int sec, int usec)
{
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = usec;
    select(0, NULL, NULL, NULL, &tv);
}

// ---- getPrevScheduleTime（ORIG _Z19getPrevScheduleTimeil，0x846c2e9，0x61 字节）----
// ORIG：t = time - hour*3600；localtime_r；置 hour:00:00；mktime。
// 返回 time 当日 hour 时刻的时间戳（未到 hour 则属前一天）。
long getPrevScheduleTime(int hour, long time)
{
    const int oneHour = 3600;
    long tmp = (long)hour * oneHour;
    long t = time - tmp;
    struct tm tmv;
    localtime_r(&t, &tmv);
    tmv.tm_hour = hour;
    tmv.tm_min = 0;
    tmv.tm_sec = 0;
    t = mktime(&tmv);
    return t;
}

// ---- getDailyInitializeTime（ORIG _Z22getDailyInitializeTimeiii，0x86b1f5a，0x66 字节）----
// ORIG：取 CSystemTime::getCurSec()，localtime_r，置 hour:min:sec，mktime；
// 若结果晚于当前时刻则回退一天（返回当日该时刻，已过则昨日）。
int getDailyInitializeTime(int hour, int min, int sec)
{
    time_t cur = GlobalData::s_systemTime_.getCurSec();
    struct tm tmv;
    struct tm* p = localtime_r(&cur, &tmv);
    p->tm_hour = hour;
    p->tm_min = min;
    p->tm_sec = sec;
    time_t ret = mktime(p);
    if (ret > cur)
        ret -= 86400;
    return (int)ret;
}

// ---- __GetCurrentDNFDate（ORIG _Z19__GetCurrentDNFDateR2tm，0x86b1fc0，0x5f 字节）----
// ORIG：cur = getCurSec()；若 localtime 后 tm_hour < CEnvironment+0x37c
// （m_serverEnvir.m_fatigueTime，即每日调度小时）则回退一天再 localtime。
void __GetCurrentDNFDate(tm& out)
{
    time_t cur = GlobalData::s_systemTime_.getCurSec();
    int daily = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // +0x37c
    localtime_r(&cur, &out);
    if (out.tm_hour < daily)
    {
        cur -= 86400;
        localtime_r(&cur, &out);
    }
}

// ---- ComputeDaysScheduleTime（ORIG _Z23ComputeDaysScheduleTimeill，0x846c144，0x1a5 字节）----
// ORIG：分别以 curTime/lastTime 求当日 hour:00:00 基准时刻（未到 hour 回退一天），
// 差值 /86400 取整天数，负数归 0。
int ComputeDaysScheduleTime(int hour, long lastTime, long curTime)
{
    time_t c = curTime;
    struct tm tmc;
    localtime_r(&c, &tmc);
    struct tm t1 = tmc;
    t1.tm_hour = hour;
    t1.tm_min = 0;
    t1.tm_sec = 0;
    time_t r1 = mktime(&t1);
    if (tmc.tm_hour < hour)
        r1 -= 86400;

    time_t l = lastTime;
    struct tm tml;
    localtime_r(&l, &tml);
    struct tm t2 = tml;
    t2.tm_hour = hour;
    t2.tm_min = 0;
    t2.tm_sec = 0;
    time_t r2 = mktime(&t2);
    if (tml.tm_hour < hour)
        r2 -= 86400;

    int days = (int)(r1 - r2) / 86400;
    if (days < 0)
        days = 0;
    return days;
}

// ---- GetRandomRange（ORIG W 0x08539d90 _Z14GetRandomRangeii）----
// a<b: rand()%(b-a+1)+a；b<a: rand()%(a-b+1)+b（C 有符号取模语义）。
int GetRandomRange(int a, int b)
{
    if (b < a)
        return (int)((rand() % (a - b + 1)) + b);
    return (int)((rand() % (b - a + 1)) + a);
}
