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
