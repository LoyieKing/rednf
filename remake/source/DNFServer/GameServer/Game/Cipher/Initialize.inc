int Cipher::Initialize(const unsigned char* key, int keyLen)
{
    std::memcpy(m_keyBuf, key, keyLen);
    for (int i = 0; i < CRYPTO_GRAPH_MAX; ++i)
    {
        int ks = m_graphs[i]->GetKeySize();
        // [b28c-L9] ORIG 0x08097357: body fall-through, return 0 outer island.
        if (keyLen >= ks)
        {
            m_graphs[i]->Initialize(key, ks);
            key += ks;
            keyLen -= ks;
        }
        else
        {
            return 0;
        }
    }
    return 1;
}
