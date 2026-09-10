int CCreatureMgr::SetOverSkillTime(const int& itemIdx)
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
                // b28d-solo-CreatureOnlyMgrRest [P0/P44/P227]：ORIG 槽集 =
                // ovstR(-0x48, **已除 1000**)/stR(-0x44, **已除 1000**)/
                // interval(-0x40)/last(-0x3c)；curSec/chNo/idStr 无槽（vararg
                // 右起求值经 %ebx/%edi/%esi 跨 ctor 保活）⇒ 除法写进局部、
                // 其余内联实参 + 全表达式临时链。
                int ovstR = m_equippedCreature->GetOverSkillRecoveryTime() / 1000;
                int stR = m_equippedCreature->GetSkillRecoveryTime() / 1000;
                int interval = GlobalData::s_systemTime_.getCurSec() -
                               *m_equippedCreature->GetOverSkillTime()->GetTime();
                int last = *m_equippedCreature->GetOverSkillTime()->GetTime();
                cMyTrace(__PRETTY_FUNCTION__, 0xe6c, 0)(
                    "fail:cur_time(%u),last_used_skilltime(%u),interval(%d) "
                    "st(%d),ovst(%d),m_id(%s),ch_no(%u)",
                    (unsigned int)GlobalData::s_systemTime_.getCurSec(),
                    (unsigned int)last,
                    interval, stR, ovstR,
                    NumberToString(m_user->get_acc_id(), 0),
                    (unsigned int)m_user->getCurCharacNo());
                return 0x13;
            }
            m_equippedCreature->GetOverSkillTime()->SetTime();
        }
    }
    return 0;
