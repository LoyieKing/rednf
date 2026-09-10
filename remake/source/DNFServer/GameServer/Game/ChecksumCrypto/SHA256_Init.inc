void SHA256_Init(SHA256_ALG_INFO* ctx)
{
    ctx->state[0] = 0x6a09e667u; ctx->state[1] = 0xbb67ae85u;
    ctx->state[2] = 0x3c6ef372u; ctx->state[3] = 0xa54ff53au;
    ctx->state[4] = 0x510e527fu; ctx->state[5] = 0x9b05688cu;
    ctx->state[6] = 0x1f83d9abu; ctx->state[7] = 0x5be0cd19u;
    ctx->total[1] = 0;
    ctx->total[0] = ctx->total[1];   // ORIG 0x080be075：先写 hi 再置 lo=hi(=0)
    std::memset(ctx->buffer, 0, sizeof(ctx->buffer));
