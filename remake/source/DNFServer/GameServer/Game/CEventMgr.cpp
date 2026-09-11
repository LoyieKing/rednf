// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CEventMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CEventMgr.h"

#include <string.h>
#include <time.h>

#include "GlobalData.h" // GlobalData::s_systemTime_（CSystemTime::getCurSec 0x080cbc9e）

// [A31-GI · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit，使本 TU 发射
// `_GLOBAL__I__ZN8WongWork9CEventMgr11_IsEventingEiiiiiiiiNS0_10EVENT_TYPEE`（ORIG 0x848f1e4）；
// _IsEventing 定义随之前移为本 TU 首个强外部符号（= GI 键名来源）。
// in_time 仅给前置声明（纯 extern 声明不参与键名，P327）。零语义/布局改动。
#include <iostream>

#include "CEventMgr/in_time.inc"

namespace WongWork
{

#include "CEventMgr/_IsEventing.inc"
                            int endYear, int endMon, int endDay, int endHour,
                            EVENT_TYPE type)
{
    struct tm endTm;
    struct tm begin;
    memset(&endTm, 0, sizeof(struct tm));   // ORIG memset 0x2c == sizeof(tm)@i386
    memset(&begin, 0, sizeof(struct tm));

    begin.tm_year = startYear - 1900;       // ORIG sub $0x76c
    begin.tm_mon  = startMon - 1;
    begin.tm_mday = startDay;
    begin.tm_hour = startHour;

    endTm.tm_year = endYear - 1900;
    endTm.tm_mon  = endMon - 1;
    endTm.tm_mday = endDay;
    endTm.tm_hour = endHour;

    // ORIG Ghidra 形态：cVar1 = in_time(&begin,&end); if (cVar1) {
    //   if (type != EVENT_WEEKEND) return 1;
    //   local_14 = CSystemTime::getCurSec(&GlobalData::s_systemTime_);
    //   ptVar2 = localtime_r(&local_14,&local_98);
    //   if (ptVar2->tm_wday==0 || ptVar2->tm_wday==6) return 1; }
    // return 0;
    if (in_time(&begin, &endTm))
    {
        if (type == EVENT_WEEKEND)
        {
            time_t now = (time_t)GlobalData::s_systemTime_.getCurSec();
            struct tm cur;
            struct tm* pcur = localtime_r(&now, &cur);
            if ((pcur->tm_wday == 0) || (pcur->tm_wday == 6))
                return true;
        }
        else
        {
            return true;
        }
    }
    return false;
}

} // namespace WongWork（_IsEventing 前移段结束）

#include "CEventMgr/in_time_1.inc"
{
    time_t t1 = mktime(begin);
    time_t t2 = mktime(end);
    time_t now = time(NULL);
    return (now >= t1 && now < t2);
}

namespace WongWork
{

#include "CEventMgr/GetPremiumServiceBonus.inc"


#include "CEventMgr/IsChildrensDayEventing.inc"


} // namespace WongWork
