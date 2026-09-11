CaptchaGen::CaptchaGen()
    : m_library(0)   // ORIG 0x080de697：m_library 清零先于 vector/XorRand 成员
                     // ctor —— mem-init（声明序）而非 body 赋值。
{
    // ORIG 0x080de6bf：FT_Init_FreeType(&m_library)（+0 ⇒ this 同值）结果落
    // 临时槽 -0xc(%ebp)（不检查，dead store 具名化）；随后 seed(now)。
    int err = FT_Init_FreeType(&m_library);
    (void)err;
    m_rng.seed(Tick::nowAsInternalValue());
    // ORIG 0x080de6de：seed call 后 jmp 越过 EH island 到尾部 epilogue——
    // 函数尾显式 return（技巧 7 尾岛）。
    return;
}