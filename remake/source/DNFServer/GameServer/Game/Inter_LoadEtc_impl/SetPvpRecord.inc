bool Inter_LoadEtc::SetPvpRecord(CUser* user, SIG_LOAD_ETC* sig)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 92/92：P258 正极性 or 形（lossGrade>pvpGrade
    // 短路）+ CheckDaily/RefW 内联（P52/P0）+ CUser.h:3652 SetPvpRecord int→bool（Main 代落）。
    // [b23S3C] P183/P0：ORIG &sig->m_pvpResult 直落实参槽（add $0x340;mov %eax,0x4(%esp)），无 pvp 指针槽
    if (!user->SetPvpRecord(sig->m_pvpResult))
    {
        user->disableSavePvP();
        return false;
    }
    user->disableSavePvP();

    // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084c29b7 正极性 or 形（P258）：第一项
    // `lossGrade > pvpGrade` 短路进 body；P48 双臂各重取 getPvPExpLossGrade/RefR。
    int minPoint = GlobalData::s_ref_pvp_grade->GetPvpPoint(
                       GlobalData::s_ref_pvp_grade->getPvPExpLossGrade());
    // [SweepInterLoadEtcImpl2 2026-09-10] 真语义 bug（判据㊲ 条件极性反写；
    // jcc 目标下标交叉验证）：旧写 `lossGrade > pvpGrade || expPoint > minPoint`
    // 与 ORIG 整体反相。ORIG 0x084c29eb `cmp %eax,%ebx; jg +0xa5`（→ mov $0,%eax）
    // 与 0x084c2a00 `cmp %eax,%edx; jle +0xa5` 两跳同落 false 块、$1 走
    // fall-through ⇒ body 条件 = (lossGrade <= pvpGrade) && (expPoint > minPoint)。
    // AE 归一化把跳转目标统一为 <T>，故 92/92 的旧读数是假阴性。
    if (GlobalData::s_ref_pvp_grade->getPvPExpLossGrade() <=
            user->getPVPResultRefR()->m_pvpGrade &&
        user->getPVPResultRefR()->m_expPoint > minPoint)
    {
        // ORIG 0x084c2a12：now/lastPlay 寄存器直传（%ebx/%eax，无槽，P0）
        if (CheckDailyScheduleTime(4, user->getCurCharacLastPlayTick(),
                                   GlobalData::s_systemTime_.getCurSec()))
        {
            // [b28d-solo-Inter_LoadEtc_impl] P52/P0：RefW 结果经 %ebx 跨
            // getPvPExpLossExp 保活（ORIG 0x084c2a4e `mov %eax,%ebx`），无 w 槽。
            user->getPVPResultRefW()->m_expPoint = std::max(
                minPoint,
                user->getPVPResultRefR()->m_expPoint -
                    GlobalData::s_ref_pvp_grade->getPvPExpLossExp());
        }
    }
    return true;
