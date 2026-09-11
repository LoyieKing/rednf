int CStackableItem::check_able_To_use(int job, int level)
{
    if (!check_job_type((char)job))
    {
        return 1;
    }
    if (!check_low_level(level))
    {
        return 2;
    }
    return 0;
}