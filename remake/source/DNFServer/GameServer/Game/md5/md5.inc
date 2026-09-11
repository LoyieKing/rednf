// 一步式封装（ORIG T _Z3md5PhiS_ @0x080b3d57：starts→update(data,len)→finish→
// memset(ctx,0,0xd8)；GameSecurity::_NS_MakeHash_ 0x0808c8f0 唯一调用点）。
void md5(unsigned char* data, int len, unsigned char* digest)
{
    md5_context ctx;
    md5_starts(&ctx);
    md5_update(&ctx, data, len);
    md5_finish(&ctx, digest);
    memset(&ctx, 0, 0xd8);
}