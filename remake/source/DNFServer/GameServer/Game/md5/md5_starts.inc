void md5_starts(md5_context* ctx)
{
    // P183：不缓存视图指针（ORIG 0x080b2ae0 每次 `mov 0x8(%ebp),%eax` 重载 ctx）。
    ((md5_ctx*)ctx)->total[0] = 0;
    ((md5_ctx*)ctx)->total[1] = 0;
    ((md5_ctx*)ctx)->state[0] = 0x67452301;
    ((md5_ctx*)ctx)->state[1] = 0xefcdab89;
    ((md5_ctx*)ctx)->state[2] = 0x98badcfe;
    ((md5_ctx*)ctx)->state[3] = 0x10325476;
}