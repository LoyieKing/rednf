int CEquipItem::check_able_To_equip(char eType, int job, int level)
{
    if (is_stackable())
    {
        return 1;
    }
    if (!check_eType(eType))
    {
        return 0x17;
    }
    if (!check_job_type((char)job))
    {
        return 8;
    }
    if (!check_low_level(level))
    {
        return 0xe;
    }
    return 0;
}