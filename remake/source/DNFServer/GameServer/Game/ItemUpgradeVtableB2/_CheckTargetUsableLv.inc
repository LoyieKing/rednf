bool CItemUpgrade::_CheckTargetUsableLv(const Inven_Item& material,
                                        const Inven_Item& target) const
{
    // [b28d-64] P194：ORIG 各出口直接 `mov $0/$1,%eax; jmp end`（0x08548c69/
    // 0x08548c86/0x08548ca7/…），无汇聚 bool 槽；汇聚形（-0x19）令每出口
    // 改走 `movb $..,-0x19; jmp` 且末尾多 movzbl（+5 条）。
    CItem* matScript = G_CDataManager()->find_item(material.m_addInfo);
    if (matScript == 0)
    {
        return false;
    }
    if (!matScript->is_stackable())
    {
        return true;
    }
    const CStackableItem* stack = (const CStackableItem*)matScript;
    // [b28d-64] P248 档②：字段 bool 化后 `if (!field)` 直产 ORIG 0x08548c9c
    // `movzbl 0x320; xor $1; test %al; je`（char 字段恒折 `cmp $0x1,%al; je`，gap 1）。
    if (!stack->m_char320)
    {
        return true;
    }
    CItem* tgtScript = G_CDataManager()->find_item(target.m_addInfo);
    if (tgtScript == 0)
    {
        return false;
    }
    if (!target.isEquipableItemType())
    {
        return false;
    }
    // ORIG 0x08548cf0 `mov -0x1c,%eax; mov %eax,-0x18` 具名副本槽，再取
    // [m_int318, m_int31c] 区间与 getUsableLevel 比较。
    const CItem* usableScript = tgtScript;
    const int minLv = stack->m_int318;
    const int maxLv = stack->m_int31c;
    const int usable = usableScript->getUsableLevel();
    // [b28d-64] P272/P222：ORIG 0x08548d14 `mov -0x14; cmp -0xc; jg fail` +
    // `mov -0xc; cmp -0x10; jle ok` ⇒ 左操作数写 minLv、第二项写 usable>maxLv。
    if (minLv > usable || usable > maxLv)
    {
        return false;
    }
    return true;
