int Store::GetSellItemPrice(Inven_Item& item, const CItem* pItem, short count,
                            bool flag, int& outPrice)
{
    // ORIG 0x08618ebc：`call isEquipableItemType; xor $1; test %al,%al; je <装备路>`
    // ⇒ 源码 `if (!item.isEquipableItemType())`（callee 返回 bool，P292/P168a①）。
    if (!item.isEquipableItemType())
    {
        // ORIG 0x08618ed7：`cmp %edx,%eax; jl <true>; cmpw $0,-0x2c; jg <false>;
        // <true> mov $1; jmp; <false> mov $0; test; je <body>` ⇒ 负极性早返
        // （P59 guard 形：fail 块直落在前，body 在 je 目标处）。
        if ((int)item.get_add_info() < (int)count || count <= 0)
            return 0x11;
        // ORIG 0x08618f37 后 `movswl -0x2c(%ebp),%edx; imul %eax,%edx`
        // ⇒ 源码把 getItemSellPrice(...) 写在乘法左侧（call 后读 count 槽）。
        outPrice = getItemSellPrice(pItem->GetSellPrice(), 0, 0,
                                    pItem->IsRandomOption()) * count;
    }
    else
    {
        const CEquipItem* pEquip = static_cast<const CEquipItem*>(pItem);
        // ORIG 0x08618f5c：先 `cmp $0xb,%eax; je <0x17>` 再 `cmp $9; jg <继续>`
        // ⇒ `m_itemType == 0xb || m_itemType < 10`。
        if (pEquip->m_itemType == 0xb || pEquip->m_itemType < 10)
            return 0x17;
        // ORIG 0x08618f79：比较对象是 item.m_fieldb（+0xb，ushort），非 count。
        if ((int)item.m_fieldb > (int)pEquip->get_endurance())
            item.m_fieldb = (unsigned short)pEquip->get_endurance();
        outPrice = getItemSellPrice(pItem->GetSellPrice(), item.m_fieldb,
                                    pEquip->get_endurance(),
                                    pItem->IsRandomOption());
    }
    return 0;
