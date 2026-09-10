int CDataManager::get_creature_exp_level(int exp) const
{
    int level = 1;
    for (; level <= 98; level += 1)
    {
        if (get_creature_level_exp(level) > exp)
        {
            break;
        }
    }
    return level - 1;
