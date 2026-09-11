// ORIG _ZN10expert_job19UseMaterialOfRecipeEP5CUserP5CItemP11PacketGuardb
// @0x0849bcfb。
// b28d-solo-ExpertJobOnlyMaterial: AE — 删 onlyInputs 分支多余的
// materials.clear()（ORIG C1Ev 后直接 GetRecipeInputItems）。
int UseMaterialOfRecipe(CUser* user, CItem* recipe, PacketGuard* packet,
                        bool onlyInputs)
{
    bool changed = false;
    if (onlyInputs)
    {
        std::vector<std::pair<int, int> > materials;
        GetRecipeInputItems(recipe, materials);
        if (materials.empty())
            return 0x11;
        if (!DelMaterialOfRecipe(user, materials, 0, packet, changed, 0))
            return 0x11;
    }
    else
    {
        if (!DelMaterialOfRecipe(user,
                                 reinterpret_cast<CStackableItem*>(recipe)
                                     ->m_recipe174.m_vec0, 0,
                                 packet, changed, 0))
            return 0x11;
    }

    if (changed)
        user->send_equip(0x16);
    return 0;
}