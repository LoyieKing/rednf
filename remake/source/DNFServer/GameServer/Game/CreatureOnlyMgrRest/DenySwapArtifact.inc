// ORIG _ZN13user_creature12CCreatureMgr16DenySwapArtifactEP10Inven_ItemiiS2_ii
// T 0x0833989c（DenySwapArtifact.md，条件序以汇编为准）：
// 有装备宠：slot 组 (s1,b)/(s3,a) 任一满足 IsArtifactEquipSlot && !b.isEmpty()
// 且装备宠 IsAvailableArtifact(s1) 为假 → deny(1)；否则 0。
// 无装备宠：任一组 IsArtifactEquipSlot && !isEmpty(b/a) → allow(0)；否则 1。
// [b28d-solo-CreatureOnlyMgrRest] verdict=IDENTICAL/AE：**真语义 bug 修正**
// （无装备宠分支 ORIG 0x08339990 无 `xor $1` ⇒ 条件为 b->isEmpty()/a->isEmpty()，
// 旧码 `!isEmpty()` 极性写反）+ P226 语句表达式零槽复现 `&&` 物化岛 +
// N93附 反向共享尾（block A return false 提为函数尾，复现 ORIG
// `mov $1,%eax; jmp; mov $0,%eax` 尾块序）。
bool CCreatureMgr::DenySwapArtifact(Inven_Item* a, int s1, int s2,
                                    Inven_Item* b, int s3, int s4)
{
    (void)s2;   // ORIG 未读 +0x14
    (void)s4;   // ORIG 未读 +0x20

    if (m_equippedCreature != 0)
    {
        // b28d-solo-CreatureOnlyMgrRest [P226/P0]：ORIG ok1/ok2 无栈槽
        // （`mov $1/jmp` + `mov $0` + `test %al,%al; je` 寄存器归一化）
        // ⇒ 语句表达式包 `&&` 短路，勿用具名 bool（4.4 槽化 + cmpb）。
        if (({ IsArtifactEquipSlot(s1) && !b->isEmpty(); }))
        {
            if (!m_equippedCreature->IsAvailableArtifact(s1))
                return true;
        }
        if (({ IsArtifactEquipSlot(s3) && !a->isEmpty(); }))
        {
            if (!m_equippedCreature->IsAvailableArtifact(s1))
                return true;   // ORIG 两处 IsAvailableArtifact 均传 +0x10(s1)
        }
    }
    else
    {
        // b28d-solo-CreatureOnlyMgrRest [真语义 bug 修正]：ORIG 0x08339990
        // 无 `xor $1`（0x083398d1 block A 才有）⇒ 无装备宠分支条件为
        // `b->isEmpty()`/`a->isEmpty()`，旧码 `!isEmpty()` 极性写反。
        if (({ IsArtifactEquipSlot(s1) && b->isEmpty(); }))
            return false;
        if (({ IsArtifactEquipSlot(s3) && a->isEmpty(); }))
            return false;
        return true;
    }
    // b28d-solo-CreatureOnlyMgrRest [N93附 反向]：ORIG 尾块序为
    // `mov $1,%eax; jmp 尾; mov $0,%eax; leave; ret`（block B ret1 在前、
    // block A 的 ret0 落共享尾）⇒ 把 block A 的 return false 提为共享尾。
    return false;
}