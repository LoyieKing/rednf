// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GameUtils/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
#include <iostream>   // std::ios_base::Init __ioinit（ORIG GI 键名 _Z22CheckDailyScheduleTimeill 的前提，P338）
#include <sys/time.h>
#include <sys/select.h>
#include <time.h>

// ---- CheckDailyScheduleTime（每日调度时间检查，ORIG _Z22CheckDailyScheduleTimeill）----
// [P327 GI 键名] ORIG 本 TU `_GLOBAL__I_` 键名 = _Z22CheckDailyScheduleTimeill ⇒ 该
// 函数定义须为本 TU 首个强符号（此前 trim 定义在前 ⇒ 键名错为 _GLOBAL__I__Z4trimPc）。
// 纯前置定义顺序（函数体/符号集合/构造序零变化）。
// ORIG：localtime_r 拆 lCurTime，复制 tm 后置 nScheduleTime:00:00，mktime；
// 若原时刻小时 < nScheduleTime 则回退一天；返回 lLastPlayTime <= 该时刻。
#include "GameUtils/CheckDailyScheduleTime.inc"
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

// ---- trim（CEnvironment::parse 用，ORIG 全局函数 _Z4trimPc @0x0829f0ef；返回入参）----
// ORIG 语义：跳过 0x01..0x20 的 ASCII 控制字符（保留 >=0x80 高位字节），
// 子串拷贝到 0x400 栈缓冲后 strcpy 回写（反汇编 0x0829f0ef 逐指令转写）。
char* trim(char* s)
{
    // [T2CryptoUtils 闭合] 原 [b28d-34] 的「槽序镜像」残差已消：声明序
    // len→i→start + `i` 无声明处初值后置赋值 ⇒ 87/87 全等（AE，ndiff 0）。
    char buf[0x400];
    int len = strlen(s);
    // [T2CryptoUtils P331/P339 槽序] ORIG 0x0829f0ef 槽：len@-0x18(最深)/i@-0x14/
    // start@-0x10(最浅) ⇒ 4.4 声明序须为 len→i→start（先声明=深槽），且 ORIG 两条
    // 零初始化 store 序为 `-0x10 先 / -0x14 后`（start 先）⇒ i 不能带声明处初值，
    // 须 `int i;` + 后置 `i = 0;`。此形 87/87 全等（ndiff 0）。
    int i;
    int start = 0;
    i = 0;
    // ORIG 0x0829f11c..0x0829f134 条件序：先 `cmp $0x20,%al; jg 出口`，
    // 再 `test %al,%al; js 出口`（短路链先 >0x20 后 <0）；跳出时 start 即第一个非空白。
    while (i < len)
    {
        start = i;
        if (s[i] > 0x20 || (signed char)s[i] < 0)
            break;
        ++i;
    }
    int end = len;
    i = len - 1;
    // ORIG 0x0829f15e..0x0829f19c：循环头两条边界（i>=0 && end>start），
    // 体内两条独立 break（先 `cmp $0x20; jg`，后 `test; js`），尾部 --end/--i。
    while (i >= 0 && end > start)
    {
        if (s[i] > 0x20)
            break;
        if ((signed char)s[i] < 0)
            break;
        --end;
        --i;
    }
    buf[end - start] = 0;
    memcpy(buf, s + start, end - start);
    strcpy(s, buf);
    return s;
}

// ---- trim(std::string&)（ORIG _Z4trimRSs @0x0829f068：字符集仅空格）----
#include "GameUtils/trim.inc"
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
#include "GameUtils/operator.inc"
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
#include "GameUtils/cMyTrace.inc"
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
#include "GameUtils/cMyTrace_1.inc"
{
    m_name = name;
    m_line = line;
    m_flag = flag;
    // [b28c-35] 字段已 bool 化（GameTypes.h）⇒ 直赋（P211 同型 LHS-first）；旧
    // `(char)` cast 产值先形（ORIG 0x0854f746 尾 store this 先）。
    m_field_c = bFieldD;   // ORIG +0x0c ← 第 6 参 bFieldD
    m_field_d = bFieldC;   // ORIG +0x0d ← 第 5 参 bFieldC
}
#include "GameUtils/operator_1.inc"
{
    // [T2CryptoUtils 复核] 原 [b28d-34] 的「多 1 条 + 帧 0x1020→0x1030」残差已消
    // （va_start 前置于 strlen + `int used` ⇒ 137/137，gap 0）；残余 diff_lines=16
    // 仅尾部 `0xfff-used` 的寄存器选择/`sub mem` 折叠（N4/N7 分配族，封顶）。
    // [L13-round3 P171 修正] 该防御分支 ORIG 完全不存在（0x0854f788 首条即
    // `movl $0x1000,0x8(%esp)`）⇒ 按 P171「ORIG 不写即删」删除 `if(!fmt)return;`：
    // 消 `cmpl $0,0xc(%ebp); je` 2 条 + 帧 0x1030→0x1020 + buf 槽 -0x101c→-0x1014。
    // 注：原码对 fmt==NULL 不做检查（照抄 ORIG 越界/空指针行为）。
    // ORIG 0x854f788（0x207 字节）：0x1000 栈缓冲清零；level(m_flag) 非 8/9 时
    // sprintf("%s(%d): ", m_name, m_line) 前缀；vsnprintf 追加变参（上限 0xfff）；
    // Guard<Mutex>(&CLog::instance()->m_mutexes[m_flag*0x18]) 加锁后按 level 分发：
    // 0=logNotice 1=logError 2=logDebug 3=logConsole 4=logMoney 5=logCritical
    // 8=logSnapShot+logInit(m_field_c,m_field_d)；6/7/9 无操作。
    char buf[0x1000];
    std::memset(buf, 0, sizeof(buf));
    if (m_flag != 8 && m_flag != 9)
        std::sprintf(buf, "%s(%d): ", m_name, m_line);
    // [T2CryptoUtils codegen+P339] ORIG 0x0854f7f0 的 va_start（`lea 0x10(%ebp),%eax;
    // mov %eax,-0x10(%ebp)`）落在 strlen **之前** ⇒ 源码须 va_start 先于 used 计算；
    // 且 used 须为 int：size_t 版使 4.4 对 `0xfff - used` 发
    // `mov $0xfff,%eax; sub -0xc(%ebp),%eax; mov %eax,%edx`（常量物化+mem 直用，
    // 136 条），int 版复现 ORIG `mov -0xc(%ebp),%eax; mov $0xfff,%ecx; sub %eax,%ecx`
    // 形态（137/137，槽 -0xc 一致；strlen ≤ 0x1000 故转换无溢出）。
    va_list ap;
    va_start(ap, fmt);
    int used = (int)std::strlen(buf);
    std::vsnprintf(buf + used, 0xfff - used, fmt, ap);
    va_end(ap);

    // [L13-round3 P0/P183] ORIG 浅槽仅 ap@-0x10 / used@-0xc / guard@-0x14，
    // **无 log/mutex 具名槽**（`CLog::instance()` 与 mutex 取址在每个使用点内联）；
    // 我方两具名局部占 -0x18/-0x1c 并整体下移 8 字节。改为 guard 实参内联。
    {
        Guard<Mutex> guard(reinterpret_cast<Mutex*>(
            // [T2CryptoUtils 语义轴③] 下标必须是 m_flag：ORIG 0x0854f8a0 段仅
            // `mov 0x8(%eax),%edx; mov %edx,%eax; add %eax,%eax; add %edx,%eax;
            //  shl $0x3,%eax`（=m_flag*24，即 m_mutexes[m_flag] 的 sizeof 换算）；
            // 旧码 `m_flag*0x18` 被再乘一次 sizeof(Mutex)=24 ⇒ 地址 m_flag*576，
            // 越界写坏 CLog 之后的堆内存（m_flag=9 时偏 0x1440）。同 TU 对照：
            // HistoryLog.cpp:334 与 LwServerWriter.cpp:38 均直用下标记法。
            &CLog::instance()->m_mutexes[m_flag]));
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

// ============================================================================
// 基础时间工具（ORIG-only 符号，被多个 TU 依赖；逐函数核对 ORIG 反汇编）
// ============================================================================

// ---- sleep_ext（ORIG _Z9sleep_extii，0x86b1b48，0x3f 字节）----
// ORIG：构造 {sec, usec} timeval 后 select(0,0,0,0,&tv)（sleep 的 select 实现）。
#include "GameUtils/sleep_ext.inc"
{
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = usec;
    select(0, NULL, NULL, NULL, &tv);
}

// ---- getPrevScheduleTime（ORIG _Z19getPrevScheduleTimeil，0x846c2e9，0x61 字节）----
// ORIG：t = time - hour*3600；localtime_r；置 hour:00:00；mktime。
// 返回 time 当日 hour 时刻的时间戳（未到 hour 则属前一天）。
#include "GameUtils/getPrevScheduleTime.inc"
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
#include "GameUtils/getDailyInitializeTime.inc"
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
#include "GameUtils/__GetCurrentDNFDate.inc"
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
#include "GameUtils/ComputeDaysScheduleTime.inc"
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
#include "GameUtils/GetRandomRange.inc"
{
    if (b < a)
        return (int)((rand() % (a - b + 1)) + b);
    return (int)((rand() % (b - a + 1)) + a);
}
