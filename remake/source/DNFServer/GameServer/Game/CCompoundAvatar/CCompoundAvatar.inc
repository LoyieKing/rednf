CCompoundAvatar::CCompoundAvatar()
{
    m_rand = new CMTRand(static_cast<unsigned long>(time(0)));
    for (int i = 0; i < 100; ++i)
    {
        // ORIG 0x08331e35：range 为绑定到 `const unsigned long&` 的匿名临时
        // （movl $0x2710,-0x20(%ebp) + lea 取址），非具名局部。
        m_rand->randInt(static_cast<unsigned long>(10000));
    }
}