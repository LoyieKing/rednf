// ORIG _ZN10expert_job17ExpertJobMaterial19UseMaterialOfRecipeEP11PacketGu
// ard @0x0849c536：套用配方，删全部材料 + 本体；changed 时 send_equip(0x16)。
// b28d-solo-ExpertJobOnlyMaterial: AE — P0 删 m_user 具名缓存（逐点重读）；
// P171 删 changed 初始化（ORIG 只读 -0xd）；P225 while+break 槽比零物化。
int ExpertJobMaterial::UseMaterialOfRecipe(PacketGuard* packet)
{
    if (Empty())
        return 0x11;

    // b28d-36：ORIG 0x0849c556 前置 `movl $0x0,-0xc` 预清（P198-①）+
    // `cmpl $0; setne %al; test %al,%al; je` 槽比零物化（P225 while+break 形）。
    int recipeErr = 0;
    recipeErr = IsRecipe();
    while (recipeErr != 0)
    {
        return recipeErr;
    }

    // b28d-solo-ExpertJobOnlyMaterial（P0）：ORIG 0x0849c59b/0x0849c5e9 均重读
    // m_user（无 user 具名槽）⇒ 删局部，每使用点内联。
    if (reinterpret_cast<CUser*>(m_user)->getCurCharacR() == 0)
        return 3;

    // b28d-solo-ExpertJobOnlyMaterial（P171）：ORIG 0x0849c5e9 只读 -0xd 槽，
    // 无 `movb $0` 初始化 ⇒ 原码 `bool changed;` 未初始化（出参由被调写入）。
    bool changed;
    if (!DelMaterialOfRecipe(reinterpret_cast<CUser*>(m_user),
                             reinterpret_cast<CStackableItem*>(m_item)
                                 ->m_recipe174.m_vec0,
                             static_cast<int>(m_slot), packet, changed))
        return 0x11;

    if (changed)
        reinterpret_cast<CUser*>(m_user)->send_equip(0x16);
    return 0;
}