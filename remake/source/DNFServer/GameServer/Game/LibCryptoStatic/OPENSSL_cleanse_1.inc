// ---- OPENSSL_cleanse（ORIG T 0x087f92b0：volatile 清零；语义等价）----
void OPENSSL_cleanse(void* ptr, size_t len)
{
    volatile unsigned char* p = static_cast<volatile unsigned char*>(ptr);
    for (size_t i = 0; i < len; ++i)
        p[i] = 0;
}