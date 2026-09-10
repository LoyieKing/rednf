void BN_BLINDING_free(void* blinding)                // ORIG T 0x088007b0
{
    void** b = static_cast<void**>(blinding);
    if (b == 0)
        return;
    for (int i = 0; i < 4; ++i)                      // BN_free ×4（就地：裸指针
        if (b[i] != 0)                               // 无外链依赖）
            CRYPTO_free(b[i]);
    CRYPTO_free(blinding);                           // ORIG 尾 jmp CRYPTO_free
