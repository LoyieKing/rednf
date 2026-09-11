void Inter_LoadEtc::_checkUserDisadvantage(CUser* user)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 154/154：P171 照抄 ORIG 恒真 bool 局部
    // （-0xd，第二个惩罚项/货物检查开关）+ `penalty +=` 读改写回 + hundredBase 声明提前
    // （赋值保持原位，P212）+ `100 - min(...)` 单表达式 + cargo 内联（P0）。
    if (user->IsGameMasterMode())
        return;

    // [b23S3C] P48/§4.1：ORIG Get() 结果 %eax 直落 isRestricted 实参槽（+0x2f），
    // 无 restrictMgr 槽、无 null 检查（原 null 检查系伪造行为）
    if (!ARAD::Singleton<ServiceRestrictManager>::Get()
             ->isRestricted(RestrictCategory::Enum(3), 1))
        return;
    if (user->getCurCharacR() == 0)
        return;

    // [b23S3C] P56/P198：ORIG 直测条件内联（xor$1/test/jne），无 invalid bool 槽
    if (user->get_charac_level() < 0x14 || user->getStdDropRate() == 0)
        return;

    // 遍历 0..29999 任务清除位（CQuestClear 区位于 questR+4，含 m_user 头）
    // [b28d-solo-Inter_LoadEtc_impl] P196：ORIG 边界 `cmpl $0x752f; setle`（有符号）
    // ⇒ i 必须 int；P248 档②：ORIG `movzbl slot; xor $1; test; je` ⇒ 标志位 bool。
    bool hasClearedQuest = false;
    for (int i = 0; i < 30000; ++i)
    {
        if ((user->getCurCharacQuestR()->m_cleared).isClearedQuest((unsigned int)i))
        {
            hasClearedQuest = true;
            break;
        }
    }

    // [b28d-solo-Inter_LoadEtc_impl] P171 照抄：ORIG 0x084c2833 存在恒真 bool 局部
    // （-0xd `movb $0x1` 后全程只读、无写 0 点）——第二个惩罚项与货物检查的开关。
    bool penaltyEnabled = true;
    int hundredBase;                                    // 声明提前（赋值保持 ORIG 位置）
    int penalty = 0;
    if (!hasClearedQuest)
        penalty += 0x28;                                // 未清任务：+40
    if (!penaltyEnabled)
        penalty += 0x1e;
    if (!hasClearedQuest || !penaltyEnabled)
    {
        // P0：ORIG 无 cargo 具名槽（getCurCharacCargoR 结果直落实参槽）
        if (user->getCurCharacCargoR()->GetItemCount() == 0)
            penalty += 0xa;                             // 无货物再 +10
    }

    hundredBase = 100;
    // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084c28ba：min 返回值经 %eax 直入减法
    // （无回读槽）⇒ 单表达式 `100 - min(...)`。
    penalty = 100 - std::min(hundredBase, penalty);     // 100 - min(penalty, 100)
    // [b23S3C] P48：ORIG getStdDropRate 双臂各重取（+0x16a 入 min、+0x18f 入比较，无 dropRate 槽）
    penalty = std::min(penalty, user->getStdDropRate());   // 再与当前掉率取小

    if (user->getStdDropRate() != penalty)
    {
        DB_SaveUserHandicap::makeRequest(user->get_acc_id(), 0x9c41, 0x65,
                                         penalty, 3, 0);
        user->setStdDropRate(penalty);
    }
}