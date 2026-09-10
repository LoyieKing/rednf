bool CCharacExpertJob::get_recipe(int index) const
{
    int idx = index / 8;
    int bit = index % 8;
    if (idx < 0 || idx > 0x1d)
        return 0;
    return (((int)(char)m_recipeFlag[idx] >> bit) & 1);
