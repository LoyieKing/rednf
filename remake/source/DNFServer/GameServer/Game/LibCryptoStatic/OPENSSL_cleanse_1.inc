void OPENSSL_cleanse(void* ptr, size_t len)
{
    volatile unsigned char* p = static_cast<volatile unsigned char*>(ptr);
    for (size_t i = 0; i < len; ++i)
        p[i] = 0;
