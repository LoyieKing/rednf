bool CInventory::removeDupliacteEquipItem()
{
    // ORIG 0x85034b5：i=-0x14、found=-0x10（具名槽 cmpl $-1,-0x10 技巧9）、
    // characNo=-0xc（0x8503523 先 spill 后 je 直落 0 形，非三目）；
    // itemIdx 跨 cMyTrace ctor 存活于 %ebx（0x8503562 mov 0x2(%eax),%ebx）。
    // b28c-L2：清除历史 register asm("ebx") 绑定（P0 硬约束），改 P227 全表达式
    // 临时链令 4.4 在 ctor 前求 varargs 并以 %ebx 跨 ctor 保活。
    int i;
    int found;
    for (i = 3; i <= 0xc8; i++)
    {
        // ORIG 0x85034d9 +0x2b: field1!=1 → jne +0x120（4.1 continue 岛 jmp+nop）；
        // b28c-L2：原码为「提前 continue」形（两个 continue 同层），4.4 才发射
        // ORIG 的 `jmp <addl>; nop` 岛（探针 l2_probe5 H 形命中）。
        if (m_pEquipSlot[i].m_field1 != 1)
        {
            continue;
        }
        found = getExistEquipItem(m_pEquipSlot[i].m_addInfo,
                                  m_pEquipSlot[i].m_addInfo2, i);
        if (found == -1)
        {
            continue;   // ORIG 0x850352a +0x7c: je → +0x123 岛
        }
        // ORIG 0x8503530..+0x9f：两分支均产 %eax、merge 单点 spill -0xc。
        // b28c-L2：ORIG else 分支 `mov $0x0,%eax` 后与 call 结果共用 merge 点
        // 0x850354d `mov %eax,-0xc(%ebp)` ⇒ 原码为单点赋值的三目（非 if/else 各自写槽）。
        int characNo = (m_pParent != 0)
                           ? ((CUserCharacInfo*)m_pParent)->getCurCharacNo()
                           : 0;
        cMyTrace(__PRETTY_FUNCTION__, 0x16d2, 5)("DUPLICATE ITEM REMOVED %d at %d of %d",
                                                 m_pEquipSlot[i].m_addInfo, i, characNo);
        m_pEquipSlot[i].reset();
        return 1;
    }
    return 0;
}