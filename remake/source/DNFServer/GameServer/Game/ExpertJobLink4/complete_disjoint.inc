// complete_disjoint（报告 complete_disjoint.md，ORIG T 0x085d31a0 sz 0x42d）。
// 耐久/经验结算 + 请求者付费、机主收款 + 机主耐久不足时经 slot1 关闭任务。
// [L2Free4to15.S3 复核] p167 CLEAR（仅 C1/C2·D1/D2 别名）。残差 = x87 比较
// 两向（ORIG `fucompp/fnstsw/test/sete` vs 4.4 `fucomip/fstp/seta`，N85）
// + 槽差 8（-0x18/-0x1c ↔ -0x20/-0x24）⇒ N85 封顶。
int CDisjointer::complete_disjoint(CUser* requestUser, PacketGuard& packet)
{
    // 耐久消耗 [m_enduranceReduce1(+0x26c), m_enduranceReduce2(+0x270)] 随机
    // [b28d-solo-ExpertJobLink4 P0/P183]：ORIG 0x85d31a8 起 GetScript 共 3 次
    // 内联调用（0x270/0x26c/0x26c），无 script 具名槽。
    int enduranceUse =
        get_rand_int(GetScript()->m_enduranceReduce2 -
                         GetScript()->m_enduranceReduce1 + 1) +
        GetScript()->m_enduranceReduce1;

    // ORIG 扩展数据访问全程内联（无 ownerExpand 具名槽）。
    int curEndurance =
        ((CCharacExpertJob*)((CUser*)m_machine.get_user())
             ->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
            ->get_disjoint_machine_endurance();
    if (curEndurance < enduranceUse)
        enduranceUse = curEndurance;

    ((CCharacExpertJob*)((CUser*)m_machine.get_user())
         ->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
        ->set_disjoint_machine_endurance(curEndurance - enduranceUse);

    // 经验收益 [m_gainExp1(+0x284), m_gainExp2(+0x288)] 随机 + 概率加成项
    int expGain =
        get_rand_int(GetScript()->m_gainExp2 - GetScript()->m_gainExp1 + 1) +
        GetScript()->m_gainExp1;

    // ORIG 0x85d328d 用 getCurCharacInvenR()（const 版）取背包。
    STItemScript::SEXPERTJOB::stExpertJobAdditionalExp additionalExp;
    int jobExp = requestUser->GetCurCharacExpertJobExp();
    int jobLevel = requestUser->GetCurExpertJobLevel(jobExp);
    int jobType = requestUser->GetCurCharacExpertJobType();
    requestUser->getCurCharacInvenR()->GetExpertJobAddtionalExp(
        (ENUM_EXPERT_JOB_TYPE)jobType, jobLevel, additionalExp);
    if ((float)get_rand_int(100) < additionalExp.m_field0)
        expGain = additionalExp.m_field4 + expGain;

    if (expGain > 0)
    {
        CExpertJob::IncreaseExpertJobExp(
            m_machine.get_user(),
            m_machine.get_user()->GetCurCharacExpertJobExp() + expGain);
    }

    if (m_machine.get_user() == requestUser)
    {
        // 自助拆解：只回填金额与剩余耐久
        packet.put_int(requestUser->getCurCharacMoney());
        packet.put_int(
            ((CCharacExpertJob*)((CUser*)m_machine.get_user())
                 ->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
                ->get_disjoint_machine_endurance());
    }
    else
    {
        // 他人拆解：请求者扣费 → 回包 → 机主收款 → 机主侧 0xcb 通知
        int cost = get_disjoint_cost();
        requestUser->getCurCharacInvenW()->use_money(cost, (eMoneySubReason)0x1d,
                                                     true);
        packet.put_int(requestUser->getCurCharacMoney());
        packet.put_int(
            ((CCharacExpertJob*)((CUser*)m_machine.get_user())
                 ->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
                ->get_disjoint_machine_endurance());

        ((CUser*)m_machine.get_user())
            ->getCurCharacInvenW()
            ->gain_money(cost, (eMoneyAddReason)0x16, true, 0);

        PacketGuard notify;
        notify.put_header(0, 0xcb);
        notify.put_byte(1);
        notify.put_int(((CUser*)m_machine.get_user())->getCurCharacMoney());
        notify.put_int(
            ((CCharacExpertJob*)((CUser*)m_machine.get_user())
                 ->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
                ->get_disjoint_machine_endurance());
        notify.finalize(true);
        ((CUser*)m_machine.get_user())->Send(notify);
    }

    // 机主耐久耗尽 → 经虚槽 slot1 OnCloseMission(机主, 0) 关闭
    if (((CCharacExpertJob*)((CUser*)m_machine.get_user())
             ->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
            ->get_disjoint_machine_endurance() < 1)
        OnCloseMission(m_machine.get_user(), ENUM_CLOSE_MISSION_TYPE_0);

    return 1;
}