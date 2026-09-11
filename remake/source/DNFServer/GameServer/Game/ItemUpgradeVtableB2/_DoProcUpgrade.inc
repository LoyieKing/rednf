// ============================================================================
// _DoProcUpgrade @ 0x0854755a
//   以 info+0x20 的失败概率经事件/炼金/装备三段修正后掷骰：
//   roll >= prob → IncUpgrade 且返回 true（强化成功）；否则保持等级并返回
//   false（调用方走 _ProcPenalty）。
// [b28d-solo-ItemUpgradeVtableB2] DIFF 166/163 封顶（N82 + setcc-fold）：
//   P167 CLEAR 14/14，帧同（0x94）。已修：删 range 缓存（m_randMax 每用点
//   重读）、两处 `&&` 拆嵌套 if（复现双 test/je 短路口）、a0/a1 独立 max 局部、
//   reduced 左操作数改 m_randMax（复现 fildl→fstps→call→fmuls 求值序）、
//   内联 invenR/roll、恢复 wornIdx 具名槽。残余：①prob 等局部槽号整体重排
//   （4.4 首次使用序 vs 4.1 声明序，N82）②`rollWin = roll < prob` 4.4 恒
//   setb+槽+xor，ORIG 是 mov $1/$0 + test 岛（P197④ setcc-fold，call 结果
//   寄存器直比不可翻）。
// [2026-09-10 SweepCEquipItem2] 本轮复核（未改）：gap 3（orig 166 / our 163），
// p167 14=14 CLEAR；非槽差异仅 `setb %al` 一处（ORIG 0x085476xx 的
// `jae; mov $1,%eax; jmp; mov $0,%eax` setcc-fold 岛，P197④/N 封顶），
// 其余为 prob/wornIdx 等局部槽号重排（N82）。语义与 ORIG 一致。
// [L3EHeroItemUpgrade 2026-09-11 负结论] 复查 ORIG 0x0854776d..0x08547784 的
// `jae; mov $0x1,%eax; jmp; mov $0x0,%eax; test %al,%al`（4.1 对**具名 bool
// 局部** bVar1 的物化，Ghidra 反编译直证）。实测改写为
// `bool failed=false; if (prob) { ... if (roll<prob) failed=true; }
//  if (!failed) IncUpgrade(); return !failed;` 后 4.4 发
// `setb→movb $1,-0x15; movzbl; xor $0x1; test; je`（166 → **168，劣化 +2**）⇒
// 回滚。现形（return 折入分支，`setb %al`）163 为 4.4 最优，P197④ 封顶。
// ============================================================================
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
}