// [b28d-solo-CUserCharacInfo AE] 三旋钮一次归零（108=108）：
// ①P0 删具名 `CUserCharacInfo* other` 局部（ORIG 0x0864633a/0x0864634d 两次重读
//   0x10(%ebp) 形参直用）；
// ②P198②/P225 两处 bool 条件整体内联（ORIG 直测 `test %al,%al; je`，无 mov$1/mov$0 物化）；
// ③P252 具名 `float penalty` 局部（ORIG 0x086462f0 `fstps -0x10(%ebp)` + 回读压栈；
//   删 other 局部后槽位自洽，旧「复测反劣」系 other 占槽所致）。
void CUserCharacInfo::DieCurCharac(int responTime, CUser* user)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_ghost = 1;
    CUR->m_chaosStateTime = GlobalData::s_systemTime_.getCurSec() - CUR->m_chaosDieTime;
    CUR->m_chaosDieTime = GlobalData::s_systemTime_.getCurSec() + responTime;
    int ghostTime = 0;
    // [b28d-solo-CUserCharacInfo P198②/P225] ORIG 0x086462b0 直测
    // `test %al,%al; je`（无 bool 物化）⇒ 条件整体内联，勿用具名/register bool。
    if (GlobalData::s_power_manager->IsPowerWarEventOn() ||
        getCurCharacVill() == 7)
    {
        // [b28d-solo-CUserCharacInfo P252] 具名 float penalty 局部复现 ORIG
        // 0x086462f0 `fstps -0x10(%ebp)` + 回读压栈（删 other 局部后槽位自洽）。
        float penalty =
            GlobalData::s_power_manager->GetPowerWarResponPenalty(user);
        ghostTime =
            GlobalData::s_power_manager->GetPowerWarGhostTime(user, penalty);
    }
    else
    {
        // [b28d-solo-CUserCharacInfo P0] 删具名 other 局部：ORIG 0x0864633a/0x0864634d
        // 两次重读 0x10(%ebp)（形参直用），命名局部多槽 + 2 insn。
        ghostTime = ((ServerParameterScript*)((char*)G_CDataManager() +
                                              offsetof(CDataManager, m_serverParameter)))
                        ->getAssaultGhostTime(((CUserCharacInfo*)user)->get_charac_level(),
                                              ((CUserCharacInfo*)user)->GetCurCharacChaosPoint());
    }
    // [b28d-solo-CUserCharacInfo P198②] 同上：ORIG 直测 `test %al,%al; je`。
    if (IsCurCharacGhost() && ghostTime != 0)
    {
        int now = GlobalData::s_systemTime_.getCurSec();
        SetCurCharacChaosDieTime(now);
        SetCurCharacChaosResponTime(now + ghostTime);
    }
    }
}