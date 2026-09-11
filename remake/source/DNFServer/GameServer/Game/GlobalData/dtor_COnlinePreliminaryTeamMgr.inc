// ORIG T 0x085886fe：m_factory 非空时 delete（CTeamFactory D1 + operator delete）；
// m_teamMap@+0x04 由编译器析构（ORIG 尾调 map D1）。
online_preliminary::COnlinePreliminaryTeamMgr::~COnlinePreliminaryTeamMgr()
{
    // ORIG 0x08588706/0x0858870f：双 load 双判空——4.4 对
    // `if (m_factory) delete m_factory;` 恒产此形（probe 实证）。
    if (m_factory)
    {
        delete m_factory;
    }
}