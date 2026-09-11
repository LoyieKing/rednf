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

#include "GameUtils/trim.inc"


#include "GameUtils/operator.inc"


// ---- SECOND_TO_MILISECOND（GameBase.h 声明）----
long long SECOND_TO_MILISECOND(int sec)
{
    return (long long)sec * 1000;
}

#include "GameUtils/cMyTrace.inc"

#include "GameUtils/cMyTrace_1.inc"

#include "GameUtils/operator_1.inc"


// ============================================================================
// 基础时间工具（ORIG-only 符号，被多个 TU 依赖；逐函数核对 ORIG 反汇编）
// ============================================================================

#include "GameUtils/sleep_ext.inc"
{
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = usec;
    select(0, NULL, NULL, NULL, &tv);
}

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

#include "GameUtils/GetRandomRange.inc"

