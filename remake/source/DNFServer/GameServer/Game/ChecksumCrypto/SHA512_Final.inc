void SHA512_Final(SHA512_ALG_INFO* ctx, unsigned char* out)
{   // ORIG 0x080c2a7e（102 条）：位计数定填充位点，128 位长度域大端
    //（total[3]:total[2] 在 +0x70，total[1]:total[0] 在 +0x78）。
    unsigned long buffered = (ctx->total[0] >> 3) & 0x7f;
    reinterpret_cast<unsigned char*>(ctx->buffer)[buffered] = 0x80;
    buffered += 1;
    if (buffered > 0x70) {
        std::memset(reinterpret_cast<unsigned char*>(ctx->buffer) + buffered, 0, 0x80 - buffered);
        SHA512_Transform(reinterpret_cast<unsigned long*>(ctx->buffer),
                         reinterpret_cast<unsigned long*>(ctx));
        std::memset(ctx->buffer, 0, 0x78);
    } else {
        std::memset(reinterpret_cast<unsigned char*>(ctx->buffer) + buffered, 0, 0x78 - buffered);
    }
    sha512StoreBE(((unsigned long long)ctx->total[3] << 32) | ctx->total[2],
                  reinterpret_cast<unsigned char*>(ctx->buffer) + 0x70);
    sha512StoreBE(((unsigned long long)ctx->total[1] << 32) | ctx->total[0],
                  reinterpret_cast<unsigned char*>(ctx->buffer) + 0x78);
    SHA512_Transform(reinterpret_cast<unsigned long*>(ctx->buffer),
                     reinterpret_cast<unsigned long*>(ctx));
    for (int i = 0; i < 8; ++i)
        sha512StoreBE(ctx->state[i], out + i * 8);
}