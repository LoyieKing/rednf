void md5_finish(md5_context* ctx, unsigned char* out)
{
    md5_ctx* c = (md5_ctx*)ctx;
    unsigned int used = (c->total[0] >> 3) & 0x3f;
    unsigned char pad[64] = {0x80};
    unsigned char lenbytes[8];
    unsigned int i;
    for (i = 0; i < 4; ++i)
    {
        lenbytes[i] = (unsigned char)(c->total[0] >> (i * 8));
        lenbytes[4 + i] = (unsigned char)(c->total[1] >> (i * 8));
    }
    md5_update(ctx, pad, used < 56 ? 56 - used : 120 - used);
    md5_update(ctx, lenbytes, 8);
    for (i = 0; i < 4; ++i)
    {
        out[i * 4] = (unsigned char)(c->state[i]);
        out[i * 4 + 1] = (unsigned char)(c->state[i] >> 8);
        out[i * 4 + 2] = (unsigned char)(c->state[i] >> 16);
        out[i * 4 + 3] = (unsigned char)(c->state[i] >> 24);
    }
