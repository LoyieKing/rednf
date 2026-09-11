CNChecksumSha256::CNChecksumSha256(CNChecksumSha256 const&)
    : IChecksum(CHECKSUM_TYPE_SHA256), m_ctx(0)
{
}