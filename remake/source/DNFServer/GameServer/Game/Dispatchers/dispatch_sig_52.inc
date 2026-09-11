int Dispatcher_DisguiseCancel::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x821b496：返回值 pseudo 缓存 %ebx（各 return 点 mov $0,%ebx → merge mov %ebx,%eax），
    // 入口无初始化（init 沉到各 return 点赋值）。
    register int ret;
    if (user == 0)
    {
        return ret = 0;
    }
    if (user->get_state() != 3)
    {
        return ret = 0;
    }
    if (!user->isDisguiseCharac())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd7, 3);
        // ORIG also executes the setter again after this branch.
        user->setDisguiseCharac(false, 0, 0);
    }
    user->setDisguiseCharac(false, 0, 0);
    PacketGuard notification;
    notification.put_header(0, 2);
    notification.put_byte(0);
    notification.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&notification), 0);
    notification.finalize(true);
    G_GameWorld()->send_all(notification, user);
    return ret = 0;
}