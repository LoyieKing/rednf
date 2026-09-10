void CPacketRespondent::SetOk()
{
    clear();
    put_header(m_header, m_subHeader);
    put_byte(1);
    finalize(true);
