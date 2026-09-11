// ORIG 0x08331ea2：delete m_rand 并置空；11 张表逆序析构（编译器生成）。
CCompoundAvatar::~CCompoundAvatar()
{
    if (m_rand != 0)
    {
        delete m_rand;
    }
    m_rand = 0;
}