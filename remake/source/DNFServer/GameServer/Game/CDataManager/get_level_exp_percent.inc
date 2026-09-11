int CDataManager::get_level_exp_percent(int level, int percent) const
{
    int section = get_level_section_exp(level);
    return section * percent / 100;
}