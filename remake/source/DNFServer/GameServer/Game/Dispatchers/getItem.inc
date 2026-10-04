// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e756c orig/ours=76/73 ndiff-blocks=8
//   帧差 sub $60 vs $64 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x60,%esp | mov    %ax,-0xc(%ebp)
int Dispatcher_UseRenameCard::getItem(const CUser& user, int invenType, short slot,
                                      Inven_Item& out) const
{
    if (user.CheckItemLock(invenType, slot))
    {
        return 1;
    }

    Inven_Item slotItem = user.getCurCharacInvenR()->GetInvenSlot(invenType, slot);

    out = slotItem; // ORIG：0x3d 字节逐字段复制

    if (out.m_addInfo == 0)
    {
        return 1;
    }

    return 0;
}