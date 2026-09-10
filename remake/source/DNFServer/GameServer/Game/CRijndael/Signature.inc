void CRijndael::Signature(char* szSignature)
{
    // ORIG：SHA-256("RIJDAEL" + key + sprintf("%d%d%d%d", bs, kl, mode, pad))
    char buf[0x40];
    std::memset(buf, 0, sizeof(buf));
    std::strcat(buf, "RIJDAEL");
    int len = (int)std::strlen(buf);
    std::memcpy(buf + len, m_key, m_keyLength);
    std::sprintf(buf + len + m_keyLength, "%d%d%d%d",
                 m_blockSize, m_keyLength, m_mode, m_padding);

    nsl::CSHA sha;
    sha.AddData(buf, (int)std::strlen(buf));
    sha.FinalDigest(szSignature);
