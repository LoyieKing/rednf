void CUser::set_MailMsgData(MSG_MAILBOX_SEND* msg)
{
    m_pad8e101[0x2eb] = 0;                       // ORIG movb $0x0,0x8e3ec(%eax)
    memcpy(m_pad8e101, msg, 0x145);
