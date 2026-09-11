CodeHackChecksumEx::~CodeHackChecksumEx()                 // T @0x0808a76c
{
    m_checksums.clear();                                  // ORIG 直接 vector::clear
}