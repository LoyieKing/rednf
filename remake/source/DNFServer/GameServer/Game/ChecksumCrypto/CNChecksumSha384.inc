CNChecksumSha384::CNChecksumSha384()
    : IChecksum(CHECKSUM_TYPE_SHA384), m_ctx(0)
{
    // ORIG 0x8098aca new 0xd0（标量 operator new）——对齐 _Znwj
    m_ctx = (unsigned char*)::operator new(0xd0);
