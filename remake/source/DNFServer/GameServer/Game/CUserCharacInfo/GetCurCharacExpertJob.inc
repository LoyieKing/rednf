expert_job::CExpertJob* CUserCharacInfo::GetCurCharacExpertJob() const
{
    return m_selected ? CUR->m_expertJob : 0;
}