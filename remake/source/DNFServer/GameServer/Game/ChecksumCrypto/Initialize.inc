int CNChecksumMd5::Initialize(unsigned char const* /*key*/, int /*keyLen*/)
{
    return CKS_ERR_DATALEN;   // ORIG 0x080983e0：恒返 0x70000017
