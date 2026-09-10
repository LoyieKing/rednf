void SHA384_Final(SHA384_ALG_INFO* ctx, unsigned char* out)
{   // ORIG 0x080c062e：与 SHA512_Final 同构，摘要取前 6 个 state（48 字节）。
    unsigned long buffered = (ctx->total[0] >> 3) & 0x7f;
    reinterpret_cast<unsigned char*>(ctx->buffer)[buffered] = 0x80;
    buffered += 1;
    if (buffered > 0x70) {
        std::memset(reinterpret_cast<unsigned char*>(ctx->buffer) + buffered, 0, 0x80 - buffered);
        SHA384_Transform(reinterpret_cast<unsigned long*>(ctx->buffer),
                         reinterpret_cast<unsigned long*>(ctx));
        std::memset(ctx->buffer, 0, 0x78);
    } else {
        std::memset(reinterpret_cast<unsigned char*>(ctx->buffer) + buffered, 0, 0x78 - buffered);
    }
    sha512StoreBE(((unsigned long long)ctx->total[3] << 32) | ctx->total[2],
                  reinterpret_cast<unsigned char*>(ctx->buffer) + 0x70);
    sha512StoreBE(((unsigned long long)ctx->total[1] << 32) | ctx->total[0],
                  reinterpret_cast<unsigned char*>(ctx->buffer) + 0x78);
    SHA384_Transform(reinterpret_cast<unsigned long*>(ctx->buffer),
                     reinterpret_cast<unsigned long*>(ctx));
    for (int i = 0; i < 6; ++i)               // SHA-384 取前 48 字节
        sha512StoreBE(ctx->state[i], out + i * 8);
