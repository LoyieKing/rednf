void Dispatcher_Write_On_The_GuildBoard::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：无条件读响应 +0x04 错误码字节并 SendCmdErrorPacket（无判 0 分支）。
    stGuildBoardParam& p = static_cast<stGuildBoardParam&>(param);
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x156, (unsigned char)p.m_4);
