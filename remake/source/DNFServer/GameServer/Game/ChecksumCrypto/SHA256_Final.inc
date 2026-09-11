void SHA256_Final(SHA256_ALG_INFO* ctx, unsigned char* out)
{   // ORIG 0x080be1d1（103 条）：位计数定填充位点，长度域 64 位大端（hi 前 lo 后），
    // 尾块经 ctx->buffer 就地补齐后压缩，摘要按大端序列化输出。
    unsigned long lo = ctx->total[0];
    unsigned long hi = ctx->total[1];
    unsigned long pos = (lo >> 3) & 0x3f;
    ctx->buffer[pos] = 0x80;
    pos += 1;
    if (pos > 0x38) {
        std::memset(ctx->buffer + pos, 0, 0x40 - pos);
        SHA256_Transform(reinterpret_cast<unsigned long*>(ctx->buffer),
                         reinterpret_cast<unsigned long*>(ctx));
        std::memset(ctx->buffer, 0, 0x38);
    } else {
        std::memset(ctx->buffer + pos, 0, 0x38 - pos);
    }
    sha256StoreBE(hi, ctx->buffer + 0x38);
    sha256StoreBE(lo, ctx->buffer + 0x3c);
    SHA256_Transform(reinterpret_cast<unsigned long*>(ctx->buffer),
                     reinterpret_cast<unsigned long*>(ctx));
    for (int i = 0; i < 8; ++i)
        sha256StoreBE(ctx->state[i], out + i * 4);
}