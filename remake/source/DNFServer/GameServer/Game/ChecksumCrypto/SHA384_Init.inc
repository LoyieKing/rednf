void SHA384_Init(SHA384_ALG_INFO* ctx)
{
    ctx->state[0] = 0xcbbb9d5dc1059ed8ULL; ctx->state[1] = 0x629a292a367cd507ULL;
    ctx->state[2] = 0x9159015a3070dd17ULL; ctx->state[3] = 0x152fecd8f70e5939ULL;
    ctx->state[4] = 0x67332667ffc00b31ULL; ctx->state[5] = 0x8eb44a8768581511ULL;
    ctx->state[6] = 0xdb0c2e0d64f98fa7ULL; ctx->state[7] = 0x47b5481dbefa4fa4ULL;
    ctx->total[3] = 0;               // ORIG 0x080c288c 清零链：3→2→1→0
    ctx->total[2] = ctx->total[3];
    ctx->total[1] = ctx->total[2];
    ctx->total[0] = ctx->total[1];
}