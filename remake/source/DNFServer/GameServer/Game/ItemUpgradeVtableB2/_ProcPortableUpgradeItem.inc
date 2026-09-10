int CItemUpgrade::_ProcPortableUpgradeItem(const Inven_Item& material,
                                           const Inven_Item& target,
                                           upgrade_info_t& info) const
{
    static const int MaxPortableItemActionIntCount = 2;  // ORIG _ZZNK8WongWork12CItemUpgrade24_ProcPortableUpgradeItem…E29MaxPortableItemActionIntCount
    CItem* script = G_CDataManager()->find_item(material.m_addInfo);
    if (script == 0)
    {
        return 4;
    }
    if (!script->is_stackable())
    {
        return 0;
    }
    // [b28d-solo-ItemUpgradeVtableB2] P96/P270：ORIG 调 **const** 版
    // `_ZNKSt6vectorIiSaIiEEixEj` ⇒ 源码该指针是 const（verifyStackableAction
    // 已声明 const，CStackableItem.h:441）。
    const CStackableItem* stack = (const CStackableItem*)script;
    if (!stack->verifyStackableAction((StackableAction::Type)0x35))
    {
        return 0;
    }
    // [b28d-64] P0/P183：ORIG 0x08548dd9 无 vector 引用局部（引用引入 -0x18 指针槽，
    // 使后续槽整体下移 4）；直接成员表达式访问。up 槽 ORIG 0x08548e1e 为
    // `mov %al,-0x9`（单字节槽）⇒ 局部类型 unsigned char，比较处 (int) 提宽。
    if (stack->m_vec3a0.size() != MaxPortableItemActionIntCount)
    {
        return 4;
    }
    const int minUp = stack->m_vec3a0[0];
    const int maxUp = stack->m_vec3a0[1];
    const unsigned char up = target.GetUpgrade();
    if ((int)up < minUp || (int)up > maxUp)
    {
        return 0x13;
    }
    info.m_discountBase = 0;            // +0x34（具名字段）
    info.m_meterialCount = 1;           // +0x30（具名字段）
    info.m_recipeItem = material.m_addInfo;  // +0x2c（具名字段）
    return 0;
