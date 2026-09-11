void Dispatcher_WarRoomWpPerMonster::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stWarRoomWp& p = static_cast<stWarRoomWp&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x179, (unsigned char)p.m_4);
}