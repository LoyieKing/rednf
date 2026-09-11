// ORIG SetError @ 0x08340538：clear → put_header(+0x10,+0x14) → put_byte(0)
// → put_byte(errCode) → finalize(true)。
void CPacketRespondent::SetError(int errorCode)
{
    clear();
    put_header(m_header, m_subHeader);
    put_byte(0);
    put_byte(errorCode);
    finalize(true);
}