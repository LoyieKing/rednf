void Dispatcher_GuildCargoMoveItem::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081d1a96 T：err=*(int*)((char*)&param+4)；0 → 静默；0x64/0xb5 →
    // SendCmdErrorPacket(0x109,(unsigned char)err)；其余 → SendCmdErrorPacket(0xfd,…)。
    // b28c-33：同 GuildCargoPopItem::send，ORIG -0xc 存 &param 并逐处重读 +0x04。
    stGuildCargoMoveItemParam* p = static_cast<stGuildCargoMoveItemParam*>(&param);
    if (p->m_4 != 0)
    {
        if (p->m_4 == 0x64 || p->m_4 == 0xb5)
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x109, (unsigned char)p->m_4);
        else
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfd, (unsigned char)p->m_4);
    }
