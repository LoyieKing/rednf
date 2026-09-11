void Dispatcher_HatchCreature::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stHatchCreature& p = static_cast<stHatchCreature&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x69, (unsigned char)p.m_4);
}