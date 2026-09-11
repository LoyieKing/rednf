int CUserCharacInfo::get_level_up_exp(int level)
{
    return G_CDataManager()->get_level_exp(level + 1) - G_CDataManager()->get_level_exp(level);
}