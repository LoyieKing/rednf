ByteBuffer NCashImpl::set_data_header(unsigned char packetId, unsigned char flag)
{
    ByteBuffer out(reinterpret_cast<char*>(m_outMsg.body()), 0x200);
    out << packetId;
    out << flag;
    _increaseRequestID();
    out << m_requestID;
    return out;
