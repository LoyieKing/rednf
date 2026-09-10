int kasumi_ecb_encrypt(const unsigned char *pt, unsigned char *ct, symmetric_key *skey)
{
    ulong32 left, right, temp;
    int n;

    LTC_ARGCHK(pt   != NULL);
    LTC_ARGCHK(ct   != NULL);
    LTC_ARGCHK(skey != NULL);

    LOAD32H(left, pt);
    LOAD32H(right, pt+4);

    for (n = 0; n <= 7; ) {
        temp = FL(left,  n,   skey);
        temp = FO(temp,  n++, skey);
        right ^= temp;
        temp = FO(right, n,   skey);
        temp = FL(temp,  n++, skey);
        left ^= temp;
    }

    STORE32H(left, ct);
    STORE32H(right, ct+4);

    return CRYPT_OK;
