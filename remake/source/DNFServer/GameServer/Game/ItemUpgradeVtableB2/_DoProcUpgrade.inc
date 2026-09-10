bool CItemUpgrade::_DoProcUpgrade(CUser* user, Inven_Item* item,
                                  const upgrade_info_t* info)
{
    unsigned int prob =
        (unsigned int)info->m_prob20[0];   // info +0x20（失败概率，具名字段）
    GlobalData::s_pEvent_UpGrade_Jar->GetProbability(
        info->m_recipeItem,   // info +0x2c（材料 item idx，具名字段）
        prob);

    // [b28d-solo-ItemUpgradeVtableB2] P183/P225：ORIG 0x08547562 起 m_randMax
    // **每用点重读**（`mov 0x8(%ebp),%eax; mov 0x4ec(%eax),%eax`），无 range
    // 缓存局部；两处 `&&` 在 ORIG 是各自 `test/je 同一尾` 的短路跳
    // （0x08547569/0x08547576、0x08547588/0x085475a3），4.4 对 `a&&b` 会物化
    // bool 岛 ⇒ 拆嵌套 if 复现（P225 推广）。
    if (prob != 0)
    {
        if (GlobalData::s_globalEffectManager_->isAffectedEffect(3))
        {
            const float effect =
                GlobalData::s_globalEffectManager_->getEffectValue(3);
            // ORIG 0x08547595..0x085475c4：`fildl m_randMax; fmuls effect;
            // RC=trunc fistpl; sub m_randMax` 且两次独立重读 m_randMax。
            const int delta =
                (int)((float)m_randMax * effect) - m_randMax;
            unsigned int a0 = prob - (unsigned int)delta;  // local_74[0]
            unsigned int a1 = 0;                           // local_74[1]
            prob = std::max(a1, a0);   // _ZSt3maxIjERKT_S2_S2_
        }
    }

    if (prob != 0)
    {
        if (expert_job::CAlchemist::IsExistContinuousEffectItem(user, 0x27ac59))
        {
            // ORIG 0x085475d5..0x085475f6：先 fildl m_randMax → fstps 槽，
            // 再 call GetItemUpgradeSuccessRate → fmuls ⇒ 左操作数是 m_randMax
            // （求值序：`(float)m_randMax * rate`，禁把 call 写左侧）。
            const int reduced =
                (int)((float)m_randMax *
                      expert_job::CAlchemist::GetItemUpgradeSuccessRate());
            unsigned int a0 = prob - (unsigned int)reduced;  // local_68
            unsigned int a1 = 0;                             // local_64
            prob = std::max(a1, a0);
        }
    }

    // [b28d-solo-ItemUpgradeVtableB2] P183：ORIG 0x0854764f `call
    // getCurCharacInvenR; ... call GetInvenSlot` 无 invenR 具名局部。
    Inven_Item worn = ((CUserCharacInfo*)user)->getCurCharacInvenR()
                          ->GetInvenSlot(0, 0xb);
    // ORIG 0x08547675 `mov -0x5b(%ebp),%eax; mov %eax,-0x1c(%ebp)` ⇒ 有具名
    // wornIdx 局部（先落槽再传入 find_item）。
    const int wornIdx = (int)worn.m_addInfo;
    CItem* equipScript = G_CDataManager()->find_item(wornIdx);
    if (equipScript != 0 && prob != 0)
    {
        // W 0x0854b552：失败概率 -= 装备 +0x1d0 强化概率提升值。
        prob -= (unsigned)((CEquipItem*)equipScript)->getUpgradeProbIncrease();
        if ((unsigned)m_randMax < prob)   // +0x4ec（具名字段）
        {
            prob = 0;
        }
    }

    if (prob != 0)
    {
        // ORIG：mov 0x4ec（m_randMax）存栈 → mov 0x4e8（m_rand）→ randInt(&bound)；
        // 结果**无槽**（%eax 直比 prob，0x08547768 `cmp %edx,%eax`）。
        unsigned long boundStorage = (unsigned long)m_randMax;
        if ((unsigned int)m_rand->randInt(boundStorage) < prob)
        {
            return false;
        }
    }
    item->IncUpgrade();
    return true;
