// [round5-A20 复核] fresh .o 170/169。残余=①curCharging 槽 -0x28 vs ORIG -0x50
//   （4.1 trace 前求值缓存落深区、4.4 落浅区，N82 分区）；②vararg %esi/%edi 角色互换
//   （ORIG `mov %eax,%esi`/`mov 0x7b(%eax),%edi` vs 4.4 %edi/%esi，N7）
//   ⇒ N82+N7 双封顶，语义完成。
bool Inter_LoadEtc::SetFatigue(CUser* user)
{
    // [Lane22-L17 L6] 判据⑩/P212：ORIG 槽 curCharging=**-0x50**（084c3221 实证，深于
    // days -0x24 / charging -0x1e / decAmount -0x1c 的浅区）⇒ 原码 curCharging 属
    // **函数级**声明（4.1 先声明深）；我方块内声明被 4.4 「浅=后声明」放到 -0x20，
    // 连带 days/charging 深移 4。提出到函数头（未初始化声明，P232 仍占槽）。
    // [Lane17 I 结果] 实测：days/charging 已归位（-0x24/-0x1e 与 ORIG 逐值一致），
    // curCharging 落 -0x28（ORIG -0x50，仍在浅区）；判据⑪ 直方图 ORIG
    // {−0x1c,−0x1e,−0x24,−0x34,−0x44,−0x4c,−0x50} vs 我方
    // {−0x1c,−0x1e,−0x24,−0x28,−0x38,−0x48,−0x4c} ⇒ 残余 = curCharging 区划
    // （4.1 把 trace 前求值缓存放深区 −0x50/−0x4c，4.4 放浅区）+ vararg %esi/%edi
    // 角色互换（N7）⇒ 封顶。
    int curCharging;
    if (user->getCurCharacR() == 0)
        return false;

    if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                               user->getCurCharacR()->m_lastPlayTick,
                               GlobalData::s_systemTime_.getCurSec()))     // +0x7b
    {
        int days = 0;
        if (user->getCurCharacR()->m_lastPlayTick != 0)
        {
            days = ComputeDaysScheduleTime(
                G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                user->getCurCharacR()->m_lastPlayTick,
                GlobalData::s_systemTime_.getCurSec());
        }

        short charging =
            GlobalData::s_fatigueBatteryHandle_->ChargingFatigueBattery(user, days);
        if (charging > 0)
        {
            user->IncFatigueBatteryCharging(charging);
            curCharging = user->GetFatigueBatteryCharging();
            cMyTrace(__PRETTY_FUNCTION__, 0x139a, 0)(
                "FATIGUE BATTERY : charno(%d) days(%d), lastplaytick(%d), "
                "curTick(%d), charging(%d), cur charging(%d)",
                  (int)user->getCurCharacNo(), days,
                  (unsigned)user->getCurCharacR()->m_lastPlayTick,
                  (unsigned)GlobalData::s_systemTime_.getCurSec(),
                (int)charging,
                curCharging);
        }

        if (user->GetFatigueBatteryCharging() > 0)
        {
            // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084e8778 `call; cwtl; mov %eax,-0x1c`
            // ⇒ decAmount 为 int 槽（short 声明恒生 `mov %ax,-N; cmpw`）。
            int decAmount = GlobalData::s_fatigueBatteryHandle_->ActiveFatigueBattery(user);
            if (decAmount != 0)
            {
                user->DecFatigueBatteryCharging(decAmount);
                cMyTrace(__PRETTY_FUNCTION__, 0x13a4, 0)(
                    "FATIGUE BATTERY ACTIVE: charno(%d) , descFatigueAmount(%d), "
                    "cur charging(%d)",
                    (int)user->getCurCharacNo(), (int)decAmount,
                    (int)user->GetFatigueBatteryCharging());
            }
        }

        user->RecoverFatigue(0);                                        // ORIG T 0x08657ada
        user->resetCurCharacMemberBonusFatigue();                       // ORIG T（恢复疲劳后清 bonus）
        user->ResetDailyQuest();
        user->ResetTrainingQuest();
    }
    return true;
}