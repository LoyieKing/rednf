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

CPCRoomPlayTimeEvent::CPCRoomPlayTimeEvent()
{
    // ORIG：CEventBase ctor 置基类 vptr 与 stADDINFO/+0x08 flag；本类 vptr 由编译器设置。
}

CPCRoomPlayTimeEvent::~CPCRoomPlayTimeEvent()
{
    // ORIG D1：恢复本类 vptr，再调 CEventBase::~CEventBase()。
}

void CPCRoomPlayTimeEvent::StartEvent()
{
    // ORIG 0x0811c5c4：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__, 0x15, 0)。
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x15, 0);
    tr("PC Room PlayTime Event Start!");
}

void CPCRoomPlayTimeEvent::StartEvent(Word_Param /*param*/)
{
    // ORIG 0x0811c616：vtable+0x8 槽（StartEvent()）虚转发。
    StartEvent();
}

void CPCRoomPlayTimeEvent::EndEvent()
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

void CPCRoomPlayTimeEvent::StartAction()
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
