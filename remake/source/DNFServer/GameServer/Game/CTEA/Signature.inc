// ORIG _ZN4CTEA9SignatureEPc T @0x080c35ec：
//   char buf[23] 内建清零；strcpy "TEA"；接 m_key(m_keyLength)；sprintf "%d%d"
//   拼 m_mode/m_padding；CSHA 摘要写出。
void CTEA::Signature(char* szSignature)
{
    char buf[0x17];
    std::memset(buf, 0, sizeof(buf));
    std::strcpy(buf, "TEA");
    const int headLen = static_cast<int>(std::strlen(buf));
    std::memcpy(buf + headLen, m_key, m_keyLength);
    std::sprintf(buf + headLen + m_keyLength, "%d%d", m_mode, m_padding);

    CSHA sha;
    sha.AddData(buf, static_cast<int>(std::strlen(buf)));
    sha.FinalDigest(szSignature);
}