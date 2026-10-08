CNChecksumSha512::CNChecksumSha512(CNChecksumSha512 const&)
    // [DNF-FIX 2026-10-08 c24] ORIG 0x08098f5d movl $0x1: copy ctor passes CHECKSUM_TYPE_SHA256 (ORIG copy-paste quirk; 0 callers)
    : IChecksum(CHECKSUM_TYPE_SHA256), m_ctx(0)
{
}