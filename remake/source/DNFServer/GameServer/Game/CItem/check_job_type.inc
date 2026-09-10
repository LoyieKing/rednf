bool CItem::check_job_type(char job) const
{
    if (job > 10)
    {
        return 0;
    }
    int idx = job;
    return m_jobUsable[idx];
