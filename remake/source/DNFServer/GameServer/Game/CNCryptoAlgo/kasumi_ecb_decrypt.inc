int kasumi_ecb_decrypt(const unsigned char *ct, unsigned char *pt, symmetric_key *skey)
{
    ulong32 left, right, temp;
    int n;

    LTC_ARGCHK(pt   != NULL);
    LTC_ARGCHK(ct   != NULL);
    LTC_ARGCHK(skey != NULL);

    LOAD32H(left, ct);
    LOAD32H(right, ct+4);

    for (n = 7; n >= 0; ) {
        temp = FO(right, n,   skey);
        temp = FL(temp,  n--, skey);
        left ^= temp;
        temp = FL(left,  n,   skey);
        temp = FO(temp,  n--, skey);
        right ^= temp;
    }

    STORE32H(left, pt);
    STORE32H(right, pt+4);

    return CRYPT_OK;
}