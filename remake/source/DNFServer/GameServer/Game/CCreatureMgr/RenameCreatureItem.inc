// ============================================================================
//   m_hdr|=0x200；+0x30(m_addInfo2)=uid；name 拷入 +0x34（m_pad34 名字区，截 0xc）；
//   返回 1。hdr 位/name 位置依同族（UseFeedCreature|0x2 等）推断，无独立报告。
//   （ORIG asm 833fbe9..833fc1a 无 buf 判空、无 name 判空。）
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
}
