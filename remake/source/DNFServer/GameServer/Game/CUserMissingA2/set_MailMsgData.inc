// ----------------------------------------------------------------------------
// ORIG T 0x0868a4b6 _ZN5CUser15set_MailMsgDataEP16MSG_MAILBOX_SEND：
// movb $0,0x8e3ec（mail 标志清 0）+ memcpy(this+0x8e101, msg, 0x145)。
// 0x8e101 起块视图沿用 m_pad8e101（0x2eb=0x8e3ec-0x8e101，先例见 CUserOnlyOrig）。
// ----------------------------------------------------------------------------
void CUser::set_MailMsgData(MSG_MAILBOX_SEND* msg)
{
    m_pad8e101[0x2eb] = 0;                       // ORIG movb $0x0,0x8e3ec(%eax)
    memcpy(m_pad8e101, msg, 0x145);
}