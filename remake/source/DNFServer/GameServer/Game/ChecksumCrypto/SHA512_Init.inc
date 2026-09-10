void SHA512_Init(SHA512_ALG_INFO* ctx)
{
    ctx->state[0] = 0x6a09e667f3bcc908ULL; ctx->state[1] = 0xbb67ae8584caa73bULL;
    ctx->state[2] = 0x3c6ef372fe94f82bULL; ctx->state[3] = 0xa54ff53a5f1d36f1ULL;
    ctx->state[4] = 0x510e527fade682d1ULL; ctx->state[5] = 0x9b05688c2b3e6c1fULL;
    ctx->state[6] = 0x1f83d9abfb41bd6bULL; ctx->state[7] = 0x5be0cd19137e2179ULL;
    ctx->total[3] = 0;               // ORIG 0x080c288c 清零链：3→2→1→0
    ctx->total[2] = ctx->total[3];
    ctx->total[1] = ctx->total[2];
    ctx->total[0] = ctx->total[1];
