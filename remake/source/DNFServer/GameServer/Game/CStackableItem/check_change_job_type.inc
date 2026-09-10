bool CStackableItem::check_change_job_type(char changeJob,
                                           char checkJob) const
{
    if (!check_job_type(checkJob))
    {
        return 0;
    }
    if ((char)m_arad148.m_field4 < 0)
    {
        return 1;
    }
    return (char)m_arad148.m_field4 == changeJob;
