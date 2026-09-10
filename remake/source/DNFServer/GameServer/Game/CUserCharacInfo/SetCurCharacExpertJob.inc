void CUserCharacInfo::SetCurCharacExpertJob(expert_job::CExpertJob* job)
{
    if (m_selected)
    {
    CUR->m_expertJob = job;
    }
