void CPacketRespondent::SetError(int errorCode)
{
    clear();
    put_header(m_header, m_subHeader);
    put_byte(0);
    put_byte(errorCode);
    finalize(true);
