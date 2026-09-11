// ORIG _ZN10expert_job17IsExistRecipeItemEP5CUserP5CItemb @0x0849b820：
// onlyInputs=true 时只校验输入材料，否则校验完整材料清单。
// b28d-solo-ExpertJobOnlyMaterial: AE — clear 移入两分支各自开头（ORIG
// 0x0849b845/0x0849b8b8 各一次）；else 分支 recipe 具名局部（-0xc 槽，
// P0）；两分支各自 CheckNeedItemList（ORIG 两个调用点）；P292 档① bool 化。
// 返回类型 int→bool（P292 档① callee 侧）：ORIG 调用点 _DoEnchant
// 0x0849cc58 `call; xor $1,%eax; test %al,%al; je` 无 setne ⇒ callee bool；
// 返回类型不入 mangled，函数体 return 0/1 不变（S370 同步改 CExpertJob.h:450）。
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
}