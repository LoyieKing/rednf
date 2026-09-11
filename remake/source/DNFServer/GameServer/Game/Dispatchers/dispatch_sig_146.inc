// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x08229768 orig/ours=130/132 ndiff-blocks=18
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x24,%esp | movl   $&_ZZN33Dispatcher_VerifyPrivateStoreItem12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | mov    &_ZN10G
int Dispatcher_VerifyPrivateStoreItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xe740,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int secuErr = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x3d);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x55, (unsigned char)(secuErr & 0xff));
        return 0;
    }
    if (user->isHackUser() || user->isHackUserTimer())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x195, 0x11);
        return 0;
    }
    short slot = 0;
    int itemIdx = 0;
    private_store::IBuyer* buyer = 0;
    if (!packet.get_short(slot))
    {
        return LineFunc(0xe76a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(itemIdx))
    {
        return LineFunc(0xe76b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (itemIdx < 0)
    {
        return LineFunc(0xe76c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [P0] ORIG 0x08229768（size 0x212）：`call GetInstancePrivateStoreMgr; mov %eax,(%esp)` 直传第 1 参数
    // （无 mgr 具名槽；我方原写具名指针使帧 0x24→0x34、secuErr 槽 -0xc→-0x10）⇒ 链式内联。
    private_store::GetInstancePrivateStoreMgr()->GetStoreBuyer(&buyer, user, (CUser*)0);
    buyer->VerifyPrivateStoreItem((int)slot, itemIdx);
    return 0;
}