CNChecksumSha256::CNChecksumSha256()
    : IChecksum(CHECKSUM_TYPE_SHA256), m_ctx(0)
{
    m_ctx = new unsigned char[0x70];
