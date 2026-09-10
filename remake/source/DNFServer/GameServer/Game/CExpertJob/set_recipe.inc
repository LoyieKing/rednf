int CCharacExpertJob::set_recipe(int index)
{
    int idx = index / 8;
    int bit = index % 8;   // ORIG 保留完整除余链（mov/sar/shr/add/and/sub）
    if (idx < 0 || idx > 0x1d)
        return 0;
    // b28 P222 两形互试：负极早返（ORIG je→set 岛形，al=1 即失败直落 mov$0）。
    if ((((int)(char)m_recipeFlag[idx] >> bit) & 1) != 0)
        return 0;
    m_recipeFlag[idx] |= (unsigned char)(1 << bit);
    return 1;
