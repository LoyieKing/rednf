// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081a0612 orig/ours=188/187 ndiff-blocks=19
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: movl   $0x0,-0x14(%ebp) | lea    -0x14(%ebp),%eax | mov    -0x14(%ebp),%eax
int Dispatcher_RequestPrcoomDayilyReward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x7e8;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    // ORIG：<3 与 >4 各重读一次 get_state（P48 无 CSE）。
    if (user->get_state() < 3 || user->get_state() > 4)
    {
        return 0;
    }
    if (user->m_pcroomDailyRemain < 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x259, 0x16);
        return 0;
    }
    int itemId = 0;
    if (packet.get_int(itemId) != 1)
    {
        return 0x7fb;
    }
    if (itemId != 0x212f && itemId != 0x2130)
    {
        return 0x800;
    }
    CItem* item = G_CDataManager()->find_item(itemId);
    if (item == 0 || item->GetItemType() != 0)
    {
        return 0;
    }
    Inven_Item inven;
    inven.m_addInfo = item->get_index();
    item->make_item(inven);
    inven.set_add_info(1);
    CInventory* inv = user->getCurCharacInvenW();
    int slot = inv->insertItemIntoInventory(
        inven, (eItemAddReason)0xe, true, true);
    if (slot < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x259, 4);
    }
    else
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x259);
        user->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0, slot);
        user->m_pcroomDailyRemain = (char)(user->m_pcroomDailyRemain - 1);
        InterSelectPcroomDailyReward::SendPacket(user);
        DBUpdatePcroomDailyReward::makeRequest(user);   // ORIG static（无 ctor/dtor）
    }
    return 0;
}