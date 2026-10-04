void Dispatcher_InformNotice::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e3056 T：param 指针→-0xc 槽；p->m_4==0 → SendCmdOkPacket(0x1de)，
    // 否则 SendCmdErrorPacket(同码, (unsigned char)p->m_4)（字段二次单级读）。
    InformNoticeData* p = static_cast<InformNoticeData*>(&param);
    if (p->m_4 == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1de);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1de, (unsigned char)p->m_4);
}