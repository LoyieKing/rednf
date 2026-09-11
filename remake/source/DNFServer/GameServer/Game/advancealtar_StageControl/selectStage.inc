// ----------------------------------------------------------------------------
// ORIG T 0x0812fa7a（docs/class_func_reports/advancealtar__StageControl/
// selectStage.md 全文，Prod_GlobalAdv 批次 2026-08-24）：
//   user 判空 → 3；!isAvailableTciket → 0xf5；!isEnterStage(stageId) → 8；
//   CharacData(+0x98)：m_field0 = getCurrentRidableId()、setUser(user)、
//     setSlotItem()；ProcStage::resetAll → startStage(stageId, diff)；
//   失败透传错误码；成功 → sendStageInfo() → sendMapInfo()【未建模批次跳过】
//     → setNotSurvivalModeFirstRound() → set_state(0xd) → 0。
// ----------------------------------------------------------------------------
// [b28d-solo-StageControl] selectStage：ORIG 78 / ours 83。已按 P183/P0 去掉
//   getCharacData()/getProcStage() out-of-line call 与具名指针槽（ORIG 每访问点重算
//   this+0x98 / this+0x4），并补 P198① `int err = 0; err = startStage(...)` 预清两句式。
//   残差 = N76：`m_field0 = getCurrentRidableId()` 4.4 恒 LHS-first（地址升格 %ebx 跨
//   call ⇒ push ebx/sub $0x24、+5 条），ORIG 4.1 为 RHS-first 直存 `mov %eax,0x98(%edx)`；
//   P211 档②/③（字段 unsigned 化、RHS (unsigned) cast）实测均不翻（4.4.6 同 LHS-first）。
//   另 setUser/startStage 实参寄存器角色 eax↔edx 互换（P75 跨版本）。语义逐条等价。
// [round1-L29 2026-09-10 复核] 判据① CLEAR。残差 = `mov this,%edx; mov %eax,0x98(%edx)`（ORIG
//   RHS-first）vs 我方 `push %ebx; mov this,%ebx; mov %eax,(%ebx)`（LHS-first+指针缓存，
//   N76/P183 反向不可补）；另 `lea 0x98(%eax),%edx` vs `add $0x98,%eax`（N107 六形探针全败）。
//   ORIG 0x0812fdd4 / 0x0812fe1c。
int StageControl::selectStage(CUser* user, int stageId,
                              advancealtar::StageDifficulty::T diff)
{
    if (user == 0)
    {
        return 3;
    }
    // [b28d-D9] ORIG 0x0812fdd4 段每次重算 `add $0x8df60,%eax`（P183 地址重物化，
    // 无具名引用槽）；具名 CharacAdvanceAltarManager& 使 4.4 落 -0x18 槽往返。
    if (!user->m_advanceAltar.isAvailableTciket())
    {
        return 0xf5;
    }
    if (!user->m_advanceAltar.isEnterStage(stageId))
    {
        return 8;
    }

    // [b28d-solo-StageControl] P183/P0：ORIG 每个访问点重算 this+0x98 / this+0x4
    // （无 getCharacData/getProcStage call、无具名指针槽）；逐点表达式直用。
    reinterpret_cast<CharacData*>(m_characData)->m_field0 =
        user->m_advanceAltar.getCurrentRidableId();
    reinterpret_cast<CharacData*>(m_characData)->setUser(user);
    reinterpret_cast<CharacData*>(m_characData)->setSlotItem();

    reinterpret_cast<ProcStage*>(m_procStage)->resetAll();
    // P198①：ORIG 在 startStage 前有 `movl $0x0,-0xc(%ebp)` 预清 ⇒ 两句式。
    int err = 0;
    err = reinterpret_cast<ProcStage*>(m_procStage)->startStage(stageId, diff);
    if (err != 0)
    {
        return err;
    }

    sendStageInfo();
    sendMapInfo();
    reinterpret_cast<ProcStage*>(m_procStage)->setNotSurvivalModeFirstRound();
    user->set_state((ch_state)0xd);
    return 0;
}