int IMethod::SetMode(int iMode)
{
    // ORIG 0x080b436e..0x080b43a7 逐指令：`movzbl +0x4; xor $1; test; je` ⇒
    // 判 m_bInit（未初始化 0x70000005）；`cmpl $0 js / cmpl $2 jle` ⇒
    // `iMode < 0 || iMode > 2` 短路双检（0x70000003）；ok 路径 `mov %edx,
    // 0x10(%eax)` 写 m_mode 后返 0x6fffffff。
    if (!m_bInit)
        return 0x70000005;
    if (iMode < 0 || iMode > 2)
        return 0x70000003;
    m_mode = iMode;
    return 0x6fffffff;
}