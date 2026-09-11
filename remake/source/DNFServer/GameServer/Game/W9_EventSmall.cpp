// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W9_EventSmall/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <ctime>
#include "W9_EventSmall.h"
#include "GameTypes.h"       // cMyTrace
#include "GlobalData.h"
#include "CSystemTime.h"
#include "StreamPool.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"
#include "CUser.h"

#include "W9_EventSmall/getDailyInitializeTime.inc"

#include "W9_EventSmall/CTrcOnOffEvent.inc"

#include "W9_EventSmall/makeRequest.inc"


// ============================ 事件基类模板 ============================
// 四个小类的 Start/End 全部为「SetEventFlag + cMyTrace(function 名, len, 0) +
// trace(msg)」同构（各报告反编译一致）。构造/析构只置 vptr（编译器生成），
// IsEventing 为基类透传。

#include "W9_EventSmall/dtor_CTrcOnOffEvent.inc"

#include "W9_EventSmall/StartEvent.inc"


#include "W9_EventSmall/EndEvent.inc"


#include "W9_EventSmall/StartEvent_1.inc"


#include "W9_EventSmall/IsEventing.inc"


#include "W9_EventSmall/CFatigueBuffEvent.inc"
#include "W9_EventSmall/dtor_CFatigueBuffEvent.inc"

#include "W9_EventSmall/StartEvent_2.inc"


#include "W9_EventSmall/EndEvent_1.inc"


#include "W9_EventSmall/StartEvent_3.inc"


#include "W9_EventSmall/CWeekendBonusEvent.inc"
#include "W9_EventSmall/dtor_CWeekendBonusEvent.inc"

#include "W9_EventSmall/StartEvent_4.inc"


#include "W9_EventSmall/EndEvent_2.inc"


#include "W9_EventSmall/StartEvent_5.inc"


#include "W9_EventSmall/CAutoMarketConditionsControlEvent.inc"
#include "W9_EventSmall/dtor_CAutoMarketConditionsControlEvent.inc"

#include "W9_EventSmall/StartEvent_6.inc"


#include "W9_EventSmall/EndEvent_3.inc"


#include "W9_EventSmall/StartEvent_7.inc"


#include "W9_EventSmall/CCharacBloodDungeon.inc"


#include "W9_EventSmall/_reset.inc"


#include "W9_EventSmall/_saveData.inc"


#include "W9_EventSmall/loadData.inc"


#include "W9_EventSmall/getData.inc"


#include "W9_EventSmall/ResetDaily.inc"
{
    m_enterCount = 0;

    time_t curSec =
        static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    struct tm tmBuf;
    // P43：返回指针须经返回值访问（ORIG lret 槽 -0xc，tm_wday 经其读取）。
    struct tm* t = localtime_r(&curSec, &tmBuf);
    if (t->tm_wday == 1)                     // 周一：weekly 区清零
    {
        m_point = 0;
        m_weeklyEnterCount = 0;
        m_useGold = 0;
        m_flag20 = 0;
    }
    // P184 死块照抄：ORIG 尾部对 tm_hour/min/sec 的归零死写（-O0 逐条发射）。
    t->tm_hour = 0;
    t->tm_min = 0;
    t->tm_sec = 0;
}

#include "W9_EventSmall/IncPoint.inc"


#include "W9_EventSmall/IncUseGold.inc"


#include "W9_EventSmall/IncEnterCount.inc"


#include "W9_EventSmall/GetEnterCount.inc"


#include "W9_EventSmall/GetWeeklyEnterCount.inc"

