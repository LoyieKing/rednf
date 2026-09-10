void CodeHackChecksumEx::SetCodeHackCheckRange(const CodeHackCheckRange& range)  // T @0x0808a7d8
{
    m_range = range;                                     // ORIG 内联 2 mov（0x8(%eax)/0x4(%eax)→+0x110/+0x114），不 memcpy
