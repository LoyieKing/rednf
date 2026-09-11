// ===========================================================================
// class CodeHackChecksumEx
// ===========================================================================
CodeHackChecksumEx::CodeHackChecksumEx()                  // T @0x0808a700
{
    std::memset(m_fileName, 0, sizeof(m_fileName));       // memset(this+0xc,0,0x104)
    std::memset(&m_range, 0, sizeof(m_range));            // memset(this+0x110,0,8)
    // ORIG 0x0808a74f..0x0808a766：memset 后对 m_range 两 int 预清直写（0x114 先/0x110 后）。
    m_range.m_size = 0;
    m_range.m_start = 0;
}