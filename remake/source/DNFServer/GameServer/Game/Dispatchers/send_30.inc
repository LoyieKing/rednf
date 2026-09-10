void Dispatcher_GuildCargoPopItem::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081d1748 T：err=*(int*)((char*)&param+4)；0 → 静默；0x64/0xb5 →
    // SendCmdErrorPacket(0x109,(unsigned char)err)；其余 → SendCmdErrorPacket(0xfc,…)。
    // b28c-33：ORIG 把 &param 存入 -0xc 具名局部并逐处重读 +0x04（P0 槽往返），
    // 值缓存形少 4 条重读 ⇒ 改具名视图指针局部。
    stGuildCargoPopItemParam* p = static_cast<stGuildCargoPopItemParam*>(&param);
    if (p->m_4 != 0)
    {
        if (p->m_4 == 0x64 || p->m_4 == 0xb5)
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x109, (unsigned char)p->m_4);
        else
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfc, (unsigned char)p->m_4);
    }
