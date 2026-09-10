CExpertJob* CExpertJobMgr::AcquireExpertJob(ENUM_EXPERT_JOB_TYPE type)
{
    return m_factory->AcquireExpertJob(type);
