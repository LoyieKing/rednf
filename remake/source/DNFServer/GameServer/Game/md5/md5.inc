void md5(unsigned char* data, int len, unsigned char* digest)
{
    md5_context ctx;
    md5_starts(&ctx);
    md5_update(&ctx, data, len);
    md5_finish(&ctx, digest);
    memset(&ctx, 0, 0xd8);
