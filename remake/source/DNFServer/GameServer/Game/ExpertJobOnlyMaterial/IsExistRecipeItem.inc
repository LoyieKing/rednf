bool IsExistRecipeItem(CUser* user, CItem* recipe, bool onlyInputs)
{
    std::vector<std::pair<int, int> > materials;

    if (onlyInputs)
    {
        materials.clear();
        GetRecipeInputItems(recipe, materials);
        user->CalculateExpertJobMaterials(materials);
        if (materials.empty())
            return 0;
        if (!user->getCurCharacInvenR()->CheckNeedItemList(materials))
            return 0;
    }
    else
    {
        materials.clear();
        // b28d-solo-ExpertJobOnlyMaterial：ORIG 0x0849b8bd 把 recipe 存入
        // -0xc 具名槽后再判空（P0：有槽 ⇒ 原码有具名局部）。
        CItem* item = recipe;
        if (item == 0)
            return 0;
        materials = reinterpret_cast<CStackableItem*>(item)
                        ->m_recipe174.m_vec0;
        user->CalculateExpertJobMaterials(materials);
        if (!user->getCurCharacInvenR()->CheckNeedItemList(materials))
            return 0;
    }
    return 1;
