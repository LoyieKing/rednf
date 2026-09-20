expert_job::CExpertJob* CUserCharacInfo::GetCurCharacExpertJob() const
{
    return m_selected ? m_selected->m_fieldE85 : 0;
}