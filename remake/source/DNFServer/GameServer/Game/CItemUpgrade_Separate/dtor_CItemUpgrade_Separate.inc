// ORIG T 0x0811e0be：delete m_rand 置空 → 表析构 → 基类析构（成员逆序自动）。
CItemUpgrade_Separate::~CItemUpgrade_Separate()
{
    if (m_rand != 0)
    {
        delete m_rand;
    }
    m_rand = 0;
}