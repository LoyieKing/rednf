// df_game_r Cera 商城奖励点数事件 CEventCeraShopRewardPoint 实现。
// 报告：docs/class_func_reports/CEventCeraShopRewardPoint/（C1 0x0816459a /
// D1 0x081645ca / D0 0x081645fa / StartEvent 0x08164618 /
// StartEvent(Word_Param) 0x08164684 / EndEvent 0x081646b8 /
// SendReward 0x0816471e）。
#include "CEventCeraShopRewardPoint.h"

#include <cstring>              // strncpy / strlen
#include "GameTypes.h"          // cMyTrace
#include "RDARScriptStringManager.h"   // g_scriptStringManager_ / findString
#include "CMailBoxHelper.h"     // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
                                //   （含 Inven_Item / ENUM_SERVER_GROUP）
#include "CUser.h"              // GetServerGroup / get_charac_no

// ORIG 0x0816459a：基类构造 + vptr 置位 + 清 m_field10(+0x10) / m_fieldC(+0x0c)
// （GCC 自动按声明序：先 +0x10 后 +0x0c，与 ORIG movl $0,0x10 → movl $0,0xc 同形）。
CEventCeraShopRewardPoint::CEventCeraShopRewardPoint()
{
    m_field10 = 0;
    m_fieldC = 0;
}

// ORIG 0x081645ca / 0x081645fa：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CEventCeraShopRewardPoint::~CEventCeraShopRewardPoint()
{
}

// ORIG 0x08164618：SetEventFlag(true) + cMyTrace(PRETTY, 0x13a, 0)
//   ("[Taiwan, CeraShopRewardPoint] Start event. Timing:%d, CeraPoint:%d ",
//    m_field10, m_fieldC)。
void CEventCeraShopRewardPoint::StartEvent()
{
    SetEventFlag(true);
    register int ceraPoint = m_fieldC;    // 0xc → esi（ORIG uVar1）
    register int timing = m_field10;      // 0x10 → ebx（ORIG uVar2）
    cMyTrace tr(__PRETTY_FUNCTION__, 0x13a, 0);
    tr("[Taiwan, CeraShopRewardPoint] Start event. Timing:%d, CeraPoint:%d ",
       timing, ceraPoint);
}

// ORIG 0x08164684：m_field10 = param.m_duration（低 ushort），
// m_fieldC = param.m_field2（高 ushort），再经 vptr slot2 虚调 StartEvent()。
// [b28c-31] N7 复核：末尾虚调用 callee ORIG %edx vs 4.4 %eax（20=20）。
void CEventCeraShopRewardPoint::StartEvent(Word_Param param)
{
    // [N7 封顶] 末尾转调 StartEvent() 的虚调用 callee 寄存器 %edx↔%eax 互换。
    m_field10 = param.m_duration;
    m_fieldC = param.m_field2;
    StartEvent();
}

// ORIG 0x081646b8：cMyTrace(PRETTY, 0x147, 0) + tr("[Taiwan, CeraShopRewardPoint]
// End event.") + 清 m_field10 / m_fieldC + SetEventFlag(false)。
void CEventCeraShopRewardPoint::EndEvent()
{
    cMyTrace tr(__PRETTY_FUNCTION__, 0x147, 0);
    tr("[Taiwan, CeraShopRewardPoint] End event.");
    m_field10 = 0;
    m_fieldC = 0;
    SetEventFlag(false);
}

// ORIG 0x0816471e：RDAR 字符串取标题/正文，附道具（idx 0x28ec3d @ m_addInfo，
// add_info = m_fieldC @ m_addInfo2）经 ReqDBSendNewSystemMail 发系统邮件。
void CEventCeraShopRewardPoint::SendReward(CUser* user)
{
    if (user == 0)
        return;

    char titleBuf[30];
    strncpy(titleBuf,
            g_scriptStringManager_.findString(4, "tw_shop_open_title", (bool*)0),
            0x1d);

    char contentBuf[256];
    strncpy(contentBuf,
            g_scriptStringManager_.findString(4, "tw_shop_open_content", (bool*)0),
            0xff);

    Inven_Item item;
    item.m_addInfo = 0x28ec3d;   // item idx @ +0x02
    item.m_addInfo2 = m_fieldC;  // add_info @ +0x07
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        titleBuf, item, (unsigned int)m_fieldC,
        (unsigned int)user->get_charac_no(-1),
        contentBuf, (int)strlen(contentBuf), 0,
        (ENUM_SERVER_GROUP)user->GetServerGroup(), false, false);
}
