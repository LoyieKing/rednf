int ENGINE_finish(void* engine)                      // ORIG T 0x087eb4a0
{
    void** e = static_cast<void**>(engine);
    // [Q7S1 语义轴 · SIG] ORIG 0x087eb548..0x087eb574（e==NULL 分支）：
    //   ERR_put_error(0x26,0x6b,0x43,"eng_init.c",0x8e) 后 `mov %ebx,%eax`（ebx=0）
    //   ⇒ **返回 0**。旧稿此处直接 `return 1;` 且无 ERR ⇒ 返回值与错误队列双双背离
    //   ORIG（发明防御 / 漏校验）。engine_unlocked_finish 一律不被调用。
    if (e == 0)
    {
        ERR_put_error(0x26, 0x6b, 0x43, "eng_init.c", 0x8e);
        return 0;
    }
    // ORIG 0x087eb4b0..0x087eb4cf / 0x087eb4e4..0x087eb505：两处 CRYPTO_lock 的
    // file 实参均为 0x8de22d2（实读 "eng_init.c"，非 eng_lib.c），line 0x91/0x93。
    CRYPTO_lock(9, 30, "eng_init.c", 145);
    const int ret = engine_unlocked_finish_kl(e);
    CRYPTO_lock(10, 30, "eng_init.c", 147);
    // ORIG 0x087eb515..0x087eb53c：ERR_put_error(0x26,0x6b,0x6a,"eng_init.c",0x96)。
    // 旧稿 (38,107,150,...,151) ⇒ func/reason 移位（107/150 应为 0x6b/0x6a）+ line 错。
    if (!ret)
        ERR_put_error(0x26, 0x6b, 0x6a, "eng_init.c", 0x96);
    return ret;
}