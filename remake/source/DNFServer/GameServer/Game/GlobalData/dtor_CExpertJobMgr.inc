expert_job::CExpertJobMgr::~CExpertJobMgr()
{
    // ORIG 0x0849e1ac：外层 guard + delete 自身 guard（两次非空检查），无清零。
    if (m_factory != 0)
        delete m_factory;
}