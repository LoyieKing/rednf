bool CAICharacter::IsHellMonster() const
{
    // [b28c-35] ORIG 0x08539de8 为单条 `movzbl 0x70(%eax),%eax`；裸偏移算术链
    // （reinterpret_cast + 0x70）4.4 发 `add $0x70,%eax; movzbl (%eax)`（+1 条）。
    // 改具名尾部视图成员访问（P82）即折叠单条 disp；视图只读该字节，不改类布局。
    struct Tail { char m_pad[0x70]; bool m_flag; };
    return reinterpret_cast<const Tail*>(this)->m_flag;
