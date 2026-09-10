void Dispatcher_Enchant_3rdChronicleItem::send(CUser* user, ParamBase& param)
{
    stEnchantParam& out = static_cast<stEnchantParam&>(param);
    if (out.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x173,
                                 (unsigned char)(out.m_4 & 0xff));
    }
    return;   // ORIG 0x081d3880: call 后 jmp merge + nop 尾岛（技巧7）
