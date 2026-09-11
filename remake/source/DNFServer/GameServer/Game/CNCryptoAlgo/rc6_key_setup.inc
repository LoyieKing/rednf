void rc6_key_setup(RC6_ALG_INFO* alg, int keylen)
{
    unsigned char* K = alg->K;                 // +0x20
    const unsigned char* key = alg->key;
    unsigned int L[8] = { 0 };
    int keyWords = (keylen + 3) / 4;           // ORIG 80b5b60..74（向上取整）
    int n = keyWords - 1;
    L[n] = 0;                                  // ORIG 80b5b77（仅清 L[n]）
    for (int i = keylen - 1; i >= 0; --i)      // ORIG 80b5b8a..d8（大端字节聚合）
        L[i / 4] = (L[i / 4] << 8) | key[i];

    K[0] = 0x63;                               // ORIG 80b5bdd
    for (int i = 1; i <= 0x2b; ++i)            // ORIG 80b5be0..0d
        K[i] = (unsigned char)(K[i - 1] - 0x47);

    unsigned int A = 0, B = 0;
    int i = 0, j = 0;
    int rounds = 0x2c;                         // ORIG 80b5c28
    if (keyWords > rounds)                     // ORIG 80b5c43..5c
        rounds = keyWords;
    rounds *= 3;                               // ORIG 80b5c5f..68

    for (int r = 1; r <= rounds; ++r)
    {
        // K[i] = (byte)rol32(K[i]+B+A, 3) —— ORIG 80b5c77..92（ror 0x1d）
        unsigned int x = (unsigned int)K[i] + B + A;
        K[i] = (unsigned char)((x << 3) | (x >> 29));
        B = K[i];                              // ORIG 80b5c94..9a
        // L[j] = rotl32(L[j]+B+A, (B+A)&0x1f) —— ORIG 80b5c9d..cf3
        unsigned int y = L[j] + B + A;
        unsigned int sh = (B + A) & 0x1f;
        L[j] = sh ? rc6_rol32(y, sh) : y;
        A = L[j];                              // ORIG 80b5cf3..f7
        i = (i + 1) % 0x2c;                    // ORIG 80b5cfa..27
        j = (j + 1) % keyWords;                // ORIG 80b5d29..4e
    }
}