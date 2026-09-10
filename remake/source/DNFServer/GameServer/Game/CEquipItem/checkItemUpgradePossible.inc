bool CEquipItem::checkItemUpgradePossible() const
{
    if (IsImpossibleUpgrade())
    {
        return 0;
    }
    // [L23-r3 试形记录·回退] ORIG 0x085138b2 单次 `mov 0x234(%eax),%eax` 后在
    //   %eax/%ecx 复用；试 `register unsigned int itemType = (unsigned int)m_itemType;`
    //   后实测仍 36 条（= 试前，ORIG 34）⇒ 4.4 寄存器复用不可造（N18a 族），已回退。
    //   [b28e 试形记录] P226 语句表达式形（`({unsigned int t=(unsigned)m_itemType;
    //   t<0x16 && ((1<<t)&0x3ff400U)!=0;})`）fresh 实测 **41 条**（t 落槽 →
    //   `mov %eax,-slot` + 两次回读，比内联形 36 更劣）⇒ 回退，维持内联形。
    if ((unsigned int)m_itemType < 0x16 &&
        ((1 << (unsigned int)m_itemType) & 0x3ff400U) != 0)
    {
        return 1;
    }
    return 0;
