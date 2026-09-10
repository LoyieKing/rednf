CodeHackCheckRangeEx::CodeHackCheckRangeEx()              // T @0x0808a966
{
    std::memset(m_fileName, 0, 0x20);                     // memset(this,0,0x20) 仅前 0x20
    std::memset(&m_start104, 0, 8);                       // memset(this+0x104,0,8)
