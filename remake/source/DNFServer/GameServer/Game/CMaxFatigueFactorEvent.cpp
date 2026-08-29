// df_game_r 最大疲劳系数事件 CMaxFatigueFactorEvent 实现。
// 报告：docs/class_func_reports/CMaxFatigueFactorEvent/ 全部 6 个方法
// （CMaxFatigueFactorEvent / ~CMaxFatigueFactorEvent /
//  ~CMaxFatigueFactorEvent_081b276e / StartEvent / StartEvent_081b27e8 /
//  EndEvent）。
#include "CMaxFatigueFactorEvent.h"

#include "GameTypes.h"             // cMyTrace
#include "CLocalChina_User.h"      // CLocalChina_User::SendFatigueEvent（static）
#include "GlobalData.h"
#include "GameWorld.h"
#include "CUser.h"                 // CUser / _Charac_info 完整定义（含 CUserCharacInfo）

extern int DEFAULT_MAX_FATIGUE;    // CUserCharacInfo.h 亦声明（ORIG B 0x9370f5c）

// ---------------------------------------------------------------------------
// ORIG 0x0812ca88（CLocalChina_User::SendFatigueEvent，报告
// CLocalChina_User/SendFatigueEvent.md）：
// GetRepeatEvent(2) → do-while 遍历 m_UserInWorld（循环头 G_GameWorld()+
// end()+!= 判定，不符即 return）→ state>2 才处理：ev 存在时经 getCurCharacW
// 写 ch->m_field35（IsEventing(0) ? factor*DEFAULT_MAX_FATIGUE/100 :
// DEFAULT_MAX_FATIGUE；ch==0 跳过 premium/SendFatigue 直入 ++it）；
// IsHavePremiumAdvantage 时按事件是否生效分两路 setCurCharacMaxPremiumFatigue
// （生效：事件缩放差 + 优惠倍率两段 /100；未生效：当前上限 × 优惠倍率 /100）；
// 最后 CUser::SendFatigue()。
void CLocalChina_User::SendFatigueEvent()
{
    CMaxFatigueFactorEvent* ev =
        static_cast<CMaxFatigueFactorEvent*>(GlobalData::s_event_manager->GetRepeatEvent(2));
    std::map<unsigned short, CUser*>::iterator it =
        G_GameWorld()->m_UserInWorld.begin();
    for (;;)
    {
        std::map<unsigned short, CUser*>::iterator end =
            G_GameWorld()->m_UserInWorld.end();
        if (!(it != end))
            return;
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            if (ev != 0)
            {
                _Charac_info* ch = user->getCurCharacW();
                if (ch != 0)
                {
                    if (ev->IsEventing(0))
                    {
                        ch->m_field35 = static_cast<unsigned short>(
                                                        ev->GetMaxFatigueFactor() * DEFAULT_MAX_FATIGUE / 100);
                    }
                    else
                    {
                        ch->m_field35 =
                            static_cast<unsigned short>(DEFAULT_MAX_FATIGUE);
                    }
                }
                else
                {
                    goto loop_tail;   // ORIG ch==0 跳过 premium/SendFatigue 直入 ++it
                }
            }
            if (user->IsHavePremiumAdvantage())
            {
                bool premiumEventing;
                if (ev == 0)
                {
                    premiumEventing = false;
                }
                else if (!ev->IsEventing(0))
                {
                    premiumEventing = false;
                }
                else
                {
                    premiumEventing = true;
                }
                if (premiumEventing)
                {
                    unsigned short local_e = static_cast<unsigned short>(
                                                static_cast<short>(ev->GetMaxFatigueFactor() * DEFAULT_MAX_FATIGUE / 100) -
                        static_cast<short>(DEFAULT_MAX_FATIGUE));
                    unsigned int rate1 = static_cast<unsigned short>(
                        user->GetPremiumInfo()->GetAdvantageFatigueRate());
                    int iVar3 = static_cast<int>(rate1 & 0xffff) * DEFAULT_MAX_FATIGUE;
                    unsigned int uVar6 = static_cast<unsigned int>(local_e);
                    unsigned int rate2 = static_cast<unsigned short>(
                        user->GetPremiumInfo()->GetAdvantageFatigueRate());
                    user->setCurCharacMaxPremiumFatigue(static_cast<unsigned short>(
                        static_cast<short>(iVar3 / 100) +
                        static_cast<short>(static_cast<int>(uVar6 * (rate2 & 0xffff)) / 100)));
                }
                else
                {
                    unsigned int curMax =
                        static_cast<unsigned int>(user->getCurCharacMaxFatigue());
                    unsigned int rate = static_cast<unsigned short>(
                        user->GetPremiumInfo()->GetAdvantageFatigueRate());
                    user->setCurCharacMaxPremiumFatigue(static_cast<unsigned short>(
                        static_cast<int>((curMax & 0xffff) * (rate & 0xffff)) / 100));
                }
            }
            user->SendFatigue();
        }
loop_tail:
        ++it;
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x081b2718：基类构造 + vptr 置位 + m_maxFatigueFactor = 100。
CMaxFatigueFactorEvent::CMaxFatigueFactorEvent()
    : m_maxFatigueFactor(100)
{
}

// ORIG 0x081b273e / 0x081b276e：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CMaxFatigueFactorEvent::~CMaxFatigueFactorEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x081b278c：SetEventFlag(true) + m_maxFatigueFactor = 100 +
// cMyTrace(__FUNCTION__,0x16,0)("Max Fatigue Factor Event Start!")。
void CMaxFatigueFactorEvent::StartEvent()
{
    SetEventFlag(true);
    m_maxFatigueFactor = 100;
    cMyTrace tr(__FUNCTION__, 0x16, 0);
    tr("Max Fatigue Factor Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x081b27e8：SetEventFlag(true) + m_maxFatigueFactor = param.m_duration +
// cMyTrace(__FUNCTION__,0x1e,0)(同字符串) + SendFatigueEvent()。
void CMaxFatigueFactorEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_maxFatigueFactor = param.m_duration;
    cMyTrace tr(__FUNCTION__, 0x1e, 0);
    tr("Max Fatigue Factor Event Start!");
    CLocalChina_User::SendFatigueEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x081b284c：SetEventFlag(false) + m_maxFatigueFactor = 100 +
// cMyTrace(__FUNCTION__,0x2a,0)("Max Fatigue Factor Event End!") +
// SendFatigueEvent()。
void CMaxFatigueFactorEvent::EndEvent()
{
    SetEventFlag(false);
    m_maxFatigueFactor = 100;
    cMyTrace tr(__FUNCTION__, 0x2a, 0);
    tr("Max Fatigue Factor Event End!");
    CLocalChina_User::SendFatigueEvent();
}
