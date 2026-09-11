void SHA512_Update(SHA512_ALG_INFO* ctx, unsigned char* in, unsigned long len)
{
    // ORIG _Z13SHA512_UpdateP15SHA512_ALG_INFOPhm T @0x080c28b9（148 条）：
    // 位计数 + 三级进位链；对齐输入直压 Transform，否则经 ctx->buffer 中转。
    unsigned long buffered = (ctx->total[0] >> 3) & 0x7f;
    unsigned long copy = 128 - buffered;
    ctx->total[0] = ctx->total[0] + len * 8;
    if (ctx->total[0] < len * 8) {
        ctx->total[1] = ctx->total[1] + 1;
        if (ctx->total[1] == 0) {
            ctx->total[2] = ctx->total[2] + 1;
            if (ctx->total[2] == 0)
                ctx->total[3] = ctx->total[3] + 1;
        }
    }
    ctx->total[1] = ctx->total[1] + (len >> 29);
    if (ctx->total[1] < (len >> 29)) {
        ctx->total[2] = ctx->total[2] + 1;
        if (ctx->total[2] == 0)
            ctx->total[3] = ctx->total[3] + 1;
    }
    if (len >= copy) {
        std::memcpy(reinterpret_cast<unsigned char*>(ctx->buffer) + buffered, in, copy);
        SHA512_Transform(reinterpret_cast<unsigned long*>(ctx->buffer),
                         reinterpret_cast<unsigned long*>(ctx));
        in += copy;
        len -= copy;
        buffered = 0;
        while (len > 0x7f) {
            if ((reinterpret_cast<unsigned long>(in) & 3) == 0) {
                SHA512_Transform(reinterpret_cast<unsigned long*>(in),
                                 reinterpret_cast<unsigned long*>(ctx));
            } else {
                std::memcpy(ctx->buffer, in, 0x80);
                SHA512_Transform(reinterpret_cast<unsigned long*>(ctx->buffer),
                                 reinterpret_cast<unsigned long*>(ctx));
            }
            in += 0x80;
            len -= 0x80;
        }
    }
    std::memcpy(reinterpret_cast<unsigned char*>(ctx->buffer) + buffered, in, len);
}