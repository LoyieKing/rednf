// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPCRoomPlayTimeEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CPCRoomPlayTimeEvent 实现（网吧时长活动事件）。
// 依据 ORIG elf 反汇编（报告目录缺失，nm 定址 0x0811c540 后 objdump 转写）：
//   C1 0x0811c55a：CEventBase() + vptr
//   D1 0x0811c576 / D0 0x0811c5a6：vptr 恢复 + ~CEventBase
//   StartEvent 0x0811c5c4：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__, 0x15, 0)
//   StartEvent(Word_Param) 0x0811c616：虚转发 StartEvent()（vtable+0x8 槽）
//   EndEvent 0x0811c630：SetEventFlag(false) + 日志 + 遍历 m_UserInWorld 清网吧时长数据
//   StartAction 0x0811c754：遍历 m_UserInWorld 置网吧起始时间并通知
#include "CPCRoomPlayTimeEvent.h"

#include <map>

#include "CUser.h"          // isAffectedPremium/get_state/setPcRoomPlayTimeData
#include "GameWorld.h"      // GameWorld::m_UserInWorld (+0x134)
#include "GlobalData.h"     // s_systemTime_ (0x941f714)
#include "CSystemTime.h"    // getCurSec

#include "CPCRoomPlayTimeEvent/CPCRoomPlayTimeEvent.inc"
{
    // ORIG：CEventBase ctor 置基类 vptr 与 stADDINFO/+0x08 flag；本类 vptr 由编译器设置。
}

#include "CPCRoomPlayTimeEvent/dtor_CPCRoomPlayTimeEvent.inc"
{
    // ORIG D1：恢复本类 vptr，再调 CEventBase::~CEventBase()。
}

#include "CPCRoomPlayTimeEvent/StartEvent.inc"
{
    // ORIG 0x0811c5c4：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__, 0x15, 0)。
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x15, 0);
    tr("PC Room PlayTime Event Start!");
}

#include "CPCRoomPlayTimeEvent/StartEvent_1.inc"
{
    // ORIG 0x0811c616：vtable+0x8 槽（StartEvent()）虚转发。
    StartEvent();
}

#include "CPCRoomPlayTimeEvent/EndEvent.inc"
{
    // ORIG 0x0811c630：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__, 0x1f, 0)，
    // 再遍历全服 m_UserInWorld，对 state>2 且受 0xc 网吧优惠影响者清空网吧时长数据。
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1f, 0);
    tr("PC Room PlayTime Event End!");
    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user == 0)
            continue;
        if (user->get_state() <= 2)
            continue;
        if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc))
            continue;
        user->setPcRoomPlayTimeData(0, 0, 0);
    }
}

#include "CPCRoomPlayTimeEvent/StartAction.inc"
{
    // ORIG 0x0811c754：取当前秒，遍历全服 m_UserInWorld，对 state>2 且受
    // 0xc 网吧优惠影响者置网吧起始时间并发送时长通知包。
    int curSec = GlobalData::s_systemTime_.getCurSec();
    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user == 0)
            continue;
        if (user->get_state() <= 2)
            continue;
        if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc))
            continue;
        user->setPcRoomPlayStartTime(curSec);
        user->sendNotiPcRoomPlayTime();
    }
}
