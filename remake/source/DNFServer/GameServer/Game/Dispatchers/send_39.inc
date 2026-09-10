void Dispatcher_CreateDisjointStore::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stCreateDisjointStoreParam& p = static_cast<stCreateDisjointStoreParam&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xee, (unsigned char)p.m_4);
