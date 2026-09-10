bool COpensslRSA::Encrypt(const char* in, int len, std::vector<unsigned char>& out)
{
    int rsa_size = RSA_size(m_rsa);
    int max_chunk = rsa_size - 0xc;
    int pos = 0;
    // [b28d-solo-D02] 同 Decrypt：ORIG 0x0816c3cb..0x0816c3e0 为 MIN_EXPR+i486 形。
    int chunk = (len - pos > max_chunk) ? max_chunk : (len - pos);
    while (chunk > 0)
    {
        std::vector<unsigned char> block;
        block.resize(static_cast<size_t>(rsa_size), 0);
        int written = RSA_private_encrypt(chunk,
                                          reinterpret_cast<const unsigned char*>(in) + pos,
                                          &block[0], m_rsa, 1);
        if (written < 1)
        {
            return false;
        }
        // [L10-2] 同 Decrypt（ORIG 0x0816c47x 段同形：指针版 insert + 2×operator[]）。
        out.insert(out.end(), &block[0], &block[0] + written);
        pos += chunk;
        chunk = (len - pos > max_chunk) ? max_chunk : (len - pos);
    }
    return true;
