int Dispatcher_ChangeGuildName::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082134a8 [0x082134a8, 0x082138f1]
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return LineFunc(0xc15f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned short slot;                                    // -0x1e（ORIG 不预清零）
    if (!packet.get_short(slot))
    {
        return LineFunc(0xc163,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned char space;                                    // -0x1f（ORIG 不预清零）
    if (!packet.get_byte(space))
    {
        return LineFunc(0xc166,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->get_charac_guildkey() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb6, 100);
        return 0;
    }
    if (!user->isSetGuildCreateFlag(2))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb6, 0x6d);
        return 0;
    }
    if (!user->IsGuildMaster())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb6, 0x56);
        return 0;
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xf);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb6, (unsigned char)secu);
        return 0;
    }
    if (user->CheckItemLock(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), (int)slot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb6, 0xd5);
        return 0;
    }
    Inven_Item item = user->getCurCharacInvenR()->GetInvenSlot(1, (int)slot);
    if (item.m_addInfo != 0x2e9)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb6, 0x11);
        return 0;
    }
    // ORIG 0x0821377b：delete_item 返回 1（成功）→ 继续；0（失败）→ 报错 0xb6/0x11。
    if (!user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1, (int)slot, 1, (eItemDelReason)3, true))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb6, 0x11);
        return 0;
    }
    PacketGuard guard;
    guard.put_header(0, 0xe);
    guard.put_byte(0);
    guard.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, (int)slot, guard);
    guard.finalize(true);
    user->Send(guard);
    // ORIG 段直证：三实参分别保活于 %edi/%esi/%ebx（0xc/0x8/0x4(%esp)），proxy 无槽
    // ⇒ P0/P183 全表达式链（原具名 name/guildkey/curNo/proxy 落 -0x18/-0x14/-0x10/-0xc
    // 四槽，帧 0x8c→0x90、push 集缺 %edi）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendGuildNameChange((unsigned int)user->getCurCharacNo(),
                              (unsigned int)user->get_charac_guildkey(),
                              user->getCreateGuildName());
    user->resetGuildCreateFlag();
    user->resetTempGuildNameAddr();
    return 0;
