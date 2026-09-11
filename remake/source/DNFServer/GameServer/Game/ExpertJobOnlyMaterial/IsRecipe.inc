// ORIG _ZN10expert_job17ExpertJobMaterial8IsRecipeEv @0x0849c114：
// 材料必须是 stackable 且 GetItemType()==3（配方类，vtable slot3 @+0x0c）。
// b28c-1 N7: 虚调用 callee %eax↔%edx 互换。
int ExpertJobMaterial::IsRecipe()
{
    // b28：P0/P183 去缓存——ORIG 每次调用独立物化 m_item 载入（无具名局部槽），
    // vcall this 参数二次重读成员。
    if (!reinterpret_cast<CItem*>(m_item)->is_stackable())
        return 0x17;
    if (reinterpret_cast<CItem*>(m_item)->GetItemType() != 3)
        return 0x17;
    return 0;
}