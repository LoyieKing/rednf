void WriteChampionFeatures(FILE* file, STAssignMonster& monster)
{
    fprintf(file, "%d ", static_cast<int>(monster.m_vec28.size()));
    for (size_t i = 0; i < monster.m_vec28.size(); ++i)
        fprintf(file, "`%s` ", CHAMPION_FEATURE_NAME[monster.m_vec28[i] & 0xff]);
    fputc('\n', file);
