int ENGINE_finish(void* engine)                      // ORIG T 0x087eb4a0
{
    void** e = static_cast<void**>(engine);
    if (e == 0)
        return 1;
    CRYPTO_lock(9, 30, "eng_lib.c", 145);            // file 串 0x8de22d2
    const int ret = engine_unlocked_finish_kl(e);
    CRYPTO_lock(10, 30, "eng_lib.c", 147);
    if (!ret)
        ERR_put_error(38, 107, 150, "eng_lib.c", 151);   // ORIG reason 0x96
    return ret;
