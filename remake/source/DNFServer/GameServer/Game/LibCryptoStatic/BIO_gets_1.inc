// ---- BIO_gets（ORIG T 0x087eb?，meth->bgets 分发型）----
long BIO_gets(void* bp, char* buf, int size)
{
    bio_st_st* b = static_cast<bio_st_st*>(bp);
    // ORIG 87ebd4f..d66：b/method/bgets 缺失 → ERR(0x20,0x68,0x79) + -2
    // （87ebdf0..e21）；我们原实现缺 init 检查——fopen 失败（init=0）后仍调
    // file_gets → fgets(NULL) 崩溃（冒烟实测 SIGSEGV）。对齐 ORIG 87ebd9c：
    // init==0 → ERR(0x20,0x68,0x78,"bio_lib.c",0x135) + -2（87ebe23）。
    if (b == 0 || b->method == 0 || b->method->bgets == 0 || size <= 0)
    {
        ERR_put_error(0x20, 0x68, 0x79, "bio_lib.c", 0x129);
        return -2;
    }
    if (b->callback != 0)                              // ORIG 87ebd6c..9a
    {
        long cb = b->callback(b, 5, buf, size, 0, 1L);
        if (cb <= 0)
            return static_cast<int>(cb);
    }
    if (b->init == 0)
    {
        ERR_put_error(0x20, 0x68, 0x78, "bio_lib.c", 0x135);
        return -2;
    }
    long r = b->method->bgets(b, buf, size);
    if (b->callback != 0)
        return static_cast<int>(b->callback(b, 0x85, buf, size, 0, r));
    return static_cast<int>(r);
}