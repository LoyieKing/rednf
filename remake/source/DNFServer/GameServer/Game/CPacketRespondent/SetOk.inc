// ORIG SetOk @ 0x083405a2：clear → put_header(+0x10,+0x14) → put_byte(1)
// → finalize(true)。
void CPacketRespondent::SetOk()
{
    clear();
    put_header(m_header, m_subHeader);
    put_byte(1);
    finalize(true);
}