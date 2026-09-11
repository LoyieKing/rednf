int Inter_AvatarInsert::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;
    // P324：ORIG 0x084cf5ce 的 `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)` 落在 guard 之后
    //   ⇒ 原码 pPkt 赋值语句位于 if 之后（物理序=书写序）。
    Inter_AvatarInsertPktViewI* pPkt = reinterpret_cast<Inter_AvatarInsertPktViewI*>(data);

    // P0/P183：ORIG 无 slot/addInfoKey/invenR/invenW/mgr 具名槽（0x084cf5de `lea -0xa(%eax),%ebx`
    //   跨 getCurCharacInvenR 调用保活 = 实参逆序求值升 callee-saved；0x084cf649 同形重算）。
    Inven_Item item = user->getCurCharacInvenR()->GetInvenSlot(2, (int)pPkt->m_8 - 10);

    user->getCurCharacInvenW()->GetAvatarItemMgrW()->AssignAvatarItemKey(
        item.m_addInfo2, pPkt->m_4b);
    item.m_addInfo2 = (int)pPkt->m_4b;

    // [b23S3C] P48：ORIG update_item 前重新 call getCurCharacInvenW（共 2 次），不复用。
    user->getCurCharacInvenW()->update_item((INVEN_TYPE)2, (int)pPkt->m_8 - 10, item);
    return 0;
}