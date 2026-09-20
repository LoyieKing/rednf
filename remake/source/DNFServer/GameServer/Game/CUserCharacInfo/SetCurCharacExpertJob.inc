void CUserCharacInfo::SetCurCharacExpertJob(expert_job::CExpertJob* job)
{
    if (m_selected)
    {
    m_selected->m_fieldE85 = job;
    }
}