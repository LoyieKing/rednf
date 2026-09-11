// ORIG _ZN13user_creature12CCreatureMgr16SetOverSkillTimeERKi T 0x0833c41e
// （SetOverSkillTime.md）：死亡宠免检返 1；匹配装备宠 item 时校验脚本/等级/
// 冷却窗（cur+2 < last+st/1000+ovst/1000 → trace 0xe6c 返 0x13），通过则
// COverSkillTime::SetTime() 刷新；其余路径 0。
// [b28d-solo-CreatureOnlyMgrRest] verdict=N 封顶（DIFF，语义完整，p167 28/28
// CLEAR）：P196 比较方向翻正（ORIG `cmp %eax,%ebx; setg` = 累加值在左）+
// P0 去 learnLevel/expLevel/lastUsed/stMs/ovstMs/curSec 具名局部 + P23/P24
// 求值序（itemIdx 写左侧）+ P227 临时链 trace1 + trace 段按 ORIG 槽集重写
// （ovstR/stR 存 **除 1000 后**的值 @-0x48/-0x44、interval@-0x40、last@-0x3c，
// trace 第 2 实参用 last 槽 —— 修掉 EXTRA GetOverSkillTime/GetTime 各 1 次）。
// 残差 = N115 帧 0x7c vs 0x6c（全槽深 0x10）+ N97 有符号 /1000 lowering
// （`mov %ecx,%eax` 中转数差 2）。
int CCreatureMgr::SetOverSkillTime(const int& itemIdx)
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
{
    if (m_equippedCreature != 0)
    {
        if (m_equippedCreature->IsDieCreature())
            return 1;

        // b28d-61 [P167 MISSING `CCreatureItem::GetItemId()`]：ORIG 0x0833c5xx
        // `call GetItemId; cmp %eax,%ebx; sete` 为真实调用，旧形直读 +0x04 字段。
        // CCreature 为镜像类（基区 +0x00..+0x1b 与 CCreatureItem 一致），
        // 按 CCreature.cpp 既有 P201 做法经 reinterpret_cast 调用权威方法。
        // b28d-solo-CreatureOnlyMgrRest [P23/P24 求值序]：ORIG 0x0833c451 先
        // `mov 0xc(%ebp),%eax; mov (%eax),%ebx` 载 itemIdx，后 call GetItemId，
        // 尾 `cmp %eax,%ebx; sete` ⇒ 源码把 itemIdx 写左侧。
        if (itemIdx == reinterpret_cast<CCreatureItem*>(m_equippedCreature)->GetItemId())
        {
            if (!m_equippedCreature->IsCreatureScript())
            {
                // [P227]：ORIG 0x0833c486 在 ctor 前把 itemIdx 载入 %ebx 保活。
                cMyTrace(__PRETTY_FUNCTION__, 0xe62, 0)(
                    "SetOverSkillTime::creature_script is null(creature_itemno=%u)",
                    itemIdx);
                return 0x13;
            }

            // b28d-solo-CreatureOnlyMgrRest [P0/P196]：ORIG 0x0833c4d8
            // learnLevel→%ebx、expLevel 无槽（`cmp %eax,%ebx; setg`）⇒ 内联
            // 表达式且左操作数先求值。
            if (m_equippedCreature->GetLearnOverSkillLevel() >
                m_equippedCreature->GetExpLevel(
                    m_equippedCreature->GetExp()))
                return 0x13;

            // b28d-solo-CreatureOnlyMgrRest [P196/P0]：ORIG 0x0833c528 起
            // lastUsed→%ebx 后 `add %eax,%ebx` 连加两次，尾 `cmp %eax,%ebx;
            // setg`（累加值在左）⇒ 累加表达式写左、内联无局部。
            if (*m_equippedCreature->GetOverSkillTime()->GetTime() +
                    m_equippedCreature->GetSkillRecoveryTime() / 1000 +
                    m_equippedCreature->GetOverSkillRecoveryTime() / 1000 >
                GlobalData::s_systemTime_.getCurSec() + 2)
            {
                // [L2Free4to15.S3] 全部 vararg 内联（去 4 个具名局部）：
                // ORIG 0x0833c5a8..c67e 证明求值序 = 实参右→左（ch_no→m_id→
                // ovstR→stR→interval→last→curSec，各经调用间 spill 落
                // -0x48/-0x44/-0x40/-0x3c），即源码无具名局部；具名形把
                // 4 个数值提到 ctor 前 ⇒ 求值序错位 + 帧 +0x10。
                // interval 与 last 各自独立重调 GetTime()（P48 无 CSE）。
                cMyTrace(__PRETTY_FUNCTION__, 0xe6c, 0)(
                    "fail:cur_time(%u),last_used_skilltime(%u),interval(%d) "
                    "st(%d),ovst(%d),m_id(%s),ch_no(%u)",
                    (unsigned int)GlobalData::s_systemTime_.getCurSec(),
                    (unsigned int)*m_equippedCreature->GetOverSkillTime()
                        ->GetTime(),
                    GlobalData::s_systemTime_.getCurSec() -
                        *m_equippedCreature->GetOverSkillTime()->GetTime(),
                    m_equippedCreature->GetSkillRecoveryTime() / 1000,
                    m_equippedCreature->GetOverSkillRecoveryTime() / 1000,
                    NumberToString(m_user->get_acc_id(), 0),
                    (unsigned int)m_user->getCurCharacNo());
                return 0x13;
            }
            m_equippedCreature->GetOverSkillTime()->SetTime();
        }
    }
    return 0;
}