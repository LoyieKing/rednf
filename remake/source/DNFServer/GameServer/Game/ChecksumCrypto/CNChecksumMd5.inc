CNChecksumMd5::CNChecksumMd5()
    : IChecksum(CHECKSUM_TYPE_MD5),
      m_ctx(0)
{
    m_ctx = new unsigned char[0xd8];   // ORIG operator new(0xd8)：md5_context 视图块
