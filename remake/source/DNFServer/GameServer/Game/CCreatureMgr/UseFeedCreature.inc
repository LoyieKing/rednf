bool CDBMsgSender::UseFeedCreature(int uid, int stomachValue)
{
    if (m_stream == 0)
        return false;
    // ORIG 0x833fc82：GetInBuffer 结果无判空，直接 memset（buf 槽 -0xc）。
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    buf->m_hdr |= 0x2;
    buf->m_addInfo2 = uid;             // +0x30：UseFeedCreature 消息承载宠物 uid
    buf->m_stomachValue = stomachValue;  // +0x44
    return true;
