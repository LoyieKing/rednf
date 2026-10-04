// 0x08657ada
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08657ada | _ZN5CUser14RecoverFatigueEi
// 2026-09-11 CuA 复核（**勿按「形参互换」改**）：ORIG 0x8657b35..0x8657b5a 的
//   makeRequest(get_acc_id, getCurCharacSchoolPoint, GetUID) 段，ORIG 把 arg3(GetUID) 放
//   %ebx、arg2 放 %esi；我方 %ebx/%esi 恰好对调 —— 但**两侧求值序同为右→左**
//   （arg3 先算），esi/ebx 内容同步互换、写入 0x8/0x4(%esp) 的槽位不变 ⇒
//   语义等价，仅 4.1↔4.4 寄存器选择差（N7）。改源码反而会真错位。
void CUser::RecoverFatigue(int value)
{
    // ORIG 0x08657ada：角色为空直接返回（跳过全部）；否则先写回
    // premium 疲劳（ushort 截断）与当前疲劳
    if (getCurCharacR() == 0)
        return;
    setCurCharacPremiumFatigue((unsigned short)value);
    setCurCharacFatigue(value);

    // ORIG 0x08657ada：学校点非零 → DB 上报并清零
    if (getCurCharacSchoolPoint() != 0)
    {
        DB_UpdateSchoolPoint::makeRequest(get_acc_id(),
                                          getCurCharacSchoolPoint(), GetUID());
        resetCurCharacSchoolPoint();
    }

    // ORIG：事件 2（CMaxFatigueFactorEvent）活跃 → m_maxFatigue = factor*DEFAULT/100，
    // 否则 DEFAULT_MAX_FATIGUE。
    CEventBase* ev2 = GlobalData::s_event_manager->GetRepeatEvent(2);
    _Charac_info* cur = getCurCharacW();
    if (cur != 0)
    {
        if (ev2 != 0)
        {
            if (ev2->IsEventing((CUser*)0))
            {
                cur->m_maxFatigue =
                    (unsigned short)(
                        reinterpret_cast<CMaxFatigueFactorEvent*>(ev2)
                            ->GetMaxFatigueFactor()
                        * DEFAULT_MAX_FATIGUE / 100);
            }
            else
            {
                cur->m_maxFatigue = (unsigned short)DEFAULT_MAX_FATIGUE;
            }
        }
        if (IsHavePremiumAdvantage())
        {
            if (ev2 != 0 && ev2->IsEventing((CUser*)0))
            {
                // base = (short)(factor*DEFAULT/100) - (short)DEFAULT
                short base = (short)(
                        reinterpret_cast<CMaxFatigueFactorEvent*>(ev2)
                            ->GetMaxFatigueFactor()
                        * DEFAULT_MAX_FATIGUE / 100)
                           - (short)DEFAULT_MAX_FATIGUE;
                setCurCharacMaxPremiumFatigue(
                    (unsigned short)(
                        // b28d-solo-CUser p167：ORIG 0x08657ada 直接
                        // `call CUserPremium::GetAdvantageFatigueRate(this+0x7121c)`（7 次），
                        // 无 GetPremiumInfoW() 中间调用（我方 EXTRA ×7）⇒ 改具名成员访问。
                        (unsigned short)m_premium.GetAdvantageFatigueRate()
                            * DEFAULT_MAX_FATIGUE / 100
                      + (unsigned short)base
                            * (unsigned short)m_premium.GetAdvantageFatigueRate()
                            / 100));
            }
            else
            {
                setCurCharacMaxPremiumFatigue(
                    (unsigned short)(
                        (unsigned short)getCurCharacMaxFatigue()
                            * (unsigned short)m_premium.GetAdvantageFatigueRate()
                            / 100));
            }
        }
    }

    // ORIG：事件 0x57 活跃（ORIG 无空检查）→ m_maxFatigue += DataManager 0x51c；
    // rate!=0 且 PC 房 → premium = rate*DEFAULT/100 + 0x51c，否则 rate*DEFAULT/100。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing((CUser*)0))
    {
        _Charac_info* curW = getCurCharacW();
        curW->m_maxFatigue = (unsigned short)(
            // dm+0x51c = SPS(+0x68)+0x4b4 = m_weekendBouns4b4.m_bonusFatiguePercent
            // [L9-C组J PATCH J2] ORIG `movzwl 0x51c(%eax),%eax` 单条 disp；取址+解引用在 4.4
            //   产 `add $0x51c,%eax; movzwl (%eax),%eax`（多 1 条）⇒ 直接值成员访问
            //   （字段本为 unsigned short，ServerParameterScript.h:302，读值不变）。
            curW->m_maxFatigue
            + G_CDataManager()->m_serverParameter.m_weekendBouns4b4
                  .m_bonusFatiguePercent);
        if (m_premium.GetAdvantageFatigueRate() != 0
            && GetPCRoomNo() != 0)
        {
            setCurCharacMaxPremiumFatigue(
                (unsigned short)(
                    (unsigned short)m_premium.GetAdvantageFatigueRate()
                        * DEFAULT_MAX_FATIGUE / 100
                  + G_CDataManager()->m_serverParameter.m_weekendBouns4b4
                        .m_bonusFatiguePercent));
        }
        else
        {
            setCurCharacMaxPremiumFatigue(
                (unsigned short)(
                    (unsigned short)m_premium.GetAdvantageFatigueRate()
                        * DEFAULT_MAX_FATIGUE / 100));
        }
    }
    else
    {
        if (IsHavePremiumAdvantage())
        {
            setCurCharacMaxPremiumFatigue(
                (unsigned short)(
                    (unsigned short)getCurCharacMaxFatigue()
                        * (unsigned short)m_premium.GetAdvantageFatigueRate()
                        / 100));
        }
    }
    resetCurCharacUsedFatigue();
}