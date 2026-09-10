// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CMaxFatigueFactorEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
// [b28d-28] DIFF | ORIG 0x0812ca88 | P13/P28 已修（while 形 + `it++` 复现
// `jmp cond` + 尾部 `ppEi`）；残差 N82：我方具名中间量（rate1/iVar3/uVar6/
// rate2/curMax/rate）致帧 0x54 vs ORIG 0x30（P0/P183 内联化未完成）。
#include "CMaxFatigueFactorEvent/SendFatigueEvent.inc"
{
    CMaxFatigueFactorEvent* ev =
        static_cast<CMaxFatigueFactorEvent*>(GlobalData::s_event_manager->GetRepeatEvent(2));
    std::map<unsigned short, CUser*>::iterator it =
        G_GameWorld()->m_UserInWorld.begin();
    // [b28d-28] ORIG 0x0812cac5 `jmp <cond>`（while 前置跳转）+ 条件处每轮
    // `call end()`（非缓存）+ 尾部 `ppEi` 后置增量 ⇒ 源形 while(it != end())。
    while (it != G_GameWorld()->m_UserInWorld.end())
    {
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
        // [b28d-28] P28 增量侧：ORIG 增量段调 `_Rb_tree_iterator::ppEi`（后置，
        // 带独立 sret 临时槽）⇒ 源形必写 `it++`；`++it` 走 ppEv 免临时槽。
        it++;
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x081b2718：基类构造 + vptr 置位 + m_maxFatigueFactor = 100。
#include "CMaxFatigueFactorEvent/CMaxFatigueFactorEvent.inc"
    : m_maxFatigueFactor(100)
{
}

// ORIG 0x081b273e / 0x081b276e：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CMaxFatigueFactorEvent/dtor_CMaxFatigueFactorEvent.inc"
{
}

// ---------------------------------------------------------------------------
// ORIG 0x081b278c：SetEventFlag(true) + m_maxFatigueFactor = 100 +
// cMyTrace(__FUNCTION__,0x16,0)("Max Fatigue Factor Event Start!")。
#include "CMaxFatigueFactorEvent/StartEvent.inc"
{
    SetEventFlag(true);
    m_maxFatigueFactor = 100;
    cMyTrace tr(__FUNCTION__, 0x16, 0);
    tr("Max Fatigue Factor Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x081b27e8：SetEventFlag(true) + m_maxFatigueFactor = param.m_duration +
// cMyTrace(__FUNCTION__,0x1e,0)(同字符串) + SendFatigueEvent()。
#include "CMaxFatigueFactorEvent/StartEvent_1.inc"
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
#include "CMaxFatigueFactorEvent/EndEvent.inc"
{
    SetEventFlag(false);
    m_maxFatigueFactor = 100;
    cMyTrace tr(__FUNCTION__, 0x2a, 0);
    tr("Max Fatigue Factor Event End!");
    CLocalChina_User::SendFatigueEvent();
}
