bool CDBMsgSender::RenameCreatureItem(int uid, const char* name)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x1;
    buf->m_addInfo2 = uid;                            // +0x30
    // ORIG asm 833fc1a：lea 0x34(%eax),%edx 直接传 dst（无中间变量），strncpy 0xc。
    strncpy(buf->m_name, name, 0xc);
    return true;
