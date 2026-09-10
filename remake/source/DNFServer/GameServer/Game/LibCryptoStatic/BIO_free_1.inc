int BIO_free(void* bio)
{
    bio_st_st* b = static_cast<bio_st_st*>(bio);
    if (b == 0)
        return 0;
    if (CRYPTO_add_lock(&b->references, -1, 20, "bio_lib.c", 117) <= 0)
        return 1;                                    // 计数<=0 → 返 1
    if (b->callback != 0)                            // callback(BIO_CB_FREE)
        b->callback(b, 1, 0, 0, 0L, 1L);
    CRYPTO_free_ex_data(0, b, &b->ex_data_sk);       // class 0 占位、对象 bio
    if (b->method != 0 && b->method->destroy != 0)
        b->method->destroy(b);
    CRYPTO_free(b);
    return 1;
