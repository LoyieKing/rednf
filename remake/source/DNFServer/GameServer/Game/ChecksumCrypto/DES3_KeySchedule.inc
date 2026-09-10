int DES3_KeySchedule(unsigned char* key, unsigned long keyLen,
                     DES3_ALG_INFO* alg)
{
    if (keyLen != 16 && keyLen != 24)
        return AES_ERR_BAD_KEYLEN;
    DES_ALG_INFO tmp;
    int rc = DES_KeySchedule(key, 8, &tmp);
    if (rc != 0)
        return rc;
    std::memcpy(alg->rk, tmp.rk, 0x80);
    rc = DES_KeySchedule(key + 8, 8, &tmp);
    if (rc != 0)
        return rc;
    std::memcpy(alg->rk + 0x80, tmp.rk, 0x80);
    if (keyLen == 24)
        rc = DES_KeySchedule(key + 0x10, 8, &tmp);
    else
        rc = DES_KeySchedule(key, 8, &tmp);
    if (rc != 0)
        return rc;
    std::memcpy(alg->rk + 0x100, tmp.rk, 0x80);
    return 0;
