bool ItemVendingMachine::_CheckEnableAvatarCondition(const CCeraShopGoods* goods,
                                                     char c, unsigned char uc,
                                                     stAvatarTypeSelect_t& out)
{
    // [b28d-solo-GlobalData] ORIG 0x081767a6：首次 GetRelatedItem 结果**无槽往返**
    // （`call; mov (%eax),%edx; add $0x10,%edx; mov (%edx),%edx; mov %eax,(%esp);
    // call *%edx`）⇒ 原码内联 `goods->GetRelatedItem()->IsAvatarItem()`（P0）；
    // 具名 related 局部会多 store/load 并把后续槽整体顶深。
    if (!goods->GetRelatedItem()->IsAvatarItem())
        return false;
    out.init();
    CEquipItem* equip = static_cast<CEquipItem*>(goods->GetRelatedItem());
    // ORIG 0x08176794 段（p167 MISSING `operator[](unsigned int) const`×4）：
    // 原码是 const 引用视图 ⇒ 声明 const 指针转型（P270 旋钮）。
    // ORIG 0x081767e4..0x081767f2 只有 equip(-0x10)/select(-0xc) 两个槽，
    // 中间无 vec 具名指针 ⇒ 内联 cast（P183 去缓存）。
    const std::vector<stAvatarTypeSelect_t>& select =
        *static_cast<const std::vector<stAvatarTypeSelect_t>*>(
            equip->getAvatarTypeSelect());
    // ORIG 0x0817680f `cmp -0x1c(%ebp),%al; jg` ⇒ 8 位有符号比较
    // （`(signed char)size() > c`；P287 同族），无具名 idx 局部。
    if (select.empty() || (signed char)select.size() <= c)
        return false;
    if (uc >= 100)
        return false;
    out.m_fieldc = select[(signed char)c].m_fieldc;
    out.m_field4 = select[(signed char)c].m_field4;
    out.m_field8 = select[(signed char)c].m_field8;
    out.m_field0 = select[(signed char)c].m_field0;
    return true;
