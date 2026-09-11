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


#include "HistoryLog/LogClose.inc"


#include "HistoryLog/LogOpen.inc"


#include "HistoryLog/WriteCreateCh.inc"



#include "HistoryLog/WriteDeleteCh.inc"


#include "HistoryLog/WriteSelectCh.inc"


#include "HistoryLog/WriteLevelUp.inc"


#include "HistoryLog/WriteChat.inc"


#include "HistoryLog/WriteFineQuest.inc"


#include "HistoryLog/WriteLearnSkill.inc"


#include "HistoryLog/WriteDunStart.inc"


#include "HistoryLog/WriteKillMob.inc"


#include "HistoryLog/WriteDie.inc"


#include "HistoryLog/WriteGiveUp.inc"


#include "HistoryLog/WriteUseCoin.inc"


#include "HistoryLog/WriteDunClear.inc"


#include "HistoryLog/WriteTradeUp.inc"


#include "HistoryLog/WriteTradeDown.inc"


#include "HistoryLog/WriteDisJoint.inc"


#include "HistoryLog/WriteComposition.inc"


#include "HistoryLog/WriteBuyItem.inc"


#include "HistoryLog/WriteSellItem.inc"


#include "HistoryLog/operator.inc"

