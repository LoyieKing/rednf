bool CDBMsgSender::UseKeepRight(int uid, int flag)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x80;
    buf->m_addInfo2 = uid;               // +0x30：宠物 uid
    buf->m_keepRight = (unsigned char)flag;  // +0x52
    return true;
