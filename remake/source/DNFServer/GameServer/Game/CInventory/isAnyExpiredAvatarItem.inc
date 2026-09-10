bool CInventory::isAnyExpiredAvatarItem() const
{
    // b28d-37：ORIG 0x850937e 两循环各带 1 个 nop 岛（je 落 +0x64/+0xd0 nop，
    // 后两条守卫 je/jge 落 +0x65/+0xd1 addl）。P223 定案①复现形=负条件 goto 到
    // 共享尾空标签串（标签序=守卫序），使 4.4 在标签处发 nop 中继。
    int now = OS_API::GetDateTimeTick();
    for (int i = 0; i <= 9; i++)
    {
        if (m_invenItem[i].m_addInfo == 0) goto L1;
        if (m_invenItem[i].m_addInfo2 != 0 && m_invenItem[i].m_addInfo2 < now)
            return true;
    L1:;
    }
    for (int i = 0; i <= 0x68; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0) goto L2;
        if (m_pAvatarSlot[i].m_addInfo2 != 0 && m_pAvatarSlot[i].m_addInfo2 < now)
            return true;
    L2:;
    }
    return false;
