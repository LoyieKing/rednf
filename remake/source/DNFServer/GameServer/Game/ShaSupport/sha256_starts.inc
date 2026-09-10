void sha256_starts(sha256_context* ctx)
{
    ctx->m_total[0] = 0;
    ctx->m_total[1] = 0;
    ctx->m_state[0] = 0x6a09e667UL;
    ctx->m_state[1] = 0xbb67ae85UL;
    ctx->m_state[2] = 0x3c6ef372UL;
    ctx->m_state[3] = 0xa54ff53aUL;
    ctx->m_state[4] = 0x510e527fUL;
    ctx->m_state[5] = 0x9b05688cUL;
    ctx->m_state[6] = 0x1f83d9abUL;
    ctx->m_state[7] = 0x5be0cd19UL;
