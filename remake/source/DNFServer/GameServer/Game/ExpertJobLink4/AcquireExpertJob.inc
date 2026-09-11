// ---- CExpertJobMgr::AcquireExpertJob（ORIG T 0x0849e1dc：纯转发工厂）----
CExpertJob* CExpertJobMgr::AcquireExpertJob(ENUM_EXPERT_JOB_TYPE type)
{
    return m_factory->AcquireExpertJob(type);
}