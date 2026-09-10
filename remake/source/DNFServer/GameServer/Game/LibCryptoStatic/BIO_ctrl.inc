int BIO_ctrl(void* a, int cmd, long larg, void* parg)
{
    static const char kFile[] = "bio_lib.c";
    bio_st_st* b = static_cast<bio_st_st*>(a);

    if (b == 0)                                      // 87eba75
        return 0;

    bio_method_st* m = b->method;
    if (m == 0 || m->ctrl == 0) {                    // 87eba81/8b
        ERR_put_error(0x20, 0x67, 0x79, kFile, 0x168);
        return -2;                                   // 87ebb43
    }

    if (b->callback != 0) {                          // 87eba96..ac
        long cb_ret = b->callback(b, 0x06,
                                  static_cast<char*>(parg),
                                  cmd, larg, 1L);
        if (cb_ret <= 0)
            return static_cast<int>(cb_ret);         // 87ebac8
    }

    long ctrl_ret = m->ctrl(static_cast<bio_st_st*>(a), cmd, larg, parg); // 87ebadf

    if (b->callback != 0) {
        return static_cast<int>(
            b->callback(b, 0x86, static_cast<char*>(parg),
                        cmd, larg, ctrl_ret));       // 87ebb01 ORIG 以其值为返回
    }
    return static_cast<int>(ctrl_ret);
