// ORIG 0x0869475e W _ZNK14CStackableItem26IsUse2ndUpgradeTableRecipeEv：
// movzbl 0xd0(%eax) —— 返回 CItem 基类 +0xd0 字节（m_boolD0）
bool CStackableItem::IsUse2ndUpgradeTableRecipe() const
{
    return m_boolD0;
}