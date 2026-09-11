CNChecksumSha256::CNChecksumSha256()
    : IChecksum(CHECKSUM_TYPE_SHA256), m_ctx(0)
{
    // ORIG 0x080986b8：标量 `_Znwj`(0x70)，非 new[]
    m_ctx = (unsigned char*)::operator new(0x70);
}