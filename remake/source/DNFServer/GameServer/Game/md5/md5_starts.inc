void md5_starts(md5_context* ctx)
{
    md5_ctx* c = (md5_ctx*)ctx;
    c->total[0] = 0;
    c->total[1] = 0;
    c->state[0] = 0x67452301;
    c->state[1] = 0xefcdab89;
    c->state[2] = 0x98badcfe;
    c->state[3] = 0x10325476;
