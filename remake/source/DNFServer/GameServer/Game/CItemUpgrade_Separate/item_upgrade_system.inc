void IItemUpgrade::item_upgrade_system(CUser* user, MSG_ITEM_UPGRADE* msg, int cmd)
{
    int result = 0;
    PacketGuard pg;
    pg.put_header(1, cmd);
    result = this->ProcUpgrade(user, msg, &pg);
    while (result == 0)
    {
        pg.finalize(true);
        user->Send(pg);
        return;
    }
    user->SendCmdErrorPacket2Byte(static_cast<ENUM_CMDPACKET>(cmd),
                                  static_cast<unsigned short>(result & 0xffff));
