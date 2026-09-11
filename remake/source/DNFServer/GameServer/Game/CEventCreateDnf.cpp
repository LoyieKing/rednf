// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CEventCreateDnf/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CEventCreateDnf.h"
#include "GlobalData.h"     // s_systemTime_（LinkE2）
#include "TimerQueue.h"     // G_TimerQueue/InsertTimer/TIMER_MESSAGE（LinkE2）
#include "GameTypes.h"      // cMyTrace（LinkE2）
#include "CreateDnfEventSciprt.h"   // CreateDnfEventSciprtServer::GetItemInfo（StubS3A SendReward）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_.findString
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper 静态邮件接口
#include "CUser.h"                    // CUser::getCurCharacNo/GetServerGroup/Send（完整类型）
#include "PacketGuard.h"
#include "DBSelectCreateDnfEventInfo.h"   // makeRequest（RegistStartTimer ORIG 0x08163fc2）
#include "LogManager.h"                   // logFormat（StartEvent(Word_Param) ORIG 0x08163cb6）


#include <ctime>

#include "CEventCreateDnf/CEventCreateDnf.inc"


#include "CEventCreateDnf/dtor_CEventCreateDnf.inc"

#include "CEventCreateDnf/setState.inc"


#include "CEventCreateDnf/GetRate.inc"


#include "CEventCreateDnf/IsRewardCondition.inc"


#include "CEventCreateDnf/SetEventInfo.inc"


#include "CEventCreateDnf/StartEvent.inc"


#include "CEventCreateDnf/StartEvent_1.inc"
{
    // b28d-D42 + round1 R19：ORIG 0x08163be0 逐字节读参（`lea 0xc(%ebp),%eax;
    // mov %eax,-0xc(%ebp); movzbl (%eax),%edx; movzbl 0x1(%eax),%edx;
    // movzbl 0x2(%eax),%eax`）⇒ P244 具名指针槽 + P311 成员视图（`p[i]` 下标形
    // 被 4.4 折成 `add $0xN,%eax` 两条，成员视图才出 disp 单条）。
    struct WordParamBytes
    {
        unsigned char m_b0;  // +0x00 → m_startHour
        unsigned char m_b1;  // +0x01 → m_endHour
        unsigned char m_b2;  // +0x02 → m_reloadInterval（分钟数）
        unsigned char m_b3;  // +0x03（ORIG 未读）
    };
    const WordParamBytes* p = reinterpret_cast<const WordParamBytes*>(&param);
    m_startHour = p->m_b0;
    m_endHour = p->m_b1;
    m_reloadInterval = static_cast<unsigned short>(p->m_b2 * 60);
    if (m_startHour < 24 && m_endHour < 24 &&
        m_reloadInterval > 59 && m_reloadInterval < 3601)
        StartEvent();
    else
    {
        // ORIG 0x08163cb6：LogManager::logFormat + 虚调 EndEvent()
        LogManager::logFormat(
            1, "localtaiwan/Event/EventCreateDnf.cpp",
            __PRETTY_FUNCTION__, 0x31,
            "[Taiwan, CreateDnfEvent] Error! startTime:%2dh, endTime:%2dh, reLoadInterval:%d",
            (int)m_startHour, (int)m_endHour, (int)m_reloadInterval);
        EndEvent();
    }
}

#include "CEventCreateDnf/EndEvent.inc"


#include "CEventCreateDnf/RegistStartTimer.inc"


#include "CEventCreateDnf/RegistNextTimer.inc"


#include "CEventCreateDnf/SendReward.inc"

