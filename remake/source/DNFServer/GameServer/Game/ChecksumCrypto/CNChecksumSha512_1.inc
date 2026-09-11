CNChecksumSha512::CNChecksumSha512(CNChecksumSha512 const&)
    : IChecksum(CHECKSUM_TYPE_SHA512), m_ctx(0)
{
}