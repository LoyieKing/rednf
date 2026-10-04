void Dispatcher_SecretShopBuyItem::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081d27ec T：param 指针→-0xc 槽；p->m_4==0x7fffffff/0 → 不发包（je end 共享），
    // 否则 SendCmdErrorPacket(0x129, (unsigned char)p->m_4)（真值 test 形）。
    stSecretShopBuyItemParam* p = static_cast<stSecretShopBuyItemParam*>(&param);
    if (p->m_4 == (int)0x7fffffff)
        return;
    if (p->m_4)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, (unsigned char)p->m_4);
}