void SHA256_Update(SHA256_ALG_INFO* ctx, unsigned char* in, unsigned long len)
{
    // ORIG _Z13SHA256_UpdateP15SHA256_ALG_INFOPhm T @0x080be08a（102 条）：
    // total[0/1] 为**位计数**（buffered=(total[0]>>3)&0x3f，0x080be093 实证），
    // 4 字节对齐输入直压 Transform（0x080be164），否则经 ctx->buffer 中转。
    unsigned long buffered = (ctx->total[0] >> 3) & 0x3f;
    unsigned long copy = 64 - buffered;
    ctx->total[0] = ctx->total[0] + len * 8;
    if (ctx->total[0] < len * 8)
        ctx->total[1] = ctx->total[1] + 1;
    ctx->total[1] = ctx->total[1] + (len >> 29);
    if (len >= copy) {
        std::memcpy(ctx->buffer + buffered, in, copy);
        SHA256_Transform(reinterpret_cast<unsigned long*>(ctx->buffer),
                         reinterpret_cast<unsigned long*>(ctx));
        in += copy;
        len -= copy;
        buffered = 0;
        while (len > 0x3f) {
            if ((reinterpret_cast<unsigned long>(in) & 3) == 0) {
                SHA256_Transform(reinterpret_cast<unsigned long*>(in),
                                 reinterpret_cast<unsigned long*>(ctx));
            } else {
                std::memcpy(ctx->buffer, in, 0x40);
                SHA256_Transform(reinterpret_cast<unsigned long*>(ctx->buffer),
                                 reinterpret_cast<unsigned long*>(ctx));
            }
            in += 0x40;
            len -= 0x40;
        }
    }
    std::memcpy(ctx->buffer + buffered, in, len);
