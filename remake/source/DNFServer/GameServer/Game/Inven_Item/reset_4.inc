void UpgradeSeparateInfo::reset()
{
    // ORIG 0x80cb346：位域 b0:5/b1:1/b2:2 清零 → 三次位清
    // （&0xffffffe0 → &0xffffffdf → &0x3f），整字节清零。
    struct Bits { unsigned char a:5, b:1, c:2; };
    reinterpret_cast<Bits*>(&m_field)->a = 0;
    reinterpret_cast<Bits*>(&m_field)->b = 0;
    reinterpret_cast<Bits*>(&m_field)->c = 0;
