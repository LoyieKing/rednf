online_preliminary::COnlinePreliminaryTeamMgr::~COnlinePreliminaryTeamMgr()
{
    // ORIG 0x08588706/0x0858870f：双 load 双判空——4.4 对
    // `if (m_factory) delete m_factory;` 恒产此形（probe 实证）。
    if (m_factory)
    {
        delete m_factory;
    }
