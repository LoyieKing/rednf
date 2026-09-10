void CUser::set_MultimailMsgData(MSG_MULTI_MAILBOX_SEND* data)
{
    m_pad8e101[0x2eb] = 1;                                 // ORIG movb $1,0x8e3ec
    memcpy(m_pad8e101 + 0x145, data, 0x1a6);               // ORIG memcpy(this+0x8e246,..)
