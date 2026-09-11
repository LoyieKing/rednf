void BN_clear_free(void* bn)                         // ORIG T 0x087fbe80
{
    BIGNUM_V* a = static_cast<BIGNUM_V*>(bn);
    if (a == 0)
        return;
    if (a->d != 0)
        OPENSSL_cleanse(a->d, static_cast<size_t>(a->dmax) * 4);
    if (a->flags & 0x2)                              // flags bit2: 结构清零
        OPENSSL_cleanse(a, sizeof(BIGNUM_V));        // ORIG cleanse(a,0x14)
    if (!(a->flags & 0x2))                           // 非 STATIC_DATA → free
        CRYPTO_free(a);
}