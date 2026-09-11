// ===========================================================================
// 伴生：BIO_clear_flags @0x087eb7d0 (0xe)
// ===========================================================================
int BIO_clear_flags(bio_st_st* b, int flags)
{
    // mov flags,%eax / not / and %eax,(b->flags+0x14)
    b->flags &= ~flags;
    return 1;                          // 函数返回值由调用方忽略（eax 残留）
}