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

    // [P167 2026-09-11] ORIG 0x80b6a9e 调全局 `_ZN4CSHA7AddDataEPKci`/`FinalDigestEPc`/
    // `C1Ev`（ChecksumAlgo.h 的 CSHA），我方原用 nsl::CSHA ⇒ 三个符号全错（EXTRA nsl 版）。
    // 两类均为 SHA-256 但实现/符号不同，按 ORIG 改用全局 CSHA。
    CSHA sha;
    sha.AddData(buf, (int)std::strlen(buf));
    sha.FinalDigest(szSignature);
}