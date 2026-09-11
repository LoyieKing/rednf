// ORIG execute 0x084a9bd0。
void CChangeExpertJobExpCmd::execute()
{
    expert_job::CExpertJob::IncreaseExpertJobExp(GetUser(), m_exp);
}