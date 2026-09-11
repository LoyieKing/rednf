CNChecksumMd5::CNChecksumMd5()
    : IChecksum(CHECKSUM_TYPE_MD5),
      m_ctx(0)
{
    // ORIG 0x0809830c：标量 `_Znwj`(0xd8)，非 new[]（p167：我方曾发 `_Znaj`）
    m_ctx = (unsigned char*)::operator new(0xd8);   // md5_context 视图块
}