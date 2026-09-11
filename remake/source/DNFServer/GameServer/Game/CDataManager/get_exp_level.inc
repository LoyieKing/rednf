int CDataManager::get_exp_level(int exp) const
{
    int level = 1;
    for (; level <= 99; level += 1)
    {
        if (get_level_exp(level) > exp)
        {
            break;
        }
    }
    return level - 1;
}