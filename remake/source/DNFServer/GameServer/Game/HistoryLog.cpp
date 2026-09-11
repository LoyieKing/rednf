// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 HistoryLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "CGameManager.h"
#include "HistoryLog.h"
#include <cstdarg>

#include "CLog.h"
#include "GameTypes.h"       // Guard<Mutex> / Mutex（0x18）
#include "cHistoryTrace.h"

// [L32 GI 调序] ORIG `_GLOBAL__I__ZN10HistoryLog9logfiles_E`（CU=HistoryLog.cpp）
//   ⇒ 本 TU 首个强符号须为 HistoryLog::logfiles_。整块类静态定义前移到所有函数定义
//   之前；logfiles_ 仍是本 TU 唯一动态初始化对象（count_/time 为 POD），
//   相对构造序不变（P327）。
StaticPool<_IO_FILE, 50> HistoryLog::logfiles_;
int HistoryLog::count_;
char HistoryLog::time[0x10];

// ---- 外部符号声明（对应 TU 翻译后移除） ----
#include "HistoryLog/get_str_datetime.inc"
#include "HistoryLog/get_str_date.inc"
char* NumberToString(unsigned int value, int radix);
#include "HistoryLog/get_str_date_1.inc"
{
    (void)len;
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm value;
    // ORIG：使用 localtime_r 的返回值指针访问字段。
    struct tm* pValue = localtime_r(&now, &value);
    // ORIG 0x086b19f1：rodata 0x8cf5a7c = "%04d%02d%02d"（无分隔符；
    // 相邻串 0x8cf5a89 与之相距 0xd 字节，长度自洽）。
    if (sep == 0)
        sprintf(buf, "%04d%02d%02d", pValue->tm_year + 1900, pValue->tm_mon + 1, pValue->tm_mday);
    else
        sprintf(buf, "%04d%c%02d%c%02d", pValue->tm_year + 1900, sep,
                pValue->tm_mon + 1, sep, pValue->tm_mday);
}
#include "HistoryLog/get_str_datetime_1.inc"
{
    (void)len;
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm value;
    struct tm* pValue = localtime_r(&now, &value);
    // ORIG 0x086b199e：rodata 0x8cf5a63 = "%04d%02d%02d%02d%02d%02d"
    // （与 0x8cf5a7c 相距 0x19 = 24+1 字节，确证无分隔符）。
    sprintf(buf, "%04d%02d%02d%02d%02d%02d", pValue->tm_year + 1900,
            pValue->tm_mon + 1, pValue->tm_mday, pValue->tm_hour,
            pValue->tm_min, pValue->tm_sec);
}

// ---- 格式串（ORIG rodata 逐字节） ----
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。
// [symname-seg34] 删发明符号 static const char FMT_*[]（ORIG 0x08c3e1xx 为无名 .rodata，
// 逐字节相同）⇒ 使用点已内联为字面量。

// ----------------------------------------------------------------------------
// HistoryLog 类声明见 HistoryLog.h（唯一声明点）
// ----------------------------------------------------------------------------

// [L32 GI 调序] 类静态定义已前移到文件首（须保持 ORIG 的 GI 键名），此处不再重复定义。

#include "HistoryLog/Init.inc"
{
    struct stat st;
    if (stat("./history/", &st) < 0)
    {
        if (mkdir("./history/", 0x1fd) < 0)
        {
            printf("Fail To Make Directory, PATH[%s]\n", "./history/");
            return false;
        }
    }
    return true;
}

#include "HistoryLog/CheckFD.inc"
{
    if (file == 0)
    {
        return false;
    }
    get_str_datetime(time, 0x10);
    return true;
}

#include "HistoryLog/LogClose.inc"
{
    if (file == 0)
        return;
    fclose(file);
    logfiles_.Free(file);
    count_ -= 1;
}

// [b28d-19] N：N82 —— 帧/槽序（对象临时物化差）；ORIG 0x084b9b32。
#include "HistoryLog/LogOpen.inc"
{
    // [P1 round5] ORIG 0x084b9bxx：`call rand; mov %eax,%ecx`（**不落槽**，%ecx 直接
    //   承接取模序列）⇒ 原码为单条初始化式 `rand() % 100`；旧稿两句式（`int rnd =
    //   rand(); rnd = rnd % 100;`）多出 -0x14 槽 store/load 对。
    int rnd = rand() % 100;
    // [P1 round5] 控制流轴：ORIG 0x084b9b71 `jle 0x084b9cf6`（跳到**函数末尾**的
    //   `mov $0x0,%eax`）⇒ 原码是「外层 if(rnd > 0x5f){ 主体 } + 末尾单一 return 0」，
    //   非早返 `if (rnd <= 0x5f) return 0;`（后者 4.4 产 `jg + mov $0 + jmp`，
    //   并把 `mov $0x0,%eax` 挪到跳转岛内）。主体整体下移入 if 块，语义不变。
    if (rnd > 0x5f)
    {
    FILE* file = logfiles_.Acquire();
    if (file == 0)
    {
        return 0;
    }


    // [b28d-round1-L24] P202①/P214③：ORIG 0x084b9b32 的清零是 `movl $0` ×2 +
    // `movw $0`（widthscan：clr_l orig=2 our=0 / clr_w orig=1 our=0 / clr_b orig=0
    // our=10）⇒ 原码是数组初始化式，非 10 条逐字节赋值。
    char date[10] = { '\0' };
    get_str_date(date, 10, '\0');

    char dir[50] = { '\0' };
    sprintf(dir, "%s%s/", "./history/", date);

    struct stat st;
    if (stat(dir, &st) < 0)
    {
        mkdir(dir, 0x1fd);
    }

    char path[127] = { '\0' };
    // [P1 round5] P0：ORIG 0x084b9c9d `call NumberToString; mov %eax,0xc(%esp)`
    //   直接以内联结果作 sprintf 第 4 实参，**无 num 具名槽**；旧稿 `char* num`
    //   占 -0xc 槽 ⇒ 本函数全部局部槽整体下移 4（date -0x1a→-0x1e / dir -0x4c→
    //   -0x50 / path -0x123→-0x127 / file -0xc→-0x10）。
    sprintf(path, "%s%s.log", dir, NumberToString(chNo, 0));


    file = fopen(path, "a");
    if (file == 0)
    {
        return 0;
    }
    count_ += 1;
    return file;
    }
    return 0;
}

#include "HistoryLog/WriteCreateCh.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\304\263\270\257\273\275\274\272\072\045\163\040\301\367\276"
    "\367\072\045\144\015\012", time, name, job);
}


#include "HistoryLog/WriteDeleteCh.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\304\263\270\257\273\350\301\246\072\045\163\054\040\301\367"
    "\276\367\072\045\144\054\040\267\271\272\247\072\045\144\054\040\274\272\300"
    "\345\307\374\072\040\045\144\015\012", time, name, job, level, growType);
}

#include "HistoryLog/WriteSelectCh.inc"
                               int growType)
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\304\263\270\257\274\261\305\303\072\045\163\054\040\301\367"
    "\276\367\072\045\144\054\040\267\271\272\247\072\045\144\054\040\274\272\300"
    "\345\307\374\072\045\144\015\012", time, name, job, level, growType);
}

#include "HistoryLog/WriteLevelUp.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\267\271\272\247\276\367\072\040\045\144\015\012", time, level);
}

#include "HistoryLog/WriteChat.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\303\244\306\303\305\270\300\324\072\045\144\040\263\273\277"
    "\353\072\040\045\163\015\012", time, type, content);
}

#include "HistoryLog/WriteFineQuest.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\304\371\275\272\306\256\040\277\317\267\341\072\040\045\144"
    "\015\012", time, questIdx);
}

#include "HistoryLog/WriteLearnSkill.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\275\272\305\263\300\324\274\366\040\111\104\072\045\144\054"
    "\040\114\145\166\145\154\072\045\144\015\012", time, skillIdx, level);
}

#include "HistoryLog/WriteDunStart.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\264\370\301\257\040\275\303\300\333\040\112\157\142\072\040"
    "\045\144\054\040\114\145\166\145\154\072\040\045\144\040\274\272\300\345\307"
    "\374\072\040\045\144\015\012", time, job, level, growType);
}

#include "HistoryLog/WriteKillMob.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\113\151\154\154\040\115\157\142\124\171\160\145\072\040\045"
    "\144\015\012", time, mobType);
}

#include "HistoryLog/WriteDie.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\307\303\267\271\300\314\301\337\040\273\347\270\301\015\012", time);
}

#include "HistoryLog/WriteGiveUp.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\264\370\301\257\306\367\261\342\072\040\045\163\015\012", time, reason);
}

#include "HistoryLog/WriteUseCoin.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\304\332\300\316\273\347\277\353\040\055\076\040\045\163\015"
    "\012", time, reason);
}

#include "HistoryLog/WriteDunClear.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\264\370\301\257\040\305\254\270\256\276\356\015\012", time);
}

#include "HistoryLog/WriteTradeUp.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\306\256\267\271\300\314\265\345\040\276\367\270\256\275\272"
    "\306\256\040\111\104\072\045\144\040\103\157\165\156\164\072\045\144\015\012", time, itemIdx, count);
}

#include "HistoryLog/WriteTradeDown.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\306\256\267\271\300\314\265\345\040\264\331\277\356\270\256"
    "\275\272\306\256\040\111\104\072\045\144\040\103\157\165\156\164\072\045\144"
    "\015\012", time, itemIdx, count);
}

#include "HistoryLog/WriteDisJoint.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\307\330\303\274\040\111\104\072\040\045\144\015\012", time, itemIdx);
}

#include "HistoryLog/WriteComposition.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\307\325\274\272\040\267\271\274\255\307\307\072\040\045\144"
    "\015\012", time, itemIdx);
}

#include "HistoryLog/WriteBuyItem.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\276\306\300\314\305\333\261\270\300\324\040\111\104\072\040"
    "\045\144\054\040\103\157\165\156\164\072\045\144\015\012", time, itemIdx, count);
}

#include "HistoryLog/WriteSellItem.inc"
{
    if (!CheckFD(file)) return;
    fprintf(file, "\133\045\163\135\276\306\300\314\305\333\306\307\270\305\040\111\104\072\040"
    "\045\144\054\040\103\157\165\156\164\072\045\144\015\012", time, itemIdx, count);
}

// ============================================================================
// cHistoryTrace::operator()(char const*, ...) —— ORIG 0x0854f990
//   CLog::instance() 后对 +0xa8（m_mutexes[7]，Mutex 0x18）构造 Guard<Mutex>，
//   再调 CLog::logHistory(fmt, va_list)，Guard 析构解锁。
// ============================================================================
#include "HistoryLog/operator.inc"
{
    // ORIG 0x0854f998/0x0854f9ba：Guard 与 logHistory 各调一次 CLog::instance()，
    // 两次调用不缓存（logHistory 的 CLog* 直载 %eax 寄存器）。
    Guard<Mutex> guard(&CLog::instance()->m_mutexes[7]);

    va_list ap;
    va_start(ap, fmt);
    CLog::instance()->logHistory(fmt, reinterpret_cast<char*>(ap));  // ORIG logHistory(fmt, &stack)

    va_end(ap);
}
