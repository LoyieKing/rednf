int kasumi_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
    static const u16 C[8] = { 0x0123,0x4567,0x89AB,0xCDEF, 0xFEDC,0xBA98,0x7654,0x3210 };
    u16 ukey[8], Kprime[8];
    int n;

    LTC_ARGCHK(key  != NULL);
    LTC_ARGCHK(skey != NULL);

    if (keylen != 16) {
       return CRYPT_INVALID_KEYSIZE;
    }

    if (num_rounds != 0 && num_rounds != 8) {
       return CRYPT_INVALID_ROUNDS;
    }

    /* Start by ensuring the subkeys are endian correct on a 16-bit basis */
    for (n = 0; n < 8; n++ ) {
        ukey[n] = (((u16)key[2*n]) << 8) | key[2*n+1];
    }

    /* Now build the K'[] keys */
    for (n = 0; n < 8; n++) {
        Kprime[n] = ukey[n] ^ C[n];
    }

    /* Finally construct the various sub keys */
    for(n = 0; n < 8; n++) {
        skey->kasumi.KLi1[n] = ROL16(ukey[n],1);
        skey->kasumi.KLi2[n] = Kprime[(n+2)&0x7];
        skey->kasumi.KOi1[n] = ROL16(ukey[(n+1)&0x7],5);
        skey->kasumi.KOi2[n] = ROL16(ukey[(n+5)&0x7],8);
        skey->kasumi.KOi3[n] = ROL16(ukey[(n+6)&0x7],13);
        skey->kasumi.KIi1[n] = Kprime[(n+4)&0x7];
        skey->kasumi.KIi2[n] = Kprime[(n+3)&0x7];
        skey->kasumi.KIi3[n] = Kprime[(n+7)&0x7];
    }

    return CRYPT_OK;
}