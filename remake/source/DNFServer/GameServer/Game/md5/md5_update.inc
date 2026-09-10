void md5_update(md5_context* ctx, unsigned char* data, int len)
{
    md5_ctx* c = (md5_ctx*)ctx;
    unsigned int used = (c->total[0] >> 3) & 0x3f;
    c->total[0] += (unsigned int)len << 3;
    if (c->total[0] < ((unsigned int)len << 3))
    {
        ++c->total[1];
    }
    c->total[1] += (unsigned int)len >> 29;
    if (used != 0)
    {
        unsigned int fill = 64 - used;
        if (len < (int)fill)
        {
            memcpy(c->buffer + used, data, len);
            return;
        }
        memcpy(c->buffer + used, data, fill);
        data += fill;
        len -= (int)fill;
        used = 0;
        md5_ctx* p = (md5_ctx*)ctx;
        unsigned int* x = (unsigned int*)p->buffer;
        unsigned int a = p->state[0], b = p->state[1];
        unsigned int cc = p->state[2], d = p->state[3];
