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
