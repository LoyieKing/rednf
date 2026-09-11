// ORIG _ZN10expert_job17ExpertJobMaterial14GetEnchantItemEv @0x0849c6ca。
CItem* ExpertJobMaterial::GetEnchantItem()
{
    if (Empty())
        return 0;
    if (IsRecipe() != 0)
        return 0;
    // ORIG 0x0849c6ca：inline helper get_recipe_materials 被 4.1 内联为
    // `mov 0x4(%eax),%eax; add $0x174,%eax`（m_item→CStackableItem+RecipeInfo
    // @+0x174 首向量 +0）。4.4 -O0 不外联该 inline ⇒ 退化 call；源形直接展开成员访问。
    return FindMonsterCard(
        reinterpret_cast<CStackableItem*>(m_item)->m_recipe174.m_vec0);
}