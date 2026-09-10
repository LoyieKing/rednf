int CRYPTO_add_lock(int* pointer, int amount, int type,
                    const char* file, int line)
{
    (void)file; (void)line;
    CRYPTO_lock(9, type, 0, 0);
    const int ret = *pointer + amount;
    *pointer = ret;
    CRYPTO_lock(10, type, 0, 0);
    return ret;
